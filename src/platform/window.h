#pragma once

#include "core/string.h"

namespace Aurora::platform
{
	struct WindowCreateInfo {
		String title{};
		i32 width{};
		i32 height{};
		bool fullscreen{ false };
	};

	addptr createWindow(const WindowCreateInfo& windowCreateInfo);

	i32 getWindowCount();

}
