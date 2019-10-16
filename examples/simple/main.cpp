

#include "ReloadableClass.h"
#include "LiveApp.h"

LiveApp *app = nullptr;

int main(int argc, char * argv[]) {

	ReloadableClass<LiveApp> dylib;

	dylib.reloaded = [](LiveApp *ptr) {
		app = ptr;
		app->setup();
	};
	
	dylib.init("MyLiveApp.h");

	while(1) {
		dylib.checkForChanges();
		if(app!=nullptr) {
			app->update();
		}
	}

	return 0;
}