#pragma once
#include "client/feature/module/Module.h"

class TestModule : public HUDModule {
private:
	EnumData testEnum;
	ComPtr<ID2D1LinearGradientBrush> brush;
	float shift = 0.f;
	ValueType colorSetting = ColorValue();
public:
	TestModule();
	~TestModule() = default;
	void render(DrawUtil& ctx, bool isDefault, bool inEditor);

	void onTick(Event& ev);
	void onRender(Event& ev);
	void onKey(Event& ev);
	void onEnable() override {};
	void onRenderLayer(Event& ev);

	void onRendererInit(Event& ev);
	void onRendererCleanup(Event& ev);
};