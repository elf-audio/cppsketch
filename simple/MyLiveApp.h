#include "LiveApp.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <set>


class MyLiveApp : public LiveApp {
public:

	void setup() override {
		printf("setup\n");
	}

	int i = 0;

	void update() override { 
		i++;
		printf("update %d\n", i);
	} 
};