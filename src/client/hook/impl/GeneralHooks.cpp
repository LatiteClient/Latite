#include "GeneralHooks.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/network/RakNetConnector.h"
#include "sdk/signature/storage.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"
#include "client/feature/command/commandmanager.h"
#include "client/event/Eventing.h"
#include "client/event/impl/TickEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/ChatEvent.h"
#include "client/event/impl/ClickEvent.h"
#include "client/event/impl/AveragePingEvent.h"
#include "client/event/impl/CinematicCameraEvent.h"
#include "client/event/impl/BeforeMoveEvent.h"
#include "client/event/impl/AfterMoveEvent.h"
#include "client/script/ScriptManager.h"

#include "api/memory/memory.h"
#include "util/Logger.h"
#include "util/Util.h"

namespace {
	std::shared_ptr<Hook> Level_tickHook;
	std::shared_ptr<Hook> ChatScreenController_sendChatMesageHook;
	std::shared_ptr<Hook> GameRenderer_renderCurrentFrameHook;
	std::shared_ptr<Hook> Keyboard_feedHook;
	std::shared_ptr<Hook> OnClickHook;
	std::shared_ptr<Hook> LoadLibraryHook;
	std::shared_ptr<Hook> ConnectorTickHook;
	std::shared_ptr<Hook> AveragePingHook;
	std::shared_ptr<Hook> TurnDeltaHook;
	std::shared_ptr<Hook> MoveInputHandler_tickHook;
	std::shared_ptr<Hook> MovePlayerHook;
}

void GenericHooks::Level_tick(SDK::Level* level) {
	if (level == SDK::ClientInstance::get()->minecraft->getLevel()) {
		// Clientside level
		// dispatch clientside tick event..
		TickEvent ev(level);
		Latite::getEventing().dispatch(ev);
		Latite::getClientMessageSink().doPrint(100);
	}
	
	ScriptManager::Event sEv{L"world-tick", {}, false};
	Latite::getScriptManager().dispatchEvent(sEv);

	return Level_tickHook->oFunc<decltype(&Level_tick)>()(level);
}

void* GenericHooks::ChatScreenController_sendChatMessage(void* controller, std::string& message) {
	if (message.starts_with(Latite::getCommandManager().prefix)) {
		Latite::getCommandManager().runCommand(message);
		return 0;
	}

	ChatEvent ev{ message };
	if (Eventing::get().dispatch(ev)) return nullptr;

	{
		ScriptManager::Event::Value val{L"message"};
		val.val = util::StrToWStr(message);
		ScriptManager::Event sev{L"send-chat", { val }, true};
		if (Latite::getScriptManager().dispatchEvent(sev)) return nullptr;
	}

	return ChatScreenController_sendChatMesageHook->oFunc<decltype(&ChatScreenController_sendChatMessage)>()(controller, message);
}

void* GenericHooks::GameRenderer_renderCurrentFrame(void* rend) {
	// this causes the jitter bug
	//{
	//	RenderGameEvent ev{};
	//	Eventing::get().dispatchEvent(ev);
	//}

	return GameRenderer_renderCurrentFrameHook->oFunc<decltype(&GameRenderer_renderCurrentFrame)>()(rend);
}

void GenericHooks::Keyboard_feed(int key, bool isDown) {
	KeyUpdateEvent ev{key, isDown};
	if (Eventing::get().dispatch(ev)) return;

	{
		ScriptManager::Event::Value val{L"isDown"};
		val.val = isDown;

		ScriptManager::Event::Value val3{L"keyCode"};
		val3.val = static_cast<double>(key);

		ScriptManager::Event::Value val2{L"keyAsChar"};

		std::string str = "";
		if (key > 31 && key < 128) {
			str = (char)key;
		}
		val2.val = util::StrToWStr(str);

		ScriptManager::Event sEv{L"key-press", { val, val2, val3 }, true};
		if (Latite::getScriptManager().dispatchEvent(sEv)) return;
	}

	return Keyboard_feedHook->oFunc<decltype(&Keyboard_feed)>()(key, isDown);
}

void GenericHooks::onClick(ClickMap* map, char clickType, char isDownWheelDelta, uintptr_t a4, int16_t a5, int16_t a6, int16_t a7, char a8) {
	ClickEvent ev{ clickType, isDownWheelDelta };
	if (Eventing::get().dispatch(ev)) return;

	if (clickType > 0) {
		Vec2& mousePos = SDK::ClientInstance::get()->cursorPos;

		ScriptManager::Event::Value val{L"mouseX"};
		val.val = static_cast<double>(mousePos.x);

		ScriptManager::Event::Value val2{L"mouseY"};
		val2.val = static_cast<double>(mousePos.y);

		ScriptManager::Event::Value val3{L"isDown"};
		val3.val = static_cast<bool>(isDownWheelDelta);

		ScriptManager::Event::Value val4{L"button"};
		val4.val = static_cast<double>(clickType);

		ScriptManager::Event ev{L"click", { val, val2, val3, val4 }, true};
		if (Latite::getScriptManager().dispatchEvent(ev)) return;
	}

	return OnClickHook->oFunc<decltype(&onClick)>()(map, clickType, isDownWheelDelta, a4, a5, a6, a7, a8);
}

BOOL __stdcall GenericHooks::hkLoadLibraryW(LPCWSTR lib) {
	// prevent double injections
#ifdef LATITE_BETA
	abort();
#endif
	return 0;
}

void* __fastcall GenericHooks::RakNetConnector_tick(void* connector) {
	SDK::RakNetConnector::setInstance(reinterpret_cast<SDK::RakNetConnector*>(connector));
	return ConnectorTickHook->oFunc<decltype(&RakNetConnector_tick)>()(connector);
}

int __fastcall GenericHooks::RakPeer_getAveragePing(void* obj, char* guidOrAddy) {
	int ping = AveragePingHook->oFunc<decltype(&RakPeer_getAveragePing)>()(obj, guidOrAddy);
	AveragePingEvent ev{ ping };
	Eventing::get().dispatch(ev);
	return ping;
}

void __fastcall GenericHooks::LocalPlayer_applyTurnDelta(void* obj, Vec2& rot) {
	CinematicCameraEvent ev{false};
	Eventing::get().dispatch(ev);

	float intensity = 60.f;
	static std::chrono::time_point lastTime = std::chrono::system_clock::now();
	std::chrono::time_point curTime = std::chrono::system_clock::now();

	auto diff = curTime - lastTime;
	lastTime = curTime;
	float mult = (float)(std::chrono::duration<double, std::milli>(diff).count() / 13.f); // 60ish FPS
	float integ = intensity / mult;

	static Vec2 lerpRot = { 0, 0 };
	if (ev.getValue()) {
		TurnDeltaHook->oFunc<decltype(&LocalPlayer_applyTurnDelta)>()(obj, lerpRot);
		Vec2 extract = rot / (Vec2(integ, integ));
		Vec2 integExtract = lerpRot / (Vec2(integ, integ));
		lerpRot = lerpRot - (integExtract);
		lerpRot = lerpRot + (extract);
		return;
	}
	lerpRot = { 0, 0 };
	TurnDeltaHook->oFunc<decltype(&LocalPlayer_applyTurnDelta)>()(obj, rot);
}

void __fastcall GenericHooks::MoveInputHandler_tick(void* obj, void* proxy) {
	SDK::MoveInputComponent* hand = reinterpret_cast<SDK::MoveInputComponent*>(obj);
	{
		BeforeMoveEvent ev{ hand };
		if (Eventing::get().dispatch(ev)) return;
	}
	MoveInputHandler_tickHook->oFunc<decltype(&MoveInputHandler_tick)>()(obj, proxy);
	{
		AfterMoveEvent ev{ hand };
		Eventing::get().dispatch(ev);
	}
}

void __fastcall GenericHooks::MovePlayer(uintptr_t** a1, void* a2, uintptr_t* a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11) {
	CONTEXT ctx;
	RtlCaptureContext(&ctx);
	
	SDK::MoveInputComponent* hand = reinterpret_cast<SDK::MoveInputComponent*>(ctx.R9);
	{
		BeforeMoveEvent ev{ hand };
		if (Eventing::get().dispatch(ev)) return;
	}
	MovePlayerHook->oFunc<decltype(&MovePlayer)>()(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
	{
		AfterMoveEvent ev{ hand };
		Eventing::get().dispatch(ev);
	}
}

GenericHooks::GenericHooks() : HookGroup("General") {
	LoadLibraryHook = addHook((uintptr_t)&::LoadLibraryW, hkLoadLibraryW);
	LoadLibraryHook = addHook((uintptr_t) & ::LoadLibraryA, hkLoadLibraryW);


	Level_tickHook = addHook(Signatures::Level_tick.result,
		Level_tick, "Level::tick");

	ChatScreenController_sendChatMesageHook = addHook(Signatures::ChatScreenController_sendChatMessage.result,
		ChatScreenController_sendChatMessage, "ChatScreenController::sendChatMessage");

	GameRenderer_renderCurrentFrameHook = addHook(Signatures::GameRenderer__renderCurrentFrame.result,
		GameRenderer_renderCurrentFrame, "GameRenderer::_renderCurrentFrame");

	Keyboard_feedHook = addHook(Signatures::Keyboard_feed.result, Keyboard_feed, "Keyboard::feed");

	OnClickHook = addHook(Signatures::onClick.result, onClick, "onClick");

	ConnectorTickHook = addHook(Signatures::RakNetConnector_tick.result, RakNetConnector_tick, "RakNetConnector::tick");

	AveragePingHook = addHook(Signatures::RakPeer_GetAveragePing.result, RakPeer_getAveragePing, "RakPeer::GetAveragePing");
	
	TurnDeltaHook = addHook(Signatures::LocalPlayer_applyTurnDelta.result, LocalPlayer_applyTurnDelta, "LocalPlayer::applyTurnDelta");

	if (Signatures::MovePlayer.result) {
		MovePlayerHook = addHook(Signatures::MovePlayer.result, MovePlayer, "MovePlayer");
	}
	else {
		MoveInputHandler_tickHook = addHook(Signatures::MoveInputHandler_tick.result, MoveInputHandler_tick, "MoveInputHandler::tick");
	}
}
