#pragma once
#include "client/feature/module/Module.h"

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
	void onRenderLayer(Event& ev);
};