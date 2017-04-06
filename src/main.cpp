#include "DynamicSoundtrackDemoAppApp.h"

int main() {
	
	auto app = new DynamicSoundtrackDemoAppApp();
	app->run("Dynamic Soundtrack Demo App", 1280, 720, false);
	delete app;

	return 0;
}