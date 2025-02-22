#pragma once

#include "core/string.h"

namespace aurora::platform
{
	struct PlatformCreateInfo{
		bool enableConsole{ true };
	};

	void init(const PlatformCreateInfo& platformCreateInfo);

	// launch a process
	void launchProcess(String path);
}
