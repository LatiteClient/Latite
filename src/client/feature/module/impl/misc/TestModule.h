#pragma once
#include "client/feature/module/Module.h"
#include "client/feature/module/HUDModule.h"
#include "client/event/impl/TickEvent.h"

class TestModule : public Module {
private:
	EnumData testEnum;
public:
	TestModule();
	~TestModule() = default;

	void onTick(Event& ev);
	void onRender(Event& ev);
	void onKey(Event& ev);
	void onEnable() override {};
};