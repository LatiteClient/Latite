#include "pch.h"
#include "TestModule.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "client/render/Renderer.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/misc/ClientMessageQueue.h"
#include "sdk/common/network/packet/TextPacket.h"
#include "sdk/String.h"
#include "client/event/impl/TickEvent.h"
#include <sdk/common/client/util/JpegCommentWriter.h>
#include <sdk/common/client/renderer/MaterialPtr.h>
#include <sdk/deps/CoreGraphics/ImageBuffer.h>
#include <sdk/common/client/gui/controls/VisualTree.h>
#include <sdk/common/client/gui/controls/UIControl.h>
#include <sdk/common/client/gui/screens/ContainerScreenController.h>
#include "client/event/impl/RendererInitEvent.h"

#define FUNC_HELPER(x)  void(Feature::*)(x&)

bool wState = false;
bool lastWState = false;

TestModule::TestModule() : HUDModule("TestModule", L"TestModule", L"Module for testing new things.", GAMEHUD) {
	
	testEnum.addEntry({ 0, L"Test", L"Test" });
	testEnum.addEntry({ 1, L"Gaming", L"Test" });
	testEnum.addEntry({ 2, L"Hi", L"Test" });
	addEnumSetting("test", L"Test", L"Gaming", this->testEnum);
	addSetting("colorSetting", L"Color Setting", L"", colorSetting);
	
	this->listen<TickEvent>(&TestModule::onTick);
	this->listen<RenderLayerEvent>(&TestModule::onRender, true);
	this->listen<KeyUpdateEvent>(&TestModule::onKey);
	this->listen<RenderLayerEvent>(&TestModule::onRenderLayer);

	this->listen<RendererInitEvent>(&TestModule::onRendererInit, true);
	this->listen<RendererCleanupEvent>(&TestModule::onRendererCleanup, true);
}

void TestModule::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
	if (ctx.isMinecraft()) return;

	{
		auto dc = Latite::getRenderer().getDeviceContext();

		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;

		auto& col = std::get<ColorValue>(colorSetting);

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop{};
		auto ss = Latite::getRenderer().getScreenSize();

		shift += SDK::ClientInstance::get()->minecraft->timer->alpha * 0.005f;

		static int color = 0;

		color++;
		color %= col.numColors;

		float sw = rect.getWidth() * shift;
		float sh = rect.getHeight() * shift;

		prop.startPoint = { 0,0 };
		prop.endPoint = { rect.getWidth(),0};

		if (col.numColors == 2) {
			d2d::Color colors[2] = { d2d::Color(col.color1), d2d::Color(col.color2) };
			const D2D1_GRADIENT_STOP stops[50] = {
				0.f, colors[color].get(),
				1.f, colors[(color + 1) % col.numColors].get(),
			};
			dc->CreateGradientStopCollection(stops, _countof(stops), gradientStopCollection.GetAddressOf());

		}
		else if (col.numColors == 3) {
			const D2D1_GRADIENT_STOP stops[] = {
				0.f, d2d::Color(col.color1).get(),
				0.5f, d2d::Color(col.color2).get(),
				1.f, d2d::Color(col.color3).get()
			};
			dc->CreateGradientStopCollection(stops, _countof(stops), gradientStopCollection.GetAddressOf());

		}
		else return;

		dc->CreateLinearGradientBrush(prop, gradientStopCollection.Get(), brush.ReleaseAndGetAddressOf());
	}



	D2DUtil& dc = (D2DUtil&)ctx;
	dc.fillRectangle({ 0.f, 0.f, rect.getWidth(), rect.getHeight()}, brush.Get());

	rect.right = rect.left + 200;
	rect.bottom = rect.top + 100;
}

void TestModule::onTick(Event& evGeneric) {
}

void TestModule::onRender(Event& evG) {
	return;

	static bool enable = isEnabled();
	static Vec2 storedVector = {};

	if (!isEnabled()) {
		enable = false;
		storedVector = SDK::ClientInstance::get()->getLocalPlayer()->getRot();
		return;
	}

	SDK::ClientInstance::get()->getLocalPlayer()->getRot() = storedVector;
	return;
	
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	
	using namespace SDK;
	auto scn = ev.getUIRenderContext()->screenContext;

	auto jpegCommentWriter = std::make_unique<JpegCommentWriter>(scn->tess, MaterialPtr::getUITextureAndColor());

	cg::ImageBuffer img = cg::ImageBuffer(200, 200);
	jpegCommentWriter->_drawImage(scn, &img);
}

void TestModule::onKey(Event& ev)
{
	return;
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

void TestModule::onRenderLayer(Event& evG) {
	return;
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	
	if (ev.getScreenView()->visualTree->rootControl->name == "inventory_screen") {
		auto info = ((SDK::ContainerScreenController*)ev.getScreenView()->screenController)->_getSelectedSlotInfo();
		int test = 32;
	}
}

void TestModule::onRendererInit(Event& ev) {
	
}

void TestModule::onRendererCleanup(Event& ev) {
	brush = nullptr;
}

