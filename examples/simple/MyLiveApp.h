
#include "LiveApp.h"

#include <stdio.h>
#include <unistd.h>

class MyLiveApp : public LiveApp {
public:

	void setup() override {
		printf("setup\n");
	}

	int i = 0;

	void update() override { 
		i+=1;
		printf("update %d\n", i);

		// sleep for 0.1 seconds
		usleep(1000*1000);
	}
};
