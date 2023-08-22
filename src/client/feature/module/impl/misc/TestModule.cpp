#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "client/render/Renderer.h"
#include "util/Logger.h"
#include "util/Util.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/misc/ClientMessageSink.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

bool wState = false;
bool lastWState = false;

TestModule::TestModule() : HUDModule("TestModule", "TestModule", "Module for testing new things.", GAMEHUD) {
	
	testEnum.addEntry({ 0, "Test", "Test" });
	testEnum.addEntry({ 1, "Gaming", "Test" });
	testEnum.addEntry({ 2, "Hi", "Test" });
	addEnumSetting("test", "Test", "Gaming", this->testEnum);
	
	this->listen<TickEvent>(&TestModule::onTick);
	this->listen<RenderGameEvent>(&TestModule::onRender);
	this->listen<KeyUpdateEvent>(&TestModule::onKey);
}

static float delayPx = 0.f;
void TestModule::render(DXContext& ctx, bool isDefault, bool inEditor) {
	rect.right = rect.left + 300.f;
	rect.bottom = rect.top + 100.f;
	ctx.fillRoundedRectangle({ 0.f, 0.f, 300.f, 100.f }, d2d::Colors::BLACK.asAlpha(0.5f), 20.f);
	ctx.drawText({ 0.f, 0.f, rect.getWidth(), rect.getHeight() }, util::StrToWStr(std::format("Input Delay: {:.2f} MS", delayPx)), d2d::Colors::WHITE, Renderer::FontSelection::Light, 20.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void TestModule::onTick(Event& evGeneric) {
	auto& even = reinterpret_cast<TickEvent&>(evGeneric);
}
bool realW = false;
bool lastW = false;
std::chrono::system_clock::time_point tp;

void TestModule::onRender(Event& ev) {
	auto inst = SDK::ClientInstance::get();
	realW = (GetKeyState('W') & 0x8000) > 0;
	if (realW && !lastW) {
		tp = std::chrono::system_clock::now();
	}
	lastW = realW;
}

void TestModule::onKey(Event& ev)
{
	auto& kev = reinterpret_cast<KeyUpdateEvent&>(ev);
	if (kev.getKey() == 'W') {
		wState = kev.isDown();
		if (!lastWState && wState) {
			auto now = std::chrono::system_clock::now();
			delayPx = (float)std::chrono::duration_cast<std::chrono::milliseconds>(now - tp).count();
		}
		
		lastWState = wState;
	}
}

