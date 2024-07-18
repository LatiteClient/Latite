#include "pch.h"
#include "ItemTweaks.h"

ItemTweaks::ItemTweaks() : Module("ItemTweaks", LocalizeString::get("client.module.itemTweaks.name"), LocalizeString::get("client.module.itemTweaks.desc"), GAME) {
	addSetting("disableFlyingItem", LocalizeString::get("client.module.itemTweaks.disableFlyingItem.name"), LocalizeString::get("client.module.itemTweaks.disableFlyingItem.desc"), this->disableFlyingItemAnimation);

	listen<RenderGuiItemEvent>((EventListenerFunc)&ItemTweaks::onItemRender);
}

void ItemTweaks::onItemRender(Event& ev) {
	if (std::get<BoolValue>(disableFlyingItemAnimation))
		reinterpret_cast<RenderGuiItemEvent&>(ev).setCancelled();
}
