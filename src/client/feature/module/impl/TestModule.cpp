#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "util/Logger.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

TestModule::TestModule() : Module("TestModule", "Module for testing new things.", "TestModule")
{
	this->listen<TickEvent>(&TestModule::onTick);
}

void TestModule::onTick(Event& ev)
{
	auto even = reinterpret_cast<TickEvent&>(ev);
	auto lvl = even.getLevel();
	Logger::info("TestModule onTick");
}

