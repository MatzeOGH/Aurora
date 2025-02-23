#pragma once

#include "core/arena.h"
#include "platform/window.h"

namespace Aurora::Renderer {

	struct RendererCreateInfo {
	};

	void init(const RendererCreateInfo& rendererCreateInfo, Arena* persistane, Arena scratch);

	void registerWindow(Aurora::Window window);
	void unregisterWindow(Aurora::Window window);

	void shutdown();

} // namespace aurora::renderer