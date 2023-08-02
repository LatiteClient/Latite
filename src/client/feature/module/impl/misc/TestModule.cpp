#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/render/Renderer.h"
#include "util/Logger.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

TestModule::TestModule() : HUDModule("TestModule", "TestModule", "Module for testing new things.", GAMEHUD) {
	this->listen<TickEvent>(&TestModule::onTick);
	this->listen<RenderOverlayEvent>(&TestModule::onRender);
}

void TestModule::render(DXContext& ctx, bool isDefault, bool inEditor) {
	rect.right = rect.left + 300.f;
	rect.bottom = rect.top + 100.f;
	ctx.fillRoundedRectangle({ 0.f, 0.f, 300.f, 100.f }, d2d::Colors::BLACK.asAlpha(0.5f), 20.f);
}

void TestModule::onTick(Event& evGeneric) {
	auto& even = reinterpret_cast<TickEvent&>(evGeneric);
}

void TestModule::onRender(Event& ev) {
}

