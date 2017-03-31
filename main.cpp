#include "DynamicSoundtrackDemoAppApp.h"

int main() {
	
	auto app = new DynamicSoundtrackDemoAppApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}