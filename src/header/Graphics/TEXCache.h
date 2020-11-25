#pragma once

#include <entt/resource/cache.hpp>
#include <entt/resource/loader.hpp>

#include "Graphics/TEX.h"

namespace LOA::Graphics {
	struct TextureLoader : entt::resource_loader<TextureLoader, TEX> {
		std::shared_ptr<TEX> load(std::string filename) const {
			return std::make_shared<TEX>(TEX::Builder().buildTexture(filename));
		}

		std::shared_ptr<TEX> load(TEX::Builder settings, std::string filename) const {
			return std::make_shared<TEX>(settings.buildTexture(filename));
		}
	};

	using TEXCache = entt::resource_cache<TEX>;
}