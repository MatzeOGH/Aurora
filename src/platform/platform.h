#pragma once

#include "core/string.h"

namespace Aurora::Platform
{
	struct PlatformCreateInfo{
		bool enableConsole{ true };
	};

	void init(const PlatformCreateInfo& platformCreateInfo); 

	void update();

	// launch a process
	void launchProcess(String path);
}
