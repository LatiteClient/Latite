#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/render/Renderer.h"
#include "util/Logger.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

TestModule::TestModule() : Module("TestModule", "TestModule", "Module for testing new things.", GAME) {
	this->listen<TickEvent>(&TestModule::onTick);
	this->listen<RenderOverlayEvent>(&TestModule::onRender);
}

void TestModule::onTick(Event&) {
	//auto& even = reinterpret_cast<TickEvent&>(ev);
}

void TestModule::onRender(Event& ev) {
}

