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
#include "client/event/impl/TickEvent.h"
#include <sdk/common/client/util/JpegCommentWriter.h>
#include <sdk/common/client/renderer/MaterialPtr.h>
#include <sdk/deps/CoreGraphics/ImageBuffer.h>

#define FUNC_HELPER(x)  void(Feature::*)(x&)

bool wState = false;
bool lastWState = false;

TestModule::TestModule() : Module("TestModule", "TestModule", "Module for testing new things.", GAMEHUD) {
	
	testEnum.addEntry({ 0, "Test", "Test" });
	testEnum.addEntry({ 1, "Gaming", "Test" });
	testEnum.addEntry({ 2, "Hi", "Test" });
	addEnumSetting("test", "Test", "Gaming", this->testEnum);
	
	this->listen<TickEvent>(&TestModule::onTick);
	this->listen<RenderLayerEvent>(&TestModule::onRender);
	this->listen<KeyUpdateEvent>(&TestModule::onKey);
}

void TestModule::onTick(Event& evGeneric) {
}

void TestModule::onRender(Event& evG) {
	
	
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	
	using namespace SDK;
	auto scn = ev.getUIRenderContext()->screenContext;

	auto jpegCommentWriter = std::make_unique<JpegCommentWriter>(MaterialPtr::getUITextureAndColor());

	cg::ImageBuffer img = cg::ImageBuffer(200, 200);
	jpegCommentWriter->_drawImage(scn, &img);
}

void TestModule::onKey(Event& ev)
{
	[[maybe_unused]] auto& kev = reinterpret_cast<KeyUpdateEvent&>(ev);
	if (kev.getKey() == 'K') {
		auto lp = SDK::ClientInstance::get()->getLocalPlayer();
		if (lp) {
			std::string str;
			auto& map = lp->molangVariableMap;
			for (auto& var : map.mVariables) {
				str += "\"" + var->mName.getString() + "\" |\n";
			}
			util::SetClipboardText(util::StrToWStr(str));
		}
	}
}

