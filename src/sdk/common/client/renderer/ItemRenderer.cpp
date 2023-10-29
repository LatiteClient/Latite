#include "ItemRenderer.h"

void SDK::ItemRenderer::renderGuiItemNew(SDK::BaseActorRenderContext* baseActorRenderContext, SDK::ItemStack* itemStack, int mode, float x, float y, float opacity, float scale, float a9, bool ench) {
    using oFunc_t = void(__fastcall*)(ItemRenderer*, BaseActorRenderContext* baseActorRenderContext, ItemStack* itemStack, int, float x, float y, bool ench, float opacity, float a9, float scale);
	static auto oFunc = reinterpret_cast<oFunc_t>(0);
	oFunc(this, baseActorRenderContext, itemStack, mode, x, y, ench, opacity, 0, scale);
}