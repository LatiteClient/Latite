#include "pch.h"
#include "ArmorHUD.h"

ArmorHUD::ArmorHUD() : HUDModule("ArmorHUD", LocalizeString::get("client.hudmodule.armorHud.name"),
                                 LocalizeString::get("client.hudmodule.armorHud.desc"), HUD) {
	mode.addEntry(EnumEntry{ mode_vetical, LocalizeString::get("client.hudmodule.armorHud.modeVertical.name") });
	mode.addEntry(EnumEntry{ mode_horizontal, LocalizeString::get("client.hudmodule.armorHud.modeHorizontal.name") });
	addEnumSetting("mode", LocalizeString::get("client.hudmodule.armorHud.mode.nam e"),
                   LocalizeString::get("client.hudmodule.armorHud.mode.desc"), mode);
	addSetting("durability", LocalizeString::get("client.hudmodule.armorHud.durability.name"),
               LocalizeString::get("client.hudmodule.armorHud.durability.desc"), showDamage);
	addSetting("percent", LocalizeString::get("client.hudmodule.armorHud.percent.name"),
               LocalizeString::get("client.hudmodule.armorHud.percent.desc"), this->percentage, "durability"_istrue);

	std::get<ColorValue>(txtColor).color1 = { 1.f, 1.f, 1.f, 1.f };
	addSetting("textColor", LocalizeString::get("client.hudmodule.armorHud.textColor.name"),
               LocalizeString::get("client.hudmodule.armorHud.textColor.desc"), txtColor);
}

void ArmorHUD::render(DrawUtil& ctxGeneric, bool isDefault, bool inEditor) {
	auto& dc = reinterpret_cast<MCDrawUtil&>(ctxGeneric);
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;

	if (!ctxGeneric.isMinecraft()) {
		rect = { rect.left, rect.top, rect.left, rect.top };
		return;
	}
	
	//dc.setFont(Latite::Get().getFont(Latite::Font::Minecraft));
	//dc.setTextShadow(true);

	std::array<SDK::ItemStack*, 6> items = {};
	// mainhand, offhand, boot, leggings, chestplate, helmet

	if (auto sl = plr->supplies->inventory->getItem(plr->supplies->selectedSlot)) {
		items[0] = sl;
	}
	//if (auto off = plr->getEquippedTotem()) {
	//	items[1] = off;
	//}
	for (int i = 2; i < 6; i++) {
		int armor = i - 2;
		auto arm = plr->getArmor(3 - armor);
		if (arm->item) items[i] = arm;
	}

	int numItems = 0;
	float itemSize = 48.f;
	float pad = 15.f;

	float extra = 0.f;

	float highest = extra;

	bool isLeft = this->getRect().centerX() > (SDK::ClientInstance::get()->getGuiData()->screenSize.x / 2.f);

	float textS = 30.f;

	if (mode.getSelectedKey() == (mode_vetical)) {
		for (auto& item : items) {
			if (item && item->item) {
				numItems++;

				// Didn't want to have to do this but it's the only good way
				auto it = *item->item;
				int mdmg = it->getMaxDamage();
				int dmg = mdmg - item->getDamageValue();
				if (std::get<BoolValue>(showDamage) && mdmg > 0) {
					std::wstring txt = std::to_wstring(dmg);

					if (std::get<BoolValue>(percentage)) txt = std::to_wstring(static_cast<int>(static_cast<float>(dmg) / static_cast<float>(mdmg) * 100.f)) + L'%';

					Vec2 textSize = dc.getTextSize(txt, Renderer::FontSelection::PrimaryRegular, textS);

					if (!isLeft) {
						extra = 6.f;
						extra += textSize.x;
					}
					else {
						extra = 6.f;
						extra += textSize.x;
					}
					if (extra > highest) highest = extra;
				}
			}
		}
	}


	Vec2 pos;
	if (mode.getSelectedKey() == mode_vetical) {
		pos.y = itemSize * numItems;
		if (isLeft) pos.x += highest;

		// render from bottom to top
		size_t add = 0;
		for (size_t i = 0; i < items.size(); i++) {
			auto item = items[i];
			if (item && item->item) {
				Vec2 renderPos = pos;
				renderPos.y -= itemSize;
				renderPos.y -= (itemSize * add);
				auto rc = drawItem(dc, renderPos, item, 1.f);

				auto it = *item->item;
				int mdmg = it->getMaxDamage();
				int dmg = mdmg - item->getDamageValue();

				if (std::get<BoolValue>(showDamage) && mdmg > 0) {
					std::wstring txt = std::to_wstring(dmg);
					if (std::get<BoolValue>(percentage)) txt = std::to_wstring(static_cast<int>(static_cast<float>(dmg) / static_cast<float>(mdmg) * 100.f)) + L'%';
					Vec2 textSize = dc.getTextSize(txt.c_str(), Renderer::FontSelection::PrimaryRegular, textS);
					auto y = rc.centerY(textSize.y);

					if (!isLeft) {
						extra = 6.f;
						dc.drawText({ rc.right + extra, rc.top, rect.getWidth(), rc.bottom}, txt, std::get<ColorValue>(txtColor).color1, Renderer::FontSelection::PrimaryRegular, textS, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
						extra += textSize.x;
					}
					else {
						extra = 6.f;
						dc.drawText(rc.translate(-rc.getWidth() - extra, 0.f), txt, std::get<ColorValue>(txtColor).color1, Renderer::FontSelection::PrimaryRegular, textS, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
						extra += textSize.x;
					}
				}

				add++;
			}
		}
		rect.right = rect.left + itemSize + (isLeft ? highest : highest);
		rect.bottom = rect.top + (itemSize * std::max(1.f, static_cast<float>(numItems)));
	}
	else {
		// render from side to side
		size_t add = 0;
		for (size_t i = 0; i < items.size(); i++) {
			auto item = items[(items.size() - 1) - i];
			if (item && item->item) {
				Vec2 renderPos = pos;
				renderPos.x += (itemSize * add);
				drawItem(dc, renderPos, item, 1.f);
				numItems++;
				add++;
			}
		}
		rect.right = rect.left + (itemSize * std::max(1.f, static_cast<float>(numItems)));
		rect.bottom = rect.top + itemSize;
	}

	if (itemSize == 0) {
		rect.right = rect.left + itemSize;
		rect.bottom = rect.top + itemSize;
	}
}

d2d::Rect ArmorHUD::drawItem(MCDrawUtil& dc, Vec2 pos, SDK::ItemStack* stack, float size){
	auto rc = dc.drawItem(stack, pos, size, 1.f);

	if (stack->itemCount > 1) {
		std::wstring st = std::to_wstring(stack->itemCount);
		float textSize = 25.f;
		dc.drawText(rc, st, std::get<ColorValue>(txtColor).color1, Renderer::FontSelection::PrimaryRegular,
			textSize, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	}
	return rc;
}
