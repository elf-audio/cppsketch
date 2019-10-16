

#include <thread>
#include <unistd.h>


#include "RecompilingDylib.h"
#include "liveCodeUtils.h"
#include "LiveApp.h"


LiveApp *app;


int main(int argc, char * argv[]) {
	liveCodeUtils::init();

	RecompilingDylib dylib;

	dylib.successCallback = []() {
		app->setup();
	};

	dylib.failureCallback = [](string msg) {
		app = nullptr;
	};

	dylib.recompiled = [](void *ptr) {
		app = (LiveApp*) ptr;
	};

	
// separate watcher thread?
	dylib.setup("MyLiveApp.h");

 	std::thread t([&](){
 		while(1) {
 			dylib.update();
 			if(app!=nullptr) {
 				app->update();
 			}
			usleep(1000*1000);
		}
    });
    t.join();
	return 0;
}