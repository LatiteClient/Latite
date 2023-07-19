#pragma once
#include "client/feature/module/Module.h"
#include "client/event/impl/TickEvent.h"

class TestModule : public Module {
public:
	TestModule();
	~TestModule() = default;

	void onTick(Event& ev);
	void onEnable() override {};
};