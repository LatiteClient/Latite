#pragma once
#ifndef LATITE_DEBUG
#define API_NAMES
#endif


class Signatures {
public:
	struct Offset {
		inline static SigImpl MinecraftGame_cursorGrabbed{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"80 b9 ? ? ? ? ? 0f 84 ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 48 85 c0",
			"MinecraftGame->cursorGrabbed"};

		inline static SigImpl LevelRendererPlayer_fovX{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? f3 0f 10 0d",
			"LevelRendererPlayer->fovX"};

		//89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? f3 0f 10 0d

		inline static SigImpl LevelRendererPlayer_origin{[](memory::signature_store& store, uintptr_t) { return store.ref(4); },
			"F3 0F 58 BB ? ? ? ? 48 8B 8B ? ? ? ?",
			"LevelRendererPlayer->origin"};

	};
	struct Misc {
		inline static SigImpl clientInstance{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			// god what a fat signature
			"48 8b 0d ? ? ? ? 48 89 43 ? 48 8b c3 48 89 3b c6 43 ? ? 48 89 4b ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 33 c0 48 8b cf 48 89 03 88 43 ? 48 89 43 ? 48 89 43 ? e8 ? ? ? ? 48 8b c3 48 8b 5c 24 ? 48 83 c4 ? 5f c3 cc 48 89 5c 24 ? 48 89 74 24 ? 57 48 83 ec ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 ? 48 8b da 48 8b f1 48 89 4c 24 ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 85 c0 0f 95 c1 85 c0 74 ? 48 8b 05 ? ? ? ? 48 85 c0 75 ? 40 b7 ? eb ? 84 c9 74 ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 40 32 ff 40 88 7c 24 ? 40 84 ff 75 ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 40 b7 ? 40 88 7c 24 ? 48 85 db 0f 84 ? ? ? ? 48 c7 05 ? ? ? ? ? ? ? ? 48 8b 0d ? ? ? ? 48 c7 05 ? ? ? ? ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 8b 43 ? 48 85 c0 74 ? f0 ff 40 ? 0f b6 7c 24 ? 48 8b 43 ? 48 8b 53 ? 48 89 15 ? ? ? ? 48 8b 0d ? ? ? ? 48 89 05 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 8b 15 ? ? ? ? 48 8b 12 48 89 16 40 84 ff 74 ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8b c6 48 8b 4c 24 ? 48 33 cc e8 ? ? ? ? 48 8b 5c 24 ? 48 8b 74 24 ? 48 83 c4 ? 5f c3 e8 ? ? ? ? 90 cc cc cc 48 89 5c 24 ? 57 48 83 ec ? 48 8b 19",
			"ClientInstance"};
		//
		inline static SigImpl clickMap{[](memory::signature_store& store, uintptr_t) { return store.deref(2); },
			"c7 05 ? ? ? ? ? ? ? ? 45 32 e4",
			"ClickMap"};

		inline static SigImpl uiColorMaterial{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8b 05 ? ? ? ? 48 85 c0 74 ? 48 83 78",
			"UIFillColorMaterial"};
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
			"48 8d 05 ? ? ? ? 48 89 85 ? ? ? ? c6 85 ? ? ? ? ? 0f 11 85 ? ? ? ? 4c 89 a5",
			"const TextPacket::`vftable'"};
		inline static SigImpl CommandRequestPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 73 ? 48 89 43 ? c7 43 ? ? ? ? ? c7 43 ? ? ? ? ? 66 89 73 ? 48 89 73 ? 89 73 ? 0f 11 43 ? 48 89 73 ? 48 c7 43 ? ? ? ? ? 40 88 73 ? 40 88 73 ? 48 89 73",
			"const TextPacket::`vftable'"};
		inline static SigImpl Level{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 07 48 8d 05 ? ? ? ? 48 89 47 ? 48 8d 05 ? ? ? ? 48 89 47 ? 33 c0",
			"const Level::`vftable'"};

		//48 8d 05 ? ? ? ? 48 89 07 48 8d 05 ? ? ? ? 48 89 47 ? 48 8d 05 ? ? ? ? 48 89 47 ? 33 c0
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 56 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 49 8b f0 48 8b da 4c 8b f1",
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
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8b ec 48 83 ec ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 48 8b f9",
		"Level::tick"};

	// callsites

	inline static SigImpl ChatScreenController_sendChatMessage{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 3c ? 75 ? 48 8b 8f",
		"ChatScreenController::sendChatMessage"};

	inline static SigImpl GameRenderer__renderCurrentFrame{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 48 8b 86",
		"GameRenderer::_renderCurrentFrame"};

	inline static SigImpl AppPlatform__fireAppFocusLost{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 87 ? ? ? ? 4c 8b 70",
		"AppPlatform::_fireAppFocusLost"};

	inline static SigImpl onClick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 ec ? 44 0f b7 bc 24",
		"onClick"};

	//ff 15 ? ? ? ? 48 8d 4c 24 ? e8 ? ? ? ? 48 8b 4c 24 ? 48 3b cb
	/*
	"SuspendAudio"
	"forceSendEvents"
	"SuspendUserManager"
	"SuspendGameRenderer
	"unloadAllTextures""*/
	inline static SigImpl MinecraftGame_onAppSuspended{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 56 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 4c 8b f1 48 8d b1",
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
		"48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 45 33 FF 4C 89 BD ? ? ? ?",
		"RakNetConnector::tick"};
	
	// ref: your GPU ("AMD Radeon RX 5500")
	inline static SigImpl GpuInfo{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8d 15 ? ? ? ? 48 c7 c7 ? ? ? ? 4c 8b c7 49 ff c0 42 80 3c 02 ? 75 ? 48 8d 4c 24",
		"GpuInfo"};

	// ref: RakPeer vtable
	inline static SigImpl RakPeer_GetAveragePing{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 81 ec ? ? ? ? 4c 8b d1 48 8d 4c 24 ? e8 ? ? ? ? 48 8b d0 45 33 c0 45 33 c9 49 8b ca e8 ? ? ? ? 4c 8b c0",
		"RakPeer::GetAveragePing"};

	inline static SigImpl LocalPlayer_applyTurnDelta{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 48 89 70 ? 55 57 41 54 41 56 41 57 48 8d 68 ? 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 44 0f 29 40 ? 44 0f 29 48 ? 44 0f 29 50 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 4c 8b e2",
		"LocalPlayer::applyTurnDelta"};

	// see what accesses things in moveinputhandler
	inline static SigImpl MovePlayer{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 83 ec ? 4d 8b f9 4c 8b ea",
		"MovePlayer"};

	inline static SigImpl MoveInputHandler_tick{};

	inline static SigImpl Tesesllator_vertex{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13",
		"Tessellator::vertex"};

	inline static SigImpl Tesesllator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 b9 ? ? ? ? ? 4c 8b c1 75",
		"Tessellator::color"};

	inline static SigImpl CameraViewBob{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 ? 49 8b 00",
		"CameraViewBob"};

	inline static SigImpl ItemStackBase_getHoverName{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 74 24 ?? 57 48 81 EC ?? ?? ?? ?? 48 8B FA 48 8B D9 48 89 94 24",
		"ItemStackBase::getHoverName"};

	inline static SigImpl SetTitlePacket_readExtended{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 55 53 56 57 41 56 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 4d 8b f0 48 8b fa 48 8b d9 48 89 54 24 ? 48 8d 55",
		"SetTimePacket::readExtended"};


	inline static SigImpl Tessellator_vertex{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13",
		"Tessellator::vertex"};

	inline static SigImpl Tessellator_begin{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 56 48 83 ec ? 80 b9 ? ? ? ? ? 45 0f b6 d1",
		"Tessellator::begin"};

	inline static SigImpl Tessellator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 b9 ? ? ? ? ? 4c 8b c1 75",
		"Tessellator::color"};

	inline static SigImpl MeshHelpers_renderMeshImmediately{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 57 48 81 ec ? ? ? ? 49 8b f8 48 8b da 48 8b f1 80 ba",
		"MeshHelpers::renderMeshImmediately"};
};

// after adding sigs here, add them in latite.cpp