#include "pch.h"
#include "ItemTweaks.h"

ItemTweaks::ItemTweaks() : Module("ItemTweaks", "Item Tweaks", "Tweaks/settings related to item rendering", GAME)  {
	addSetting("disableFlyingItem", "Disable flying item animation", "Disables the animation when you shift click an item", this->disableFlyingItemAnimation);

	listen<RenderGuiItemEvent>((EventListenerFunc)&ItemTweaks::onItemRender);
}

void ItemTweaks::onItemRender(Event& ev) {
	if (std::get<BoolValue>(disableFlyingItemAnimation))
		reinterpret_cast<RenderGuiItemEvent&>(ev).setCancelled();
}
