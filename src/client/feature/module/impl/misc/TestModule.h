#pragma once
#include "client/feature/module/Module.h"
#include "client/feature/module/HUDModule.h"
#include "client/event/impl/TickEvent.h"

class TestModule : public HUDModule {
private:
	EnumData testEnum;
public:
	TestModule();
	~TestModule() = default;

	void render(DXContext& ctx, bool isDefault, bool inEditor) override;

	void onTick(Event& ev);
	void onRender(Event& ev);
	void onKey(Event& ev);
	void onEnable() override {};
};