#pragma once
#include <util/XorString.h>
#include <mnemosyne/scan/signature.hpp>

// 1.20.71
class Signatures {
public:
	struct Offset {
		inline static SigImpl MinecraftGame_cursorGrabbed{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"38 99 ? ? ? ? 0f 85 ? ? ? ? 48 83 c1"_sig,
			"MinecraftGame->cursorGrabbed"};

		inline static SigImpl LevelRendererPlayer_fovX{};

		//89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? f3 0f 10 0d

		inline static SigImpl LevelRendererPlayer_origin{[](memory::signature_store& store, uintptr_t) { return store.ref(4); },
			"F3 0F 58 BB ? ? ? ? 48 8B 8B ? ? ? ?"_sig,
			"LevelRendererPlayer->origin"};

	};
	struct Misc {
		inline static SigImpl clientInstance{};

		inline static SigImpl minecraftGamePointer {[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 89 0d ? ? ? ? 48 8b 00"_sig,
			"MinecraftGame"};
		inline static SigImpl clickMap{[](memory::signature_store& store, uintptr_t) { return store.deref(2); },
			"89 0D ? ? ? ? 40 B5"_sig, // MouseDevice::_instance
			"ClickMap"};

		inline static SigImpl uiColorMaterial{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8b 05 ? ? ? ? 48 85 c0 74 ? 48 83 78"_sig,
			"UIFillColorMaterial"};
		inline static SigImpl thirdPersonNametag{ [](memory::signature_store&, uintptr_t res) { return res; },
			"0f 84 ? ? ? ? 49 8b d6 48 8b ce e8 ? ? ? ? 84 c0 0f 84"_sig,
			"ThirdPersonNametag" };
		
	};

	struct Components {
		inline static SigImpl moveInputComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"48 89 5C 24 08 57 48 83 EC 30 48 8B DA BA 2E CD 8B 46"_sig,
			"MoveInputComponent::try_get"};

		inline static SigImpl runtimeIDComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"48 89 5C 24 08 57 48 83 EC 30 48 8B DA BA 14 14 A1 3C"_sig,
			"ActorRuntimeIDComponent"};
		

		inline static SigImpl actorTypeComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"48 89 5C 24 08 57 48 83 EC 30 48 8B DA BA 14 AD F3 51"_sig,
			"ActorTypeComponent"}; //51F3AD14

		inline static SigImpl attributesComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"48 89 5C 24 08 57 48 83 EC 30 48 8B DA BA 44 94 B2 B6"_sig,
			"AttributesComponent"};

		inline static SigImpl actorEquipmentPersistentComponent{ [](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 41 48 4C 8B C9 48 8B 41 50 4C 8B 51 68 49 2B C0 48 C1 F8 03 48 FF C8 25 36 48 C4 71"_sig,
			"struct ActorEquipmentComponent" };

		//76 59 47 33
		inline static SigImpl actorDataFlagsComponent{ [](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"48 89 5C 24 08 57 48 83 EC 30 48 8B DA BA 76 59 47 33"_sig,
			"struct ActorDataFlagsComponent"};
	};

	struct Vtable {
		inline static SigImpl TextPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 01 33 d2 48 89 51 ? 0f 57 c0 48 89 51 ? 48 8b c1 89 51"_sig,
			"const TextPacket::`vftable'"};
		inline static SigImpl CommandRequestPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 73 ? 48 89 43 ? c7 43 ? ? ? ? ? c7 43 ? ? ? ? ? 66 89 73 ? 48 89 73 ? 89 73 ? 0f 11 43 ? 48 89 73 ? 48 c7 43 ? ? ? ? ? 40 88 73 ? 40 88 73 ? 48 89 73"_sig,
			"const CommandRequestPacket::`vftable'"};

		// "Client%d camera ticking system"
		// 1st of 3 data LEA's
		inline static SigImpl Level{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8D 05 ? ? ? ? 48 89 03 48 8D 05 ? ? ? ? 48 89 43 ? 48 8D 05 ? ? ? ? 48 89 43 ? 48 8D BB"_sig,
			"const Level::`vftable'"};
		inline static SigImpl SetTitlePacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 01 89 51 ? 48 83 c1 ? 0f 57 c0 0f 11 01 48 89 79"_sig,
			"const SetTitlePacket::`vftable'"};

		//

		//48 8d 05 ? ? ? ? 48 89 07 48 8d 05 ? ? ? ? 48 89 47 ? 48 8d 05 ? ? ? ? 48 89 47 ? 33 c0
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 49 8B F8 48 8B DA 48 8B F1 33 D2"_sig,
		"LevelRenderer::renderLevel"};

	inline static SigImpl Keyboard_feed{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 0f b6 c1 4c 8d 05"_sig,
		"Keyboard::feed"};

	// The signature is big but it hasn't died in a while soo
	inline static SigImpl Options_getGamma{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? 80 B9 ? ? ? ? ? 48 8D 54 24 ? 48 8B 01 48 8B 40 ? 74 ? 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 74 ? 48 8B 42 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 83 C4 ? C3 F3 0F 10 42 ? 48 83 C4 ? C3 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 75 ? E8 ? ? ? ? CC E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 40 53 48 83 EC ? 48 8B 01"_sig,
		"Options::getGamma"};

	inline static SigImpl Options_getPerspective{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 48 8b 01 48 8d 54 24 ? 41 b8 ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 8b 42 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc 48 83 ec ? 48 8b 01 48 8d 54 24 ? 41 b8 ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 8b 42 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc 48 89 5c 24 ? 57 48 83 ec ? 48 8b 01 0f b6 fa 41 b8 ? ? ? ? 48 8d 54 24 ? 48 8b 40 ? ff 15 ? ? ? ? 48 8b 18 48 85 db 74 ? 48 8b 43 ? 48 83 b8 ? ? ? ? ? 75 ? 48 8b cb e8 ? ? ? ? 84 c0 74 ? 41 b0 ? 40 0f b6 d7 48 8b cb e8 ? ? ? ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc 48 83 ec ? 48 8b 01 48 8d 54 24 ? 41 b8 ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 0f b6 42 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc 40 53"_sig,
		"Options::getPerspective"};

	inline static SigImpl Options_getHideHand{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 48 8b 40 ? 74 ? 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 0f b6 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc 48 89 5c 24"_sig,
		"Options::getHideHand"};

	inline static SigImpl Options_getSensitivity{};

	inline static SigImpl ClientInstance_grabCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ?? 48 8B 01 48 8B D9 48 8B 80 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 84 C0 74 ?? 48 8B 8B ?? ?? ?? ?? 48 8B 01 48 8B 80 ?? ?? ?? ?? 48 83 C4 ?? 5B 48 FF 25 ?? ?? ?? ?? 48 83 C4 ?? 5B C3 40 53"_sig,
		"ClientInstance::grabCursor"};

	inline static SigImpl ClientInstance_releaseCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ?? 48 8B 01 48 8B D9 48 8B 80 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 84 C0 74 ?? 48 8B 8B ?? ?? ?? ?? 48 8B 01 48 8B 80 ?? ?? ?? ?? 48 83 C4 ?? 5B 48 FF 25 ?? ?? ?? ?? 48 83 C4 ?? 5B C3 48 89 5C 24"_sig,
		"ClientInstance::releaseCurosr"};

	inline static SigImpl Level_tick{ [](memory::signature_store& stor, uintptr_t res) { return stor.deref(1); },
		"e8 ? ? ? ? 48 8b 4b ? 48 85 c9 74 ? 48 8b 41 ? 48 83 c1 ? 48 8b 40"_sig,
		"Level::tick" };

	// callsites

	inline static SigImpl ChatScreenController_sendChatMessage{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 3c ? 75 ? 48 8b 8b"_sig,
		"ClientInstanceScreenModel::sendChatMessage"};

	inline static SigImpl GameRenderer__renderCurrentFrame{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 48 8b 86"_sig,
		"GameRenderer::_renderCurrentFrame"};

	//48 89 5c 24 ? 48 89 74 24 ? 57 48 83 ec ? 48 8b f1 c6 81 ? ? ? ? ? 48 8d b9 ? ? ? ? 48 89 7c 24 ? 48 8b cf ff 15 ? ? ? ? 85 c0 74 ? 8b c8 ff 15 ? ? ? ? 90 48 8b 5e ? 48 8b 1b 80 7b ? ? 75 ? 0f 1f 84 00 ? ? ? ? 48 8b 4b ? 48 8b 01 48 8b 40 ? ff 15 ? ? ? ? 48 8b 43 ? 80 78 ? ? 74 ? 48 8b 43 ? 80 78 ? ? 75 ? 48 3b 58 ? 75 ? 48 8b d8 48 8b 40 ? 80 78 ? ? 74 ? 48 8b d8 eb ? 48 8b d8 48 8b 08 80 79 ? ? 75 ? 0f 1f 80 ? ? ? ? 48 8b d9 48 8b 01 48 8b c8 80 78 ? ? 74 ? 80 7b ? ? 74 ? c7 86 ? ? ? ? ? ? ? ? 48 8b cf 48 8b 5c 24 ? 48 8b 74 24 ? 48 83 c4 ? 5f 48 ff 25 ? ? ? ? cc cc cc cc cc cc cc cc 48 89 5c 24 ? 48 89 74 24
	inline static SigImpl AppPlatform__fireAppFocusLost{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 57 48 83 ec ? 48 8b f1 c6 81 ? ? ? ? ? 48 8d b9 ? ? ? ? 48 89 7c 24 ? 48 8b cf ff 15 ? ? ? ? 85 c0 74 ? 8b c8 ff 15 ? ? ? ? 90 48 8b 5e ? 48 8b 1b 80 7b ? ? 75 ? 0f 1f 84 00 ? ? ? ? 48 8b 4b ? 48 8b 01 48 8b 40 ? ff 15 ? ? ? ? 48 8b 43 ? 80 78 ? ? 74 ? 48 8b 43 ? 80 78 ? ? 75 ? 48 3b 58 ? 75 ? 48 8b d8 48 8b 40 ? 80 78 ? ? 74 ? 48 8b d8 eb ? 48 8b d8 48 8b 08 80 79 ? ? 75 ? 0f 1f 80 ? ? ? ? 48 8b d9 48 8b 01 48 8b c8 80 78 ? ? 74 ? 80 7b ? ? 74 ? c7 86 ? ? ? ? ? ? ? ? 48 8b cf 48 8b 5c 24 ? 48 8b 74 24 ? 48 83 c4 ? 5f 48 ff 25 ? ? ? ? cc cc cc cc cc cc cc cc 48 89 5c 24 ? 48 89 74 24"_sig,
		"AppPlatform::_fireAppFocusLost"};

	inline static SigImpl onClick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 ec ? 44 0f b7 bc 24"_sig,
		"onClick"};

	//ff 15 ? ? ? ? 48 8d 4c 24 ? e8 ? ? ? ? 48 8b 4c 24 ? 48 3b cb
	/*
	"SuspendAudio"
	"forceSendEvents"
	"SuspendUserManager"
	"SuspendGameRenderer
	"unloadAllTextures""*/
	inline static SigImpl MinecraftGame_onAppSuspended{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 48 89 7c 24 ? 55 41 54 41 55 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 4c 8b e9 45 33 f6"_sig,
		"MinecraftGame::onAppSuspended"};

	inline static SigImpl RenderController_getOverlayColor{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 0f 28 05 ? ? ? ? 0f 11 85 ? ? ? ? 4c 8d 8d"_sig,
		"RenderController::getOverlayColor"};

	inline static SigImpl ScreenView_setupAndRender{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 44 24 ? 48 8d 4c 24 ? 48 8b 80"_sig,
		"ScreenView::setupAndRender"};

	inline static SigImpl KeyMap{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"4c 8d 05 ? ? ? ? 89 54 24 ? 88 4c 24"_sig,
		"KeyMap"};

	inline static SigImpl MinecraftGame__update{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 8b ? ? ? ? ba ? ? ? ? 48 83 c4"_sig,
		"MinecraftGame::_update"};

	// "Nat Punch timed out"
	inline static SigImpl RakNetConnector_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"4C 8B DC 49 89 5B ? 49 89 73 ? 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F9 49 C7 43"_sig,
		"RakNetConnector::tick"};
	
	// ref: your GPU ("AMD Radeon RX 5500")
	inline static SigImpl GpuInfo{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8d 15 ? ? ? ? 48 c7 c7 ? ? ? ? 4c 8b c7 49 ff c0 42 80 3c 02 ? 75 ? 48 8d 4c 24"_sig,
		"GpuInfo"};

	// ref: RakPeer vtable
	inline static SigImpl RakPeer_GetAveragePing{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 81 ec ? ? ? ? 4c 8b d1 48 8d 4c 24 ? e8 ? ? ? ? 48 8b d0 45 33 c0 45 33 c9 49 8b ca e8 ? ? ? ? 4c 8b c0"_sig,
		"RakPeer::GetAveragePing"};

	inline static SigImpl LocalPlayer_applyTurnDelta{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8b c4 48 89 58 ? 48 89 70 ? 55 57 41 54 41 56 41 57 48 8d 68 ? 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 44 0f 29 40 ? 44 0f 29 48 ? 44 0f 29 50 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 4c 8b e2"_sig,
		"LocalPlayer::applyTurnDelta"};

	// see what accesses things in moveinputhandler
	inline static SigImpl MovePlayer{[](memory::signature_store&, uintptr_t res) { return res; },
		"4c 89 4c 24 ? 48 89 54 24 ? 48 89 4c 24 ? 55 56 57 48 8d 6c 24"_sig,
		"MovePlayer"};

	inline static SigImpl MoveInputHandler_tick{};

	inline static SigImpl Tesesllator_vertex{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13"_sig,
		"Tessellator::vertex"};

	inline static SigImpl Tesesllator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 b9 ? ? ? ? ? 4c 8b c1 75"_sig,
		"Tessellator::color"};

	inline static SigImpl CameraViewBob{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 ? 49 8b 00"_sig,
		"CameraViewBob"};

	inline static SigImpl ItemStackBase_getHoverName{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 55 56 57 48 81 EC ? ? ? ? 48 8B F2 48 8B F9 48 89 54 24 ? 33 ED"_sig,
		"ItemStackBase::getHoverName"};


	inline static SigImpl Tessellator_vertex{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13"_sig,
		"Tessellator::vertex"};

	inline static SigImpl Tessellator_begin{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 56 48 83 ec ? 80 b9 ? ? ? ? ? 45 0f b6 d1"_sig,
		"Tessellator::begin"};

	inline static SigImpl Tessellator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 b9 ? ? ? ? ? 4c 8b c1 75"_sig,
		"Tessellator::color"};

	inline static SigImpl MeshHelpers_renderMeshImmediately{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? C6 43 ? ? F3 0F 10 1D ? ? ? ? 0F 57 D2 0F 57 C9"_sig,
		"MeshHelpers::renderMeshImmediately"};

	inline static SigImpl BaseActorRenderContext_BaseActorRenderContext{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 4C 24 ?? 57 48 83 EC ?? 49 8B F8 48 8B DA 48 8B F1 48 8D 05"_sig,
		"BaseActorRenderContext::BaseActorRenderContext"};

	inline static SigImpl ItemRenderer_renderGuiItemNew{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 45 8B E9"_sig,
		"ItemRenderer::renderGuiItemNew"};

	// TODO: this is actually BaseAttributeMap::getInstance
	inline static SigImpl BaseAttributeMap_getInstance{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? F3 0F 10 90"_sig,
		"BaseAttributeMap::getInstance"};

	inline static SigImpl UIControl_setPosition{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 41 0f 10 74 24 ? f3 0f 10 3d"_sig,
		"UIControl::setPosition"};

	inline static SigImpl MinecraftGame_getPrimaryClientInstance{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 ec ? 48 8b da 48 8b 91 ? ? ? ? 48 8b ca"_sig,
		"MinecraftGame::getPrimaryClientInstance"};

	inline static SigImpl ActorRenderDispatcher_render{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 44 0f 28 54 24 ? 4c 8d 9c 24 ? ? ? ? 41 0f 28 73"_sig,
		"ActorRenderDispatcher::render"};

	inline static SigImpl LevelRendererPlayer_renderOutlineSelection{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 83 c4 ? 41 5f 41 5e 41 5d 5e 5d c3 0f b6 84 24"_sig,
		"LevelRendererPlayer::renderOutlineSelection"};
	
	inline static SigImpl Dimension_getTimeOfDay{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 0f 28 c8 0f 57 c0 f3 0f 59 0d ? ? ? ? f3 0f 58 c9"_sig,
		"Dimension::getTimeOfDay"};
	
	inline static SigImpl Weather_tick{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 8f ? ? ? ? e8 ? ? ? ? 48 8b 8f ? ? ? ? 48 8b 01 48 8b 80"_sig,
		"Weather::tick"};
	
	inline static SigImpl Dimension_getSkyColor{[](memory::signature_store&, uintptr_t res) { return res; },
		"41 0F 10 08 48 8B C2 0F 28 D3"_sig,
		"Dimension::getSkyColor"};

	inline static SigImpl ItemStackBase_getDamageValue{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 57 48 83 ec ? 48 8b da 48 85 d2"_sig,
		"ItemStackBase::getDamageValue" };

	inline static SigImpl MinecraftPackets_createPacket{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ? 45 33 C0 48 8B D9 81 FA"_sig,
		"MinecraftPackets::createPacket" };

	inline static SigImpl GameMode_attack{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 48 89 74 24 ? 55 57 41 56 48 8d 6c 24 ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 48 8b f2 48 8b f9 45 33 f6 4c 89 75 ? 48 8d 4d ? e8 ? ? ? ? 90 c6 45"_sig,
		"GameMode::attack" };

	inline static SigImpl GuiData__addMessage{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 55 56 57 41 54 41 56 41 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 45 8B F8 48 8B F2"_sig,
		"GuiData::_addMessage(MessageContext*, UIProfanityContext)" };

	inline static SigImpl Actor_getArmor{ [](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 85 c0 0f 84 ? ? ? ? 48 8b 08 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 48 8b f8 80 78 ? ? 0f 84 ? ? ? ? 48 8b 40 ? 48 85 c0 0f 84"_sig,
		"Actor::getArmor" };

	inline static SigImpl _updatePlayer{ [](memory::signature_store&, uintptr_t res) { return res; },
		"4c 8b dc 49 89 73 ? 49 89 7b"_sig,
		"anonymous namespace::_updatePlayer" };

	inline static SigImpl GameArguments__onUri{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 89 54 24"_sig,
		"GameArguments::_onUri" };

	inline static SigImpl _bobHurt{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 55 48 8d 6c 24 ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 49 8b 00"_sig,
		"anonymous namespace::_bobHurt" };

	inline static SigImpl ItemRenderer_render{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 49 8B D8"_sig,
		"ItemRenderer::render" };

	// Inside ItemRenderer::render, ask marioCST for help if you can't figure out how to update it
	inline static SigImpl ItemPositionConst{ [](memory::signature_store&, uintptr_t res) { return res; },
		"F3 0F 10 1D ? ? ? ? F3 0F 59 C3 F3 0F 58 C7"_sig,
		"ItemPositionConst" };

	// Inside ItemRenderer::render, ask marioCST for help if you can't figure out how to update it
	inline static SigImpl glm_rotate{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ? F3 0F 59 0D ? ? ? ? 4C 8D 4C 24"_sig,
		"glm::rotate" };

	// Inside ItemRenderer::render, ask marioCST for help if you can't figure out how to update it
	inline static SigImpl glm_rotateRef{ [](memory::signature_store&, uintptr_t res) { return res; },
		"E8 ? ? ? ? 41 BD ? ? ? ? 41 0F B6 46"_sig,
		"glm::rotateRef" };

	// Inside ItemRenderer::render, ask marioCST for help if you can't figure out how to update it
	inline static SigImpl glm_translateRef{ [](memory::signature_store&, uintptr_t res) { return res; },
		"E8 ? ? ? ? 4C 8D 05 ? ? ? ? 44 0F 28 05"_sig,
		"glm::translateRef" };

	// Inside ItemRenderer::render, ask marioCST for help if you can't figure out how to update it
	inline static SigImpl glm_translateRef2{ [](memory::signature_store&, uintptr_t res) { return res; },
		"E8 ? ? ? ? C6 46 ? ? F3 0F 11 74 24 ? F3 0F 10 1D"_sig,
		"glm::translateRef2" };

	inline static SigImpl ActorCollision_isOnGround{ [](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 84 C0 49 8B 87"_sig,
		"ActorCollision::isOnGround" };
};

// after adding sigs here, add them in latite.cpp