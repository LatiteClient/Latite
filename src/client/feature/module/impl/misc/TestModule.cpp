#include "pch.h"
#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "client/render/Renderer.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/misc/ClientMessageSink.h"
#include "sdk/common/network/packet/TextPacket.h"
#include "sdk/String.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

bool wState = false;
bool lastWState = false;

TestModule::TestModule() : Module("TestModule", "TestModule", "Module for testing new things.", GAMEHUD) {
	
	testEnum.addEntry({ 0, "Test", "Test" });
	testEnum.addEntry({ 1, "Gaming", "Test" });
	testEnum.addEntry({ 2, "Hi", "Test" });
	addEnumSetting("test", "Test", "Gaming", this->testEnum);
	
	this->listen<TickEvent>(&TestModule::onTick);
	this->listen<RenderGameEvent>(&TestModule::onRender);
	this->listen<KeyUpdateEvent>(&TestModule::onKey);
}

void TestModule::onTick(Event& evGeneric) {
	auto inst = SDK::ClientInstance::get();
	if (inst->getLocalPlayer()) {
		Logger::Info("{}", inst->getLocalPlayer()->getCommandPermissionLevel());
		SDK::TextPacket pkt{};
		String s{};
		s.setString("hello");
		pkt.chat(s);
		inst->getLocalPlayer()->packetSender->sendToServer(pkt);
	}
}

void TestModule::onRender(Event& ev) {
	
}

void TestModule::onKey(Event& ev)
{
	[[maybe_unused]] auto& kev = reinterpret_cast<KeyUpdateEvent&>(ev);
}

