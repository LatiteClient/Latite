#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "util/Logger.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

TestModule::TestModule() : Module("TestModule", "TestModule", "Module for testing new things.") {
	this->listen<TickEvent>(&TestModule::onTick);
}

void TestModule::onTick(Event& ev) {
	auto even = reinterpret_cast<TickEvent&>(ev);
	Logger::info("TestModule onTick");
}

