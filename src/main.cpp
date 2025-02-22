#include <iostream>

#include <vulkan/vulkan.h>

#include "platform/platform.h"
#include "platform/window.h"
#include "renderer/renderer.h"
#include "core/logger.h"
#include "core/arena.h"


void AuroraMain() {
	Arena persistent = Arena::makeArena(100_MiB);
	Arena scratch = Arena::makeArena(10_MiB);

	// init the platform layer
	Aurora::platform::init({ .enableConsole = true });
	Aurora::renderer::init({}, &persistent, scratch);

	Aurora::platform::createWindow({});

	i32 frame = 0;
	do {
		Aurora::platform::update();
		//LOG("frame %d", frame);
		frame++;
	} while (Aurora::platform::getWindowCount() > 0);

	Aurora::renderer::shutdown();

	Arena::destroyArena(scratch);
	Arena::destroyArena(persistent);
}