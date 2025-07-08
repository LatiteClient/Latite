#include "pch.h"
#include "TextModule.h"
#include "client/feature/setting/Setting.h"

void TextModule::onInit() {
	addSetting("fillBg", LocalizeString::get("client.textmodule.props.background.name"), L"", fillBg);
	addSetting("customSize", LocalizeString::get("client.textmodule.props.customSize.name"),
               LocalizeString::get("client.textmodule.props.customSize.desc"), customSize, "fillBg"_istrue);
	addSliderSetting("bgX", LocalizeString::get("client.textmodule.props.bgX.name"),
                     LocalizeString::get("client.textmodule.props.bgX.desc"), bgX, FloatValue(0.f), FloatValue(maxBGX),
                     FloatValue(2.5f), "customSize"_istrue);
	addSliderSetting("bgY", LocalizeString::get("client.textmodule.props.bgY.name"),
                     LocalizeString::get("client.textmodule.props.bgY.desc"), bgY, FloatValue(0.f), FloatValue(300.f),
                     FloatValue(2.5f), "customSize"_istrue);

    addSliderSetting("padX", LocalizeString::get("client.textmodule.props.padX.name"),
                     LocalizeString::get("client.textmodule.props.padX.desc"), padX, FloatValue(0.f), FloatValue(40.f),
                     FloatValue(2.f), "customSize"_isfalse);
    addSliderSetting("padY", LocalizeString::get("client.textmodule.props.padY.name"),
                     LocalizeString::get("client.textmodule.props.padY.desc"), padY, FloatValue(0.f), FloatValue(40.f),
                     FloatValue(2.f), "customSize"_isfalse);

    addSetting("prefix", LocalizeString::get("client.textmodule.props.prefix.name"),
               LocalizeString::get("client.textmodule.props.prefix.desc"), prefix);
    addSetting("suffix", LocalizeString::get("client.textmodule.props.suffix.name"),
               LocalizeString::get("client.textmodule.props.suffix.desc"), suffix);
    addSliderSetting("textSize", LocalizeString::get("client.textmodule.props.textSize.name"), L"", textSizeS,
                     FloatValue(2.f), FloatValue(100.f), FloatValue(2.f));

    addSetting("textCol", LocalizeString::get("client.textmodule.props.textColor.name"), L"", textColor);
    addSetting("bgColor", LocalizeString::get("client.textmodule.props.bgColor.name"), L"", bgColor);

	alignment.addEntry({ alignment_center, LocalizeString::get("client.textmodule.props.alignmentCenter.name")});
	alignment.addEntry({ alignment_left, LocalizeString::get("client.textmodule.props.alignmentLeft.name") });
	alignment.addEntry({ alignment_right, LocalizeString::get("client.textmodule.props.alignmentRight.name") });
    addEnumSetting("alignmnet", LocalizeString::get("client.textmodule.props.alignment.name"),
                   LocalizeString::get("client.textmodule.props.alignment.desc"), this->alignment);
    addSetting("showOutline", LocalizeString::get("client.textmodule.props.showOutline.name"), L"", showOutline);
    addSliderSetting("outlineThickness", LocalizeString::get("client.textmodule.props.outlineThickness.name"), L"",
                     outlineThickness, FloatValue(0.f), FloatValue(20.f), FloatValue(1.f), "showOutline"_istrue);
    addSetting("outlineCol", LocalizeString::get("client.textmodule.props.outlineColor.name"), L"", outlineColor,
               "showOutline"_istrue);
    addSliderSetting("radius", LocalizeString::get("client.textmodule.props.radius.name"), L"", radius, FloatValue(0.f),
                     FloatValue(10.f), FloatValue(1.f));
}

void TextModule::render(DrawUtil& dc, bool isDefault, bool inEditor) {
	//dc.setTextShadow(textShadow);
	int textPadding = std::get<BoolValue>(fillBg) ? static_cast<int>(std::get<FloatValue>(padX)) : 0;
	int textPaddingY = std::get<BoolValue>(fillBg) ? static_cast<int>(std::get<FloatValue>(padY) * 2.f) : 0;

	float textSize = std::get<FloatValue>(textSizeS);//this->getTextSize();


	std::wstringstream text = this->text(isDefault, inEditor);
	text = processText(text);

	std::wstring str = text.str();
	lastText = str;

	auto sCol = std::get<ColorValue>(bgColor).getMainColor();
	d2d::Color realCol = sCol;
	auto sTCol = std::get<ColorValue>(textColor).getMainColor();
	d2d::Color realTCol = sTCol;
	auto sOCol = std::get<ColorValue>(outlineColor).getMainColor();
	d2d::Color realOCol = sOCol;

	float rad = 0.f;

	//if (textScaled && customSize) textSize = dc.scaleTextInBounds(str.c_str(), 100.f, rect.getWidth(), 4.f, rect.getHeight());

	DWRITE_TEXT_ALIGNMENT align = alignment.getSelectedKey() == alignment_center ? DWRITE_TEXT_ALIGNMENT_CENTER :
		alignment.getSelectedKey() == alignment_left ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_TRAILING;
	if (std::get<BoolValue>(customSize)) {
		d2d::Rect rc = d2d::Rect(0, 0, std::get<FloatValue>(bgX), std::get<FloatValue>(bgY));
		if (!text.str().empty()) {
			Vec2 ts = dc.getTextSize(str.c_str(), Renderer::FontSelection::SecondaryLight, textSize, false);
			Vec2 drawPos = rc.center(ts);

			rad = (std::get<FloatValue>(radius).value / 10.f) * (rc.getHeight() / 2.f);

			if (std::get<BoolValue>(showOutline)) dc.drawRoundedRectangle(rc, realOCol, rad, std::get<FloatValue>(outlineThickness));
			if (std::get<BoolValue>(fillBg)) dc.fillRoundedRectangle(rc, realCol, rad);
			dc.drawText(rc, str.c_str(), realTCol, Renderer::FontSelection::SecondaryLight, textSize, align, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, !isDefault && !inEditor && cacheText);
		}
		this->rect.right = rect.left + std::get<FloatValue>(bgX);
		this->rect.bottom = rect.top + std::get<FloatValue>(bgY);

	}
	else {
		Vec2 drawPos = { static_cast<float>(textPadding), static_cast<float>(textPaddingY) };
		Vec2 ts = dc.getTextSize(str.c_str(), Renderer::FontSelection::SecondaryLight, textSize, false);
		d2d::Rect rc = d2d::Rect(0, 0, ts.x + (textPadding * 2), ts.y + (textPaddingY * 2));

		if (!text.str().empty()) {
			rad = (std::get<FloatValue>(radius).value / 10.f) * (rc.getHeight() / 2.f);
			if (std::get<BoolValue>(showOutline)) dc.drawRoundedRectangle(rc, realOCol, rad, std::get<FloatValue>(outlineThickness));
			if (std::get<BoolValue>(fillBg)) dc.fillRoundedRectangle(rc, std::get<ColorValue>(bgColor).getMainColor(), rad);
			dc.drawText(rc, str.c_str(), realTCol, Renderer::FontSelection::SecondaryLight, textSize, align, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, !isDefault && !inEditor && cacheText);
		}

		this->rect.right = rect.left + rc.right;
		this->rect.bottom = rect.top + rc.bottom;
	}

	//d2d::Rect rc = { 0.f, 0.f, rect.getWidth(), rect.getHeight() };

	//dc.fillRoundedRectangle(rc, Latite::get().getHUDBlurBrush(), rad);
	dc.flush();
}

std::wstringstream TextModule::processText(std::wstringstream& stream) {
	std::wstringstream str;
	str << std::get<TextValue>(prefix).str;
	str << stream.str();
	str << std::get<TextValue>(suffix).str;
	return str;
}
