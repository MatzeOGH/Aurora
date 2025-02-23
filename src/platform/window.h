#pragma once

#include "core/string.h"

namespace Aurora
{
	struct WindowCreateInfo {
		String title{};
		i32 width{};
		i32 height{};
		bool fullscreen{ false };
	};

	struct Window
	{
		addptr handle;
	};

	namespace Platform
	{

		Window createWindow(const WindowCreateInfo& windowCreateInfo);

		void closeWindow(Window& window);

		i32 getWindowCount();
	}

}
