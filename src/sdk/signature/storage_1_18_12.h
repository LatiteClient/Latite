#pragma once

class Signatures_1_18_12 {
public:
	struct Offset {

		// manual offsets

		inline static SigImpl MinecraftGame_cursorGrabbed{};

		inline static SigImpl LevelRendererPlayer_fovX{};

		inline static SigImpl LevelRendererPlayer_origin{};

	};
	struct Misc {
		inline static SigImpl minecraftGamePointer {};

		inline static SigImpl clientInstance{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8B 15 ? ? ? ? 4C 8B 02 4C 89 06 40 84 FF 74 ? 48 8B CD E8 ? ? ? ? 48 8B C6 48 8B 4C 24 ? 48 33 CC E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 6C 24 ? 48 8B 74 24 ? 48 83 C4 ? 5F C3 B9 ? ? ? ? E8 ? ? ? ? CC E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 6C 24 ? 56",
			"ClientInstance"};
		inline static SigImpl clickMap{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 0d ? ? ? ? e8 ? ? ? ? b3 ? 48 85 f6",
			"ClickMap"};
		inline static SigImpl uiColorMaterial{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8b 05 ? ? ? ? 48 85 c0 74 ? 48 83 78",
			"UIFillColorMaterial"};
	};

	struct Vtable {
		inline static SigImpl TextPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			// last 4 bytes is the hash of the component
			"48 8D 05 ?? ?? ?? ?? 48 8B F9 48 89 01 48 83 C1 ?? E8 ?? ?? ?? ?? 48 8D 8F",
			"const TextPacket::vftable"};
		inline static SigImpl CommandRequestPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			// last 4 bytes is the hash of the component
			"4c 8d 3d ? ? ? ? 4c 89 7d ? 48 89 75 ? 66 0f 7f 45",
			"const TextPacket::`vftable'"};
		inline static SigImpl Level{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 07 48 8d 05 ? ? ? ? 48 89 47 ? 48 8d 05 ? ? ? ? 48 89 47 ? 33 c0",
			"const Level::`vftable'"};
		inline static SigImpl SetTitlePacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 01 89 51 ? 48 83 c1 ? 0f 57 c0 0f 11 01 48 89 79",
			"const SetTitlePacket::`vftable'"};
	};

	struct Components {
		inline static SigImpl moveInputComponent{};
		inline static SigImpl runtimeIDComponent{};
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 57 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24 ? ? ? ? 49 8b f8 48 8b da 48 8b f1 33 d2",
		"LevelRenderer::renderLevel"};

	inline static SigImpl Keyboard_feed{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 57 48 83 EC ?? 8B 05 ?? ?? ?? ?? 8B DA",
		"Keyboard::feed"};

	// The signature is big but it hasn't died in a while soo
	inline static SigImpl Options_getGamma{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 74 ? 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 f3 0f 10 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc f3 0f 11 4c 24",
		"Keyboard::feed"};

	inline static SigImpl Options_getPerspective{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 48 8b 01 48 8d 54 24 ? 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 8b 42 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc cc 48 83 ec ? 48 8b 01 48 8d 54 24 ? 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 8b 42 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc cc 48 83 ec ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 ? 48 8b 01 41 b8 ? ? ? ? 88 54 24 ? 48 8d 54 24 ? ff 50 ? 48 8b 08 48 85 c9 74 ? 48 8d 54 24 ? e8 ? ? ? ? 48 8b 4c 24 ? 48 33 cc e8 ? ? ? ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc 48 83 ec ? 48 8b 01 48 8d 54 24 ? 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 0f b6 42 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc 48 83 ec ? 48 8b 01",
		"Options::getPerspective"};

	inline static SigImpl Options_getHideHand{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 74 ? 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 0f b6 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 50 ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc 48 83 ec ? 48 8b 05",
		"Options::getHideHand"};

	inline static SigImpl Options_getSensitivity{[](memory::signature_store&, uintptr_t res) { return res; },
		"4c 8b d1 44 0f b6 ca 49 bb ? ? ? ? ? ? ? ? 48 b8 ? ? ? ? ? ? ? ? 4c 33 c8 8b c2 4d 0f af cb c1 e8 ? 44 0f b6 c0 8b c2 4d 33 c8 c1 e8 ? 4d 8b 42 ? 4d 0f af cb 0f b6 c8 4c 33 c9 8b c2 49 8b 4a ? 4d 0f af cb 48 c1 e8 ? 4c 33 c8 4d 0f af cb 49 23 c9 4d 8b 4a",
		"Options::getSensitivity"};

	// manual indices for 1.18.12

	inline static SigImpl ClientInstance_grabCursor{};

	inline static SigImpl ClientInstance_releaseCursor{};

	inline static SigImpl Level_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 48 89 7c 24 ? 55 41 54 41 55 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 4c 8b f1 8b 81",
		"Level::tick"};

	inline static SigImpl ChatScreenController_sendChatMessage{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 4c 8b ea 4c 8b f9 48 8b 49",
		"ChatScreenController::sendChatMessage"};

	// callsites

	inline static SigImpl onClick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 ec ? 44 0f b7 bc 24",
		"onClick"};

	inline static SigImpl GameRenderer__renderCurrentFrame{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 89 58 ?? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ?? ?? ?? ?? 48 81 EC ?? ?? ?? ?? 0F 29 70 ?? 0F 29 78 ?? 44 0F 29 40 ?? 44 0F 29 48 ?? 44 0F 29 90 ?? ?? ?? ?? 44 0F 29 98 ?? ?? ?? ?? 44 0F 29 A0 ?? ?? ?? ?? 44 0F 29 A8 ?? ?? ?? ?? 44 0F 29 B0 ?? ?? ?? ?? 44 0F 29 B8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 ?? ?? ?? ?? 0F 28 F1",
		"GameRenderer::_renderCurrentFrame"};

	inline static SigImpl AppPlatform__fireAppFocusLost{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 8f ? ? ? ? 48 8b 49 ? e8",
		"AppPlatform::_fireAppFocusLost"};

	inline static SigImpl MinecraftGame_onAppSuspended{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 56 48 8b ec 48 83 ec ? 48 8b f1 e8",
		"MinecraftGame::onAppSuspended"};

	inline static SigImpl RenderController_getOverlayColor{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 0f 28 05 ? ? ? ? 0f 11 85 ? ? ? ? 4c 8d 8d",
		"RenderController::getOverlayColor"};

	inline static SigImpl ScreenView_setupAndRender{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 89 58 ?? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ?? ?? ?? ?? 48 81 EC ?? ?? ?? ?? 0F 29 70 ?? 0F 29 78 ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 ?? ?? ?? ?? 4C 8B FA",
		"ScreenView::setupAndRender"};

	inline static SigImpl KeyMap{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8d 0d ? ? ? ? 89 1c b9",
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
		"48 8b c4 48 89 58 ? 48 89 68 ? 56 57 41 56 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 44 0f 29 40 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 ? 4c 8b f2",
		"LocalPlayer::applyTurnDelta"};

	// see what accesses things in moveinputhandler
	inline static SigImpl MovePlayer{};

	// see what accesses things in moveinputhandler
	// https://github.com/Imrglop/LatiteClient/blob/release/Latite/Functions.cpp#L135
	inline static SigImpl MoveInputHandler_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 0F B6 41 ?? 40 32 F6",
		"MoveInputHandler::tick"};

	// see what accesses the Y camera position value in Camera
	inline static SigImpl CameraViewBob{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 ? 49 8b 00",
		"`anonymous namespace'::_bobMovement"};

	inline static SigImpl ItemStackBase_getHoverName{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 74 24 ?? 57 48 81 EC ?? ?? ?? ?? 48 8B FA 48 8B D9 48 89 94 24",
		"ItemStackBase::getHoverName"};


	inline static SigImpl Tessellator_vertex{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13",
		"Tessellator::vertex"};

	inline static SigImpl Tessellator_begin{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 55 48 83 EC ?? 80 B9 ?? ?? ?? ?? 00 45",
		"Tessellator::begin"};

	inline static SigImpl Tessellator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 b9 ? ? ? ? ? 4c 8b c1 75",
		"Tessellator::color"};

	inline static SigImpl MeshHelpers_renderMeshImmediately{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 90 48 85 ed 74 ? c6 45 ? ? 48 83 6d ? ? 75 ? 48 c7 45",
		"MeshHelpers::renderMeshImmediately"};

	inline static SigImpl BaseActorRenderContext_BaseActorRenderContext{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 74 24 ?? 48 89 4C 24 ?? 57 48 83 EC ?? 49 8B D8 48 8B F9 48 8D 05",
		"BaseActorRenderContext::BaseActorRenderContext"};

	inline static SigImpl ItemRenderer_renderGuiItemNew{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 53 55 56 57 41 54 41 55 41 56 41 57 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24 ? ? ? ? 44 89 8c 24",
		"ItemRenderer::renderGuiItemNew"};

	inline static SigImpl Actor_getAttribute{};

	inline static SigImpl UIControl_setPosition{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 6c 24 ? 48 89 74 24 ? 57 48 83 ec ? 0f 29 74 24 ? 0f 29 7c 24 ? 48 8b 05",
		"UIControl::setPosition"};
};

// after adding sigs here, add them in latite.cpp