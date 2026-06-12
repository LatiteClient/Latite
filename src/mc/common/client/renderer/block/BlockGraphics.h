#pragma once


#include <cstdint>

#include "mc/Addresses.h"
#include "mc/common/client/renderer/texture/TextureUVCoordinateSet.h"
#include "mc/common/world/level/block/Block.h"
#include "util/LMath.h"

namespace SDK {
	class BlockGraphics {
	public:
		static BlockGraphics const *getForBlock(Block const &block) {
			auto fn = reinterpret_cast<BlockGraphics const*(__fastcall*)(Block const *)>(
				Signatures::BlockGraphics_getForBlock.result);
			return fn ? fn(&block) : nullptr;
		}

		TextureUVCoordinateSet const *getTexture(std::uint64_t textureSlot, unsigned int textureVariant = 0) const {
			auto fn = reinterpret_cast<TextureUVCoordinateSet const*(__fastcall*)(
				BlockGraphics const *, std::uint64_t, unsigned int)>(Signatures::BlockGraphics_getTexture.result);
			return fn ? fn(this, textureSlot, textureVariant) : nullptr;
		}

		TextureUVCoordinateSet const *getTextureAt(BlockPos const &pos, std::uint64_t textureSlot,
		                                           unsigned int textureVariant = 0) const {
			auto fn = reinterpret_cast<TextureUVCoordinateSet const*(__fastcall*)(
				BlockGraphics const *, BlockPos const *, std::uint64_t, unsigned int)>(
				Signatures::BlockGraphics_getTextureAtPos.result);
			return fn ? fn(this, &pos, textureSlot, textureVariant) : getTexture(textureSlot, textureVariant);
		}
	};
}
