

#include <thread>
#include <unistd.h>

#include "ReloadableClass.h"
#include "LiveApp.h"

LiveApp *app = nullptr;

int main(int argc, char * argv[]) {


	ReloadableClass<LiveApp> dylib;


	dylib.reloaded = [](LiveApp *ptr) {
		app = ptr;
		app->setup();
	};

	dylib.reloadFailed = [](const string &msg) {
		app = nullptr;
	};
	
	dylib.init("MyLiveApp.h");

	
 	std::thread t([&](){
 		while(1) {
 			dylib.update();
 			if(app!=nullptr) {
 				app->update();
 			}
		}
    });
    t.join();
	return 0;
}