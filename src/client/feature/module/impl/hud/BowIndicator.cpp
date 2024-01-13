#include "pch.h"
#include "BowIndicator.h"

BowIndicator::BowIndicator() : TextModule("BowIndicator", "Bow Indicator", "Shows bow chargedness", HUD, 400.f, 0, true) {
	addSetting("visual", "Visual Indicator", "To show a bar instead of text", this->visual);
	addSetting("bgCol", "Color", "The indicator foreground color", this->indicatorCol2);
	addSetting("bgCol", "Background Color", "The indicator background color", this->indicatorCol);
	addSetting("horizontal", "Horizontal", "Whether it's horizontal or not", this->horizontal, "visual"_istrue);

	addSliderSetting("size", "Size", "The size of the indicator", indicatorSize, FloatValue(0.f), FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
	addSliderSetting("width", "Width", "The width of the indicator", indicatorWidth, FloatValue(0.f), FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
	addSliderSetting("rad", "Radius", "The radius of the indicator", indicatorRad, FloatValue(0.f), FloatValue(5.f), FloatValue(2.5f), "visual"_istrue);
}

BowIndicator::~BowIndicator() {
}

void BowIndicator::render(DrawUtil& dc, bool isDefault, bool inEditor) {
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

	rect.right = rect.left + rc.getWidth();
	rect.bottom = rect.top + rc.getHeight();

	if (auto percent = getBowCharge(slot)) {
		dc.fillRoundedRectangle(rc, std::get<ColorValue>(indicatorCol).color1, rad);

		d2d::Rect fillRc = rc;
		fillRc.left += 6;
		fillRc.top += 6;
		fillRc.right -= 6;
		fillRc.bottom -= 6;

		if (horiz) {
			fillRc.right = fillRc.left + fillRc.getWidth() * percent.value();
		}
		else {
			fillRc.top = fillRc.bottom - fillRc.getHeight() * percent.value();
		}
		dc.fillRoundedRectangle(fillRc, std::get<ColorValue>(indicatorCol2).color1, rad);
	}

}

std::wstringstream BowIndicator::text(bool, bool) {
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	auto slot = plr->supplies->inventory->getItem(plr->supplies->selectedSlot);
	auto charge = getBowCharge(slot);

	std::wstringstream wss;
	if (!plr) return wss;
	wss << std::round(charge.value_or(0.f) * 100.f) << "%";
	return wss;
}

std::optional<float> BowIndicator::getBowCharge(SDK::ItemStack* slot) {
	if (!slot->item) return std::nullopt;
	auto item = *slot->item;

	if (item->id.hash == "bow"_fnv64 /*bow*/ || item->id.hash == "crossbow"_fnv64 /*crossbow*/ || item->id.hash == "trident"_fnv64) {
		int useDur = SDK::ClientInstance::get()->getLocalPlayer()->getItemUseDuration();
		if (useDur) {
			auto mxu = item->getMaxUseDuration(slot);
			float diff = static_cast<float>(item->getMaxUseDuration(slot) - useDur);
			return (std::min)((std::max)(diff / 20.f, 0.f), 1.f);
		}
	}
	return std::nullopt;
}
