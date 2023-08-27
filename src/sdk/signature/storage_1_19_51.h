#pragma once
#ifndef LATITE_DEBUG
#define API_NAMES
#endif


class Signatures_1_19_51 {
public:
	struct Offset {
		inline static SigImpl MinecraftGame_cursorGrabbed{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"80 b9 ? ? ? ? ? 0f 84 ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 48 85 c0",
			"MinecraftGame->cursorGrabbed"};

		inline static SigImpl LevelRendererPlayer_fovX{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45 ? 89 86 ? ? ? ? 41 8b 45",
			"LevelRendererPlayer->fovX"};

		//89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? f3 0f 10 0d

		inline static SigImpl LevelRendererPlayer_origin{[](memory::signature_store& store, uintptr_t) { return store.ref(4); },
			"f3 0f 58 bb ? ? ? ? f3 0f 11 7c 24",
			"LevelRendererPlayer->origin"};

	};
	struct Misc {
		inline static SigImpl clientInstance{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8B 15 ?? ?? ?? ?? 48 8B 12 48 89 16 40 84 FF 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C6 48 8B 4C 24 ?? 48 33 CC E8 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 8B 74 24 ?? 48 83 C4 ?? 5F C3 E8 ?? ?? ?? ?? 90 CC CC CC CC CC CC CC CC CC 48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 ?? 48 8B DA 48 8B F1 48 89 4C 24 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0 0F 95 C1 85 C0 74 ?? 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 ?? 40 B7 ?? EB ?? 84 C9 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 40 32 FF 40 88 7C 24 ?? 40 84 FF 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 40 B7 ?? 40 88 7C 24 ?? 48 85 DB 0F 84 ?? ?? ?? ?? 33 C0 48 89 05 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 48 89 05 ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 43 ?? 48 85 C0 74 ?? F0 FF 40 ?? 48 8B 43 ?? 0F B6 7C 24 ?? 48 8B 53 ?? 48 89 15 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 48 89 05 ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 15 ?? ?? ?? ?? 48 8B 12 48 89 16 40 84 FF 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C6 48 8B 4C 24 ?? 48 33 CC E8 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 8B 74 24 ?? 48 83 C4 ?? 5F C3 E8 ?? ?? ?? ?? 90 CC CC CC CC CC CC CC CC CC 40 53 48 83 EC ?? 48 8B D9",
			"ClientInstance"};
	};

	struct Components {
		inline static SigImpl moveInputComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"40 53 48 83 EC ? 48 8B DA BA 2E CD 8B 46",
			"MoveInputComponent::try_get"};

		inline static SigImpl runtimeIDComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"40 53 48 83 EC ? 48 8B DA BA 14 14 A1 3C",
			"ActorRuntimeIDComponent"};
	};

	struct Vtable {
		inline static SigImpl TextPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			// last 4 bytes is the hash of the component
			"48 8D 05 ?? ?? ?? ?? 48 8B F9 48 89 01 48 83 C1 ?? E8 ?? ?? ?? ?? 48 8D 8F",
			"const TextPacket::vftable"};
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 55 56 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 49 8b f0 48 8b da",
		"LevelRenderer::renderLevel"};

	inline static SigImpl Keyboard_feed{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 0f b6 c1 4c 8d 05",
		"Keyboard::feed"};

	// The signature is big but it hasn't died in a while soo
	inline static SigImpl Options_getGamma{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 48 8b 40 ? 74 ? 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 f3 0f 10 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc f3 0f 11 4c 24",
		"Options::getGamma"};

	inline static SigImpl ClientInstance_grabCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ?? 48 8B 01 48 8B D9 48 8B 80 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 84 C0 74 ?? 48 8B 8B ?? ?? ?? ?? 48 8B 01 48 8B 80 ?? ?? ?? ?? 48 83 C4 ?? 5B 48 FF 25 ?? ?? ?? ?? 48 83 C4 ?? 5B C3 40 53",
		"ClientInstance::grabCursor"};

	inline static SigImpl ClientInstance_releaseCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ?? 48 8B 01 48 8B D9 48 8B 80 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 84 C0 74 ?? 48 8B 8B ?? ?? ?? ?? 48 8B 01 48 8B 80 ?? ?? ?? ?? 48 83 C4 ?? 5B 48 FF 25 ?? ?? ?? ?? 48 83 C4 ?? 5B C3 48 89 5C 24",
		"ClientInstance::releaseCurosr"};

	inline static SigImpl Level_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 74 24 ?? 55 57 41 54 41 56 41 57 48 8D AC 24 ?? ?? ?? ?? 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 ?? ?? ?? ?? 4C 8B F1 8B 81",
		"Level::tick"};

	// callsites

	inline static SigImpl ChatScreenController_sendChatMessage{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 3c ? 75 ? 48 8b 8f",
		"ChatScreenController::sendChatMessage"};

	inline static SigImpl GameRenderer__renderCurrentFrame{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 48 8b 86",
		"GameRenderer::_renderCurrentFrame"};

	inline static SigImpl AppPlatform__fireAppFocusLost{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 8f ? ? ? ? 48 8b 49 ? e8", // hopefully this works
		"AppPlatform::_fireAppFocusLost"};

	inline static SigImpl onClick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 ec ? 44 0f b7 bc 24",
		"onClick"};

	inline static SigImpl MinecraftGame_onAppSuspended{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 56 48 8b ec 48 83 ec ? 48 8b f1 e8",
		"MinecraftGame::onAppSuspended"};

	inline static SigImpl RenderController_getOverlayColor{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 0f 28 05 ? ? ? ? 0f 11 85 ? ? ? ? 4c 8d 8d",
		"RenderController::getOverlayColor"};

	inline static SigImpl ScreenView_setupAndRender{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 44 24 ? 48 8d 4c 24 ? 48 8b 80",
		"ScreenView::setupAndRender"};

	inline static SigImpl KeyMap{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"4c 8d 05 ? ? ? ? 89 54 24 ? 88 4c 24",
		"KeyMap"};

	inline static SigImpl MinecraftGame__update{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 8b ? ? ? ? ba ? ? ? ? 48 83 c4",
		"MinecraftGame::_update"};

	// "Nat Punch timed out"
	inline static SigImpl RakNetConnector_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 48 8b f9 45 33 ff",
		"RakNetConnector::tick"};

	// ref: your GPU ("AMD Radeon RX 5500")
	inline static SigImpl GpuInfo{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"4c 8d 0d ? ? ? ? 48 c7 c3 ? ? ? ? 48 8b fb 48 ff c7 41 80 3c 39 ? 75 ? 48 8d 4c 24 ? 48 83 ff ? 77 ? 48 89 7c 24 ? 4c 8b c7 49 8b d1 e8 ? ? ? ? c6 44 3c ? ? eb ? 48 8b d7 e8 ? ? ? ? 48 8d 4c 24",
		"GpuInfo"};

	// ref: RakPeer vtable
	inline static SigImpl RakPeer_GetAveragePing{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 81 ec ? ? ? ? 4c 8b d1 48 8d 4c 24 ? e8 ? ? ? ? 48 8b d0 45 33 c0 45 33 c9 49 8b ca e8 ? ? ? ? 4c 8b c0",
		"RakPeer::GetAveragePing"};

	inline static SigImpl LocalPlayer_applyTurnDelta{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 55 56 57 41 56 41 57 48 8d 68 ? 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 44 0f 29 40 ? 44 0f 29 48 ? 44 0f 29 50 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 4c 8b fa",
		"LocalPlayer::applyTurnDelta"};

	// see what accesses things in moveinputhandler
	inline static SigImpl MovePlayer{};

	// see what accesses things in moveinputhandler
	// https://github.com/Imrglop/LatiteClient/blob/release/Latite/Functions.cpp#L135
	inline static SigImpl MoveInputHandler_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 0F B6 41 ?? 40 32 F6",
		"MoveInputHandler::tick"};
};


// after adding sigs here, add them in latite.cpp