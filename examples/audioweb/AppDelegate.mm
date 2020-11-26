//
//  AppDelegate.m
//  wk
//
//  Created by Marek Bereza on 19/11/2020.
//

#import "AppDelegate.h"
#import <WebKit/WebKit.h>
#include <functional>
#include <vector>
#include <string>
#include <math.h>

using namespace std;



#include <unistd.h>
#include "ReloadableClass.h"
#include "LiveAudioWeb.h"
#include "RtAudio.h"
#include <mutex>
#import "AppDelegate.h"

LiveAudioWeb *audio = nullptr;
ReloadableClass<LiveAudioWeb> dylib;

mutex audioMutex;

// Two-channel sawtooth wave generator.
int audioOutCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData) {
	float *output = (float*) outputBuffer;
	audioMutex.lock();
	if(audio) {

		audio->audioOut(output, nBufferFrames, 2);
	} else {

		memset(output, 0, nBufferFrames * 2 * sizeof(float));
	}
	audioMutex.unlock();
	return 0;
}


RtAudio rtAudio;

void startRtAudio() {
	RtAudio::StreamParameters parameters;
	parameters.deviceId = rtAudio.getDefaultOutputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	unsigned int bufferFrames = 256; // 256 sample frames
	
	try {
		rtAudio.openStream( &parameters, NULL, RTAUDIO_FLOAT32, 44100, &bufferFrames, &audioOutCallback, nullptr );
		rtAudio.startStream();
	} catch ( RtAudioError& e ) {
		e.printMessage();
		exit( 0 );
	}
}




@interface LambdaMenuItem : NSMenuItem {
	function<void()> actionLambda;
}
	
- (id)initWithTitle:(NSString *)title keyEquivalent:(NSString *)keyEquivalent actionLambda: (function<void()>) func;
@end

@interface LambdaMenuItem ()
- (void)menuAction:(NSMenuItem *)item;
@end

@implementation LambdaMenuItem
	
- (id)initWithTitle:(NSString *)title keyEquivalent:(NSString *)keyEquivalent actionLambda:(function<void()>) func {
	self = [super initWithTitle:title action:@selector(menuAction:) keyEquivalent:keyEquivalent];
	if (self) {
		actionLambda = func;
		[self setTarget:self];
	}
	return self;
}
	

	
- (void)menuAction:(NSMenuItem *)item {
	if ((item == self) && (actionLambda)) {
		actionLambda();
	}
}
@end




@interface AppDelegate () {
	WKWebView *webView;
	NSWindow *window;
}

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
	[self makeWindow];
	[self makeMenus];
	[self startCppSketch];
}


- (void) startCppSketch {

	dylib.willCloseDylib = []() {
		audioMutex.lock();
		audio = nullptr;
		audioMutex.unlock();
	};
	dylib.reloaded = [self](LiveAudioWeb *ptr) {
		audioMutex.lock();
		audio = ptr;
		ptr->jsExternalCall = [self](const std::string &s) {
			// NSLog(@"Executing %s", s.c_str());
			[webView evaluateJavaScript:[NSString stringWithUTF8String:s.c_str()] completionHandler:nil];
		};
		audio->setup();
		audioMutex.unlock();
	};
	
	dylib.init("MyLiveAudioWeb.h");
	

	startRtAudio();

	[NSTimer scheduledTimerWithTimeInterval:1.0
	    target:self
	    selector:@selector(checkForChanges)
	    userInfo:nil
	    repeats:YES];

	[NSTimer scheduledTimerWithTimeInterval:1.0/60.0
	    target:self
	    selector:@selector(update)
	    userInfo:nil
	    repeats:YES];
}
- (void) update {
	audioMutex.lock();
	if(audio!=nullptr) {
		audio->update();
	}
	audioMutex.unlock();
}
- (void) checkForChanges {
	dylib.checkForChanges();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}





- (void) makeMenus {

	NSString *appName = [[NSProcessInfo processInfo] processName];
	
	NSApp.mainMenu = [[NSMenu alloc] initWithTitle: @"MainMenu"];
	
	NSMenuItem *menuItem = [NSApp.mainMenu addItemWithTitle:appName action:nil keyEquivalent:@""];
	NSMenu *appMenu = [[NSMenu alloc] initWithTitle:appName];
	[NSApp.mainMenu setSubmenu:appMenu forItem:menuItem];
	
	
	
	
	id reloadItem = [[LambdaMenuItem alloc] initWithTitle: @"Reload"
									  keyEquivalent:@"r" actionLambda:[self]() {
		[self reload];
	}];

	[appMenu addItem: reloadItem];
	
	[appMenu addItem: [NSMenuItem separatorItem]];


	id quitItem = [[LambdaMenuItem alloc] initWithTitle: @"Quit"
									  keyEquivalent:@"q" actionLambda:[]() {
		[[NSApplication sharedApplication] terminate:nil];
	}];

	[appMenu addItem: quitItem];
}

- (void) reload {
	NSLog(@"Reload %s", __FILE__);
	[webView reload];
}

- (void) makeWindow {
	
	
	NSRect windowRect = NSMakeRect(0, 0, 800, 600);
	
	window = [[NSWindow alloc]
				 initWithContentRect:windowRect
				 styleMask:NSTitledWindowMask | NSWindowStyleMaskResizable | NSWindowStyleMaskClosable backing:NSBackingStoreBuffered defer:NO];
	window.acceptsMouseMovedEvents = YES;
	
	id appName = [[NSProcessInfo processInfo] processName];
	
	
	[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[window setTitle:appName];
	WKWebViewConfiguration *config = [[WKWebViewConfiguration alloc] init];
	
	[config.userContentController addScriptMessageHandler:self name:@"updateHandler"];
	


	
	webView = [[WKWebView alloc] initWithFrame:windowRect configuration:config];
	
	
	NSURL *baseURL = [[NSBundle mainBundle] resourceURL];
	NSString *path = [[NSFileManager defaultManager] currentDirectoryPath];
	path = [path stringByAppendingString: @"/index.html"];
	NSURL *url = [NSURL fileURLWithPath: path];
	// NSURL *url = [[NSBundle mainBundle] URLForResource:@"index" withExtension:@"html"];
	[webView loadFileURL:url allowingReadAccessToURL:baseURL];
	// [webView loadHTMLString:@"<html><body style='background-color:red'><p>Hello!</p></body></html>" baseURL:baseURL];
	
//

	[[window contentView] addSubview:webView];
	[window makeKeyAndOrderFront:nil];
	[window makeMainWindow];

}


- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
	// std::string s;
	NSDictionary *dict = message.body;
	NSString *func = dict[@"function"];
	// NSLog(@"%@", dict[@"args"]);
	NSArray *args = dict[@"args"];

	string funcStr = string([func UTF8String]);
	vector<string> params;
	for(int i = 0; i < [args count]; i++) {
		id s = [args objectAtIndex: i];
		if([s isKindOfClass: [NSString class]]) {
			params.push_back([s UTF8String]);
		} else {
			params.push_back([[s stringValue] UTF8String]);
		}
	}
	audioMutex.lock();
	if(audio!=nullptr) {
		audio->jsReceived(funcStr, params);
	}
	audioMutex.unlock();
	// [webView evaluateJavaScript:[NSString stringWithUTF8String:s.c_str()] completionHandler:nil];

}
@end
