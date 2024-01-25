#include "pch.h"
#include "ItemTweaks.h"

ItemTweaks::ItemTweaks() : Module("ItemTweaks", "Item Tweaks", "Tweaks/settings related to item rendering", GAME)  {
}

void ItemTweaks::onItemRender(Event& ev) {
	reinterpret_cast<RenderGuiItemEvent&>(ev).setCancelled();
}
