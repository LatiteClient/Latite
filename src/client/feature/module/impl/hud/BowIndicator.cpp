#include "BowIndicator.h"
#include "sdk/common/world/actor/player/Player.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/player/LocalPlayer.h"

BowIndicator::BowIndicator() : TextModule("BowIndicator", "Bow Indicator", "Shows bow chargedness", HUD) {
	addSetting("visual", "Visual Indicator", "To show a bar instead of text", this->visual);
	addSetting("horizontal", "Horizontal", "Whether it's horizontal or not", this->horizontal, "visual"_istrue);
	addSliderSetting("size", "Size", "The size of the indicator", indicatorSize, FloatValue(0.f), FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
	addSliderSetting("width", "Width", "The width of the indicator", indicatorWidth, FloatValue(0.f), FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
}

BowIndicator::~BowIndicator() {
}

void BowIndicator::render(DXContext& dc, bool isDefault, bool inEditor) {
	if (!std::get<BoolValue>(visual)) {
		TextModule::render(dc, isDefault, inEditor);
		return;
	}
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;
	auto slot = plr->supplies->inventory->getItem(plr->supplies->selectedSlot);

	bool horiz = std::get<BoolValue>(horizontal);
	float wid = std::get<FloatValue>(indicatorWidth);
	float siz = std::get<FloatValue>(indicatorSize);

	d2d::Rect rc = { 0.f, 0.f, horiz ? siz : wid, horiz ? wid : siz };
	float rad = std::get<FloatValue>(indicatorRad) / 10.f * (std::min)(rc.getWidth(), rc.getHeight());

	if (slot->item) {
		auto item = *slot->item;
		if (item->id.hash == 0xd8d9a7186bad3c2f /*bow*/) {
			int useDur = plr->getItemUseDuration();
			auto mxu = item->getMaxUseDuration(slot);
			float diff = static_cast<float>(item->getMaxUseDuration(slot) - useDur);
			float percent = (std::min)((std::max)(diff / 20.f, 0.f), 1.f);

			d2d::Rect fillRc = rc;
			if (horiz) {
				fillRc.right = fillRc.left + fillRc.getWidth() * percent;
			}
			else {
				fillRc.bottom = fillRc.top + fillRc.getHeight() * percent;
			}
			dc.fillRoundedRectangle(fillRc, std::get<ColorValue>(indicatorCol2).color1, rad);
		}
	}
	else {

	}

	dc.fillRoundedRectangle(rc, std::get<ColorValue>(indicatorCol).color1, rad);
}

std::wstringstream BowIndicator::text() {
	std::wstringstream wss;
	wss << "hi";
	return wss;
}
