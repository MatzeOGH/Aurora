#include <iostream>

#include <vulkan/vulkan.h>

#include "platform/platform.h"
#include "platform/window.h"
#include "renderer/renderer.h"
#include "core/logger.h"
#include "core/arena.h"


namespace Aurora {

	void AuroraMain() {
		Arena persistent = Arena::makeArena(100_MiB);
		Arena scratch = Arena::makeArena(10_MiB);

		// init the platform layer
		Platform::init({ .enableConsole = true });
		Renderer::init({}, &persistent, scratch);

		Window window = Platform::createWindow({});
		Renderer::registerWindow(window);

		i32 frame = 0;
		do {
			Platform::update();
			frame++;
		} while (Platform::getWindowCount() > 0);

		Renderer::unregisterWindow(window);
		Renderer::shutdown(scratch);

		Arena::destroyArena(scratch);
		Arena::destroyArena(persistent);
	}

}