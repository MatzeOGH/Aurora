#pragma once

#include "core/arena.h"

namespace Aurora::renderer {

	struct RendererCreateInfo {
	};

	void init(const RendererCreateInfo& rendererCreateInfo, Arena* persistane, Arena scratch);

	void RegisterWindow(addptr window);

	void shutdown();

} // namespace aurora::renderer