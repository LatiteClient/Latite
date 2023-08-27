#include "TextModule.h"
#include "api/feature/setting/Setting.h"
#include "util/Util.h"
#include <client/Latite.h>

void TextModule::onInit() {
	addSetting("fillBg", "Background", "", fillBg);
	addSetting("customSize", "Fixed Size", "Use a custom size instead of padding", customSize, "fillBg"_istrue);
	addSliderSetting("bgX", "BG X", "Background size (only if Fixed Size is enabled)", bgX, FloatValue(0.f), FloatValue(maxBGX), FloatValue(2.5f), "customSize"_istrue);
	addSliderSetting("bgY", "BG Y", "Background size (only if Fixed Size is enabled)", bgY, FloatValue(0.f), FloatValue(300.f), FloatValue(2.5f), "customSize"_istrue);

	addSliderSetting("padX", "Pad X", "Padding", padX, FloatValue(0.f), FloatValue(40.f), FloatValue(2.f), "customSize"_isfalse);
	addSliderSetting("padY", "Pad Y", "Padding", padY, FloatValue(0.f), FloatValue(40.f), FloatValue(2.f), "customSize"_isfalse);

	addSetting("prefix", "Prefix", "The text before the value", prefix);
	addSetting("suffix", "Suffix", "The text after the value", suffix);
	addSliderSetting("textSize", "Text Size", "", textSizeS, FloatValue(2.f), FloatValue(100.f), FloatValue(2.f));

	addSetting("textCol", "Text", "", textColor);
	addSetting("bgColor", "Background", "", bgColor);

	alignment.addEntry({ alignment_center, "Center" });
	alignment.addEntry({ alignment_left, "Left" });
	alignment.addEntry({ alignment_right, "Right" });
	addEnumSetting("alignmnet", "Alignment", "How the text is aligned/expands", this->alignment);
	addSetting("showOutline", "Outline", "", showOutline);
	addSliderSetting("outlineThickness", "Thickness", "", outlineThickness, FloatValue(0.f), FloatValue(20.f), FloatValue(1.f), "showOutline"_istrue);
	addSetting("outlineCol", "Outline Color", "", outlineColor, "showOutline"_istrue);
	addSliderSetting("radius", "Radius", "", radius, FloatValue(0.f), FloatValue(10.f), FloatValue(1.f));
}

void TextModule::render(DXContext& ctx, bool isDefault, bool inEditor) {
	DXContext dc;
	//dc.setTextShadow(textShadow);
	int textPadding = static_cast<int>(std::get<FloatValue>(padX));
	int textPaddingY = static_cast<int>(std::get<FloatValue>(padY) * 2.f);

	float textSize = std::get<FloatValue>(textSizeS);//this->getTextSize();


	std::wstringstream text = this->text(isDefault, inEditor);
	text = processText(text);


	std::wstring str = text.str();
	auto sCol = std::get<ColorValue>(bgColor).color1;
	d2d::Color realCol = sCol;
	auto sTCol = std::get<ColorValue>(textColor).color1;
	d2d::Color realTCol = sTCol;
	auto sOCol = std::get<ColorValue>(outlineColor).color1;
	d2d::Color realOCol = sOCol;

	float rad = 0.f;

	//if (textScaled && customSize) textSize = dc.scaleTextInBounds(str.c_str(), 100.f, rect.getWidth(), 4.f, rect.getHeight());
	if (std::get<BoolValue>(customSize)) {
		d2d::Rect rc = d2d::Rect(0, 0, std::get<FloatValue>(bgX), std::get<FloatValue>(bgY));
		Vec2 ts = dc.getTextSize(str.c_str(), Renderer::FontSelection::Light2, textSize, false);
		Vec2 drawPos = rc.center(ts);

		rad = (std::get<FloatValue>(radius).value / 10.f) * (rc.getHeight() / 2.f);

		if (std::get<BoolValue>(fillBg)) dc.fillRoundedRectangle(rc, realCol, rad);
		if (std::get<BoolValue>(showOutline)) dc.drawRoundedRectangle(rc, realOCol, rad, std::get<FloatValue>(outlineThickness));
		dc.drawText(rc, str.c_str(), realTCol, Renderer::FontSelection::Light2, textSize, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		this->rect.right = rect.left + std::get<FloatValue>(bgX);
		this->rect.bottom = rect.top + std::get<FloatValue>(bgY);

	}
	else {
		Vec2 drawPos = { static_cast<float>(textPadding), static_cast<float>(textPaddingY) };
		Vec2 ts = dc.getTextSize(str.c_str(), Renderer::FontSelection::Light2, textSize, false);
		d2d::Rect rc = d2d::Rect(0, 0, ts.x + (textPadding * 2), ts.y + (textPaddingY * 2));

		rad = (std::get<FloatValue>(radius).value / 10.f) * (rc.getHeight() / 2.f);
		if (std::get<BoolValue>(fillBg)) dc.fillRoundedRectangle(rc, std::get<ColorValue>(bgColor).color1, rad);
		if (std::get<BoolValue>(showOutline)) dc.drawRoundedRectangle(rc, realOCol, rad, std::get<FloatValue>(outlineThickness));
		dc.drawText(rc, str.c_str(), realTCol, Renderer::FontSelection::Light2, textSize, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		this->rect.right = rect.left + rc.right;
		this->rect.bottom = rect.top + rc.bottom;
	}

	//d2d::Rect rc = { 0.f, 0.f, rect.getWidth(), rect.getHeight() };

	//dc.fillRoundedRectangle(rc, Latite::get().getHUDBlurBrush(), rad);
}

std::wstringstream TextModule::processText(std::wstringstream& stream) {
	std::wstringstream str;
	str << util::StrToWStr(std::get<TextValue>(prefix).str);
	str << stream.str();
	str << util::StrToWStr(std::get<TextValue>(suffix).str);
	return str;
}
