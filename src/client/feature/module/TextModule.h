#pragma once
#include "HUDModule.h"

class TextModule : public HUDModule {
protected:
	Setting::Value customSize = BoolValue(false);

	Setting::Value fillBg = BoolValue(true);

	Setting::Value bgX = FloatValue(150.f);
	Setting::Value bgY = FloatValue(45.f);

	Setting::Value padX = FloatValue(18.f);
	Setting::Value padY = FloatValue(12.f);

	Setting::Value textColor = ColorValue(1.f, 1.f, 1.f, 1.f);
	Setting::Value bgColor = ColorValue(0.f, 0.f, 0.f, 0.5f);
	
	Setting::Value brackets = BoolValue(false);
	Setting::Value showOutline = BoolValue(false);

	Setting::Value outlineThickness = FloatValue(2.f);
	Setting::Value outlineColor = ColorValue(1.f, 1.f ,1.f, 1.f);

	Setting::Value radius = FloatValue(5.f);
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