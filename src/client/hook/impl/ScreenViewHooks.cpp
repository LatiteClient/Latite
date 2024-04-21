#include "pch.h"
#include "ScreenViewHooks.h"
#include "PacketHooks.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"
#include "sdk/common/client/renderer/Tessellator.h"
#include "sdk/common/client/renderer/MaterialPtr.h"
#include "../Hooks.h"
#include <client/script/PluginManager.h>
#include <imgui/imgui.h>
#include <sdk/common/client/renderer/MeshUtils.h>
#include <util/ImRendererScreenContext.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define __STDC_LIB_EXT1__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace {
	std::shared_ptr<Hook> setupAndRenderHook;
}

void __fastcall ScreenViewHooks::setupAndRender(SDK::ScreenView* view, void* ctx) {

		// render imgui
	ImRendererScreenContext::init();
	if (!ImRendererScreenContext::getDrawList()) {
		}

		RenderLayerEvent ev{ view, reinterpret_cast<SDK::MinecraftUIRenderContext*>(ctx) };
		Eventing::get().dispatch(ev);

		static bool initFile = false;

		/* {
			auto& io = ImGui::GetIO();

			io.DisplaySize.x = 1920;
			io.DisplaySize.y = 1008;

			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
		}*/
		/*
		ImGui::NewFrame();

		ImGui::GetForegroundDrawList()->AddCircleFilled({ 200.f, 200.f }, 75.f, 0xFFFF6D51, 10);

		ImGui::Begin("###");

		bool checkbox111 = true;

		ImGui::Text("        ");
		ImGui::Checkbox("##", &checkbox111);

		ImGui::End();

		//ImGui::GetForegroundDrawList()->AddText({ 10.f, 10.f }, 0xFFFFFFFF, "H");

		ImGui::Render();
		*/



	setupAndRenderHook->oFunc<decltype(&setupAndRender)>()(view, ctx);

	static bool hasInitPacketSender = false;
	if (!hasInitPacketSender) {
		if (SDK::ClientInstance::get()->getLocalPlayer()) {
			Latite::getHooks().get<PacketHooks>().initPacketSender(SDK::ClientInstance::get()->getLocalPlayer()->packetSender);
			hasInitPacketSender = true;
		}
	}
	RenderGameEvent evt{ };
	Eventing::get().dispatch(evt);

	//ImRendererScreenContext::getDrawList()->AddRectFilled({400, 400}, {1000, 1000.f}, 0xFF00FFFF, 300.f);
	ImRendererScreenContext::renderDrawList(ev.getUIRenderContext()->screenContext, ImRendererScreenContext::getDrawList());
}

ScreenViewHooks::ScreenViewHooks() : HookGroup("ScreenView") {
	setupAndRenderHook = addHook(Signatures::ScreenView_setupAndRender.result, setupAndRender,
		"ScreenView::setupAndRender");
}
