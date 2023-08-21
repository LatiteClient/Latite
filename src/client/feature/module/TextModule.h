#pragma once
#include "HUDModule.h"

class TextModule : public HUDModule {
protected:
	ValueType customSize = BoolValue(false);

	ValueType fillBg = BoolValue(true);

	ValueType bgX = FloatValue(150.f);
	ValueType bgY = FloatValue(45.f);

	ValueType padX = FloatValue(22.f);
	ValueType padY = FloatValue(4.f);
	
	ValueType textSizeS = FloatValue(30.f);

	ValueType textColor = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType bgColor = ColorValue(0.f, 0.f, 0.f, 0.5f);
	
	ValueType prefix = TextValue("");
	ValueType suffix = TextValue("");

	ValueType showOutline = BoolValue(false);

	ValueType outlineThickness = FloatValue(2.f);
	ValueType outlineColor = ColorValue(1.f, 1.f ,1.f, 1.f);

	ValueType radius = FloatValue(0.f);
public:
	TextModule(std::string const& name, std::string const& displayName,
		std::string const& description, Category category, float maxBgX = 400.f, int keybind = 0)
	: HUDModule(name, displayName, description, category, keybind, true),
	maxBGX(maxBgX) {
		this->textual = true;
	}

	void onInit() override;

	void render(DXContext& ctx, bool isDefault, bool inEditor) override;

protected:
	virtual std::wstringstream text(bool isDefault, bool inEditor) = 0;
	
	FloatValue maxBGX = 0.f;
private:
	std::wstringstream processText(std::wstringstream& stream);
};