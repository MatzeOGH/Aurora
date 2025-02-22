#include <iostream>

#include <vulkan/vulkan.h>

#include "platform/platform.h"
#include "renderer/renderer.h"

#include "core/arena.h"


void AuroraMain() {
	Arena persistent = Arena::makeArena(100_MiB);
	Arena scratch = Arena::makeArena(10_MiB);

	// init the platform layer
	aurora::platform::init({ .enableConsole = true });

	aurora::renderer::init({}, &persistent, scratch);

	do {

	} while (true);

	aurora::renderer::shutdown();

	Arena::destroyArena(scratch);
	Arena::destroyArena(persistent);
}