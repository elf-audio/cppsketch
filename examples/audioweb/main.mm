

#import "AppDelegate.h"

int main(int argc, char * argv[]) {

	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	
	
	[NSApp activateIgnoringOtherApps:YES];
	id appDelegate = [[AppDelegate alloc] init];
	[NSApp setDelegate:appDelegate];
	[NSApp run];
	return 0;
}
