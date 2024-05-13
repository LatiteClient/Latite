#include "pch.h"
#include "ItemCounter.h"


namespace {
	class ItemCount {
	public:
		std::string texture;
		bool isHash = false;
		ValueType& setting;
		int64_t hash = 0;
		std::string inString = "";

		ItemCount(std::string texture, ValueType& setting, bool hashed, int64_t hash = 0, std::string include = "") : texture(texture), isHash(hashed), inString(include), setting(setting), hash(hash) {
		}
	};

	std::vector<ItemCount> counterList;

	int getCountStr(SDK::Inventory* inv, std::string incl) {
		int count = 0;
		for (int i = 0; i < 36; i++) {
			auto stack = inv->getItem(i);
			if (auto it = stack->item) {
				auto tName = (*it)->id.getString();
				if (tName.find(incl) != std::string::npos) count += stack->itemCount;
			}
		}
		return count;
	}

	int getCountHashed(SDK::Inventory* inv, int64_t hash) {
		int count = 0;
		for (int i = 0; i < 36; i++) {
			auto stack = inv->getItem(i);
			if (auto it = stack->item) {
				if ((*it)->id.hash == hash)  count += stack->itemCount;
			}
		}
		return count;
	}
}


ItemCounter::ItemCounter() : HUDModule("ItemCounter", "Item Counter", "Counts certain items in your inventory.", HUD) {
	std::string arrowT = "textures/items/arrow";
	std::string potionT = "textures/items/potion_bottle_splash_heal";
	std::string xpT = "textures/items/experience_bottle";
	std::string totemT = "textures/items/totem";
	std::string crystalT = "textures/items/end_crystal";

	addSetting("alwaysShow", "Always Show", "", alwaysShow);
	addSetting("potions", "Potions", "", potions);
	addSetting("arrows", "Arrows", "", arrow);
	addSetting("crystals", "Crystals", "", crystals);
	addSetting("totems", "Totems", "", totems);
	addSetting("bottles", "XP Bottles", "", xpBottles);

	counterList.emplace_back(potionT, potions, false, 0, "potion");
	counterList.emplace_back(arrowT, arrow, false, 0, "arrow");
	counterList.emplace_back(totemT, totems, false, 0, "totem");
	counterList.emplace_back(xpT, xpBottles, false, 0, "experience_bottle");
	counterList.emplace_back(crystalT, crystals, false, 0, "end_crystal");
}

void ItemCounter::render(DrawUtil& ct, bool isDefault, bool inEditor) {
	if (isDefault) return;

	auto& dc = reinterpret_cast<MCDrawUtil&>(ct);
	auto ctx = dc.renderCtx;

	this->rect.right = rect.left + 16.f;
	this->rect.bottom = rect.top + 16.f;


	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	auto supplies = lp->supplies;
	auto inv = supplies->inventory;

	Vec2 pos;

	int renderedCounterList = 0;

	for (auto& counters : counterList) {
		if (std::get<BoolValue>(counters.setting) == false) continue;
		int count = counters.isHash ? getCountHashed(inv, counters.hash) : getCountStr(inv, counters.inString);

		if (count || std::get<BoolValue>(alwaysShow)) {
			SDK::TexturePtr text{};
			ctx->getTexture(&text, SDK::ResourceLocation(counters.texture.c_str(), 0), false);

			d2d::Rect rc = { pos.x, pos.y, pos.x + 48.f, pos.y + 48.f };
			dc.drawImage(text, pos, { 48.f, 48.f }, d2d::Colors::WHITE);

			// here we go again
			std::wstring txt = std::to_wstring(count);
			Vec2 txtSize = dc.getTextSize(txt, Renderer::FontSelection::PrimaryRegular, 30.f);
			float y = rc.centerY(txtSize.y);

			d2d::Rect textRect = { rc.right + 2.f, rc.top, rc.right + 2.f + txtSize.x, rc.bottom };

			dc.drawText(textRect, txt.c_str(), d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular, 30.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			pos.y += 48.f;
			renderedCounterList++;
		}
	}

	this->rect.right = this->rect.left + 150.f;

	this->rect.bottom = this->rect.top + (renderedCounterList * 48.f);
}
