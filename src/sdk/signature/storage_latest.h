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
			"F3 0F 58 BB ? ? ? ? 48 8B 8B"_sig,
			"LevelRendererPlayer->origin"};

	};
	struct Misc {
		inline static SigImpl clientInstance{};

		inline static SigImpl minecraftGamePointer {[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"4C 89 35 ? ? ? ? 4C 89 35 ? ? ? ? 48 8B 1D"_sig,
			"MinecraftGame"};
		inline static SigImpl clickMap{[](memory::signature_store& store, uintptr_t) { return store.deref(2); },
			"89 0D ? ? ? ? 41 B7"_sig, // MouseDevice::_instance
			"ClickMap"};

		inline static SigImpl uiColorMaterial{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8B 05 ? ? ? ? 4C 8B AD"_sig,
			"UIFillColorMaterial"};
		inline static SigImpl thirdPersonNametag{ [](memory::signature_store&, uintptr_t res) { return res; },
			"0F 84 ? ? ? ? 49 8B 04 ? 49 8B CC 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 0F 85"_sig,
			"ThirdPersonNametag" };
		
	};

	struct Components {
		inline static SigImpl moveInputComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 41 48 4C 8B D1 48 8B 41 50 4C 8B 49 68 49 2B C0 8B 12 48 C1 F8 03 48 FF C8 25 2E CD 8B 46"_sig,
			"MoveInputComponent::try_get"};

		inline static SigImpl runtimeIDComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 41 48 4C 8B D1 48 8B 41 50 4C 8B 49 68 49 2B C0 8B 12 48 C1 F8 03 48 FF C8 25 14 14 A1 3C"_sig,
			"ActorRuntimeIDComponent"};
		

		inline static SigImpl actorTypeComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 41 48 4C 8B D1 48 8B 41 50 4C 8B 49 68 49 2B C0 8B 12 48 C1 F8 03 48 FF C8 25 14 AD F3 51"_sig,
			"ActorTypeComponent"}; //51F3AD14

		inline static SigImpl attributesComponent{[](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 51 08 41 BB 44 94 B2 B6"_sig,
			"AttributesComponent"};

		inline static SigImpl actorEquipmentPersistentComponent{ [](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 41 48 4C 8B D1 48 8B 41 50 4C 8B 49 68 49 2B C0 8B 12 48 C1 F8 03 48 FF C8 25 36 48 C4 71"_sig,
			"struct ActorEquipmentComponent" };

		//76 59 47 33
		inline static SigImpl actorDataFlagsComponent{ [](memory::signature_store&, uintptr_t res) { return res; },
			// last 4 bytes is the hash of the component
			"4C 8B 41 48 4C 8B D1 48 8B 41 50 4C 8B 49 68 49 2B C0 8B 12 48 C1 F8 03 48 FF C8 25 76 59 47 33"_sig,
			"struct ActorDataFlagsComponent"};
	};

	struct Vtable {
		inline static SigImpl TextPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 01 33 d2 48 89 51 ? 0f 57 c0 48 89 51 ? 48 8b c1 89 51"_sig,
			"const TextPacket::`vftable'"};
		inline static SigImpl CommandRequestPacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8D 05 ? ? ? ? 48 89 45 ? 49 8B D5 48 8D 4D ? E8 ? ? ? ? 90 49 8B 4D"_sig,
			"const CommandRequestPacket::`vftable'"};

		// "Client%d camera ticking system"
		// 1st of 3 data LEA's
		inline static SigImpl Level{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8D 05 ? ? ? ? 48 89 07 48 8D 05 ? ? ? ? 48 89 47 ? 48 8D 05 ? ? ? ? 48 89 47 ? 4C 8D 35"_sig,
			"const Level::`vftable'"};
		inline static SigImpl SetTitlePacket{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8d 05 ? ? ? ? 48 89 01 89 51 ? 48 83 c1 ? 0f 57 c0 0f 11 01 48 89 79"_sig,
			"const SetTitlePacket::`vftable'"};

		//

		//48 8d 05 ? ? ? ? 48 89 07 48 8d 05 ? ? ? ? 48 89 47 ? 48 8d 05 ? ? ? ? 48 89 47 ? 33 c0
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 ? ? 44 0F 29 ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 89 44 24 ? 4C 8B EA"_sig,
		"LevelRenderer::renderLevel"};

	inline static SigImpl Keyboard_feed{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? ? ? C1 4C 8D 05"_sig,
		"Keyboard::feed"};

	// The signature is big but it hasn't died in a while soo
	inline static SigImpl Options_getGamma{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? 80 B9 ? ? ? ? ? 48 8D 54 24 ? 48 8B 01 48 8B 40 ? 74 ? 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 74 ? 48 8B 42 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 83 C4 ? C3 F3 0F 10 42 ? 48 83 C4 ? C3 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 75 ? E8 ? ? ? ? CC E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24"_sig,
		"Options::getGamma"};

	inline static SigImpl Options_getPerspective{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? 48 8B 01 48 8D 54 ? ? 41 B8 03 00 00 00"_sig,
		"Options::getPerspective"};

	inline static SigImpl Options_getHideHand{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 48 8b 40 ? 74 ? 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 0f b6 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc 48 89 5c 24"_sig,
		"Options::getHideHand"};

	inline static SigImpl Options_getSensitivity{};

	inline static SigImpl ClientInstance_grabCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ? 48 8B 01 48 8B D9 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8B ? ? ? ? 48 8B 01 48 8B 80 ? ? ? ? 48 83 C4 ? 5B 48 FF 25 ? ? ? ? 48 83 C4 ? 5B C3 40 53"_sig,
		"ClientInstance::grabCursor"};

	inline static SigImpl ClientInstance_releaseCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ? 48 8B 01 48 8B D9 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8B ? ? ? ? 48 8B 01 48 8B 80 ? ? ? ? 48 83 C4 ? 5B 48 FF 25 ? ? ? ? 48 83 C4 ? 5B C3 48 89 5C 24"_sig,
		"ClientInstance::releaseCurosr"};

	inline static SigImpl Level_tick{ [](memory::signature_store& stor, uintptr_t res) { return stor.deref(1); },
		"e8 ? ? ? ? 48 8b 4b ? 48 85 c9 74 ? 48 8b 41 ? 48 83 c1 ? 48 8b 40"_sig,
		"Level::tick" };

	// callsites

	inline static SigImpl ChatScreenController_sendChatMessage{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 3C ? 75 ? 48 8B 8F"_sig,
		"ClientInstanceScreenModel::sendChatMessage"};

	inline static SigImpl MinecraftGame_onDeviceLost{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 8B F9 C7 44 24"_sig,
		"MinecraftGame::onDeviceLost"};

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
		"48 89 5c 24 ? 48 89 74 24 ? 48 89 7c 24 ? 55 41 54 41 55 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? ? 8b ? 45 33 f6"_sig,
		"MinecraftGame::onAppSuspended"}; // "OnAppSuspend" "SuspendAudio" "CancelJoinGameTelemetry"

	inline static SigImpl RenderController_getOverlayColor{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 0F 28 05 ? ? ? ? 0F 11 85 ? ? ? ? 4C 8D 8D"_sig,
		"RenderController::getOverlayColor"};

	inline static SigImpl ScreenView_setupAndRender{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 8b 44 24 ? 48 8d 4c 24 ? 48 8b 80"_sig,
		"ScreenView::setupAndRender"};

	inline static SigImpl KeyMap{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"4c 8d 05 ? ? ? ? 89 54 24 ? 88 4c 24"_sig,
		"KeyMap"};

	inline static SigImpl MinecraftGame__update{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 48 8B 8B ? ? ? ? BA ? ? ? ? 48 83 C4"_sig,
		"MinecraftGame::_update"};

	// ref: your GPU ("AMD Radeon RX 5500")
	inline static SigImpl GpuInfo{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8d 15 ? ? ? ? 48 c7 c7 ? ? ? ? 4c 8b c7 49 ff c0 42 80 3c 02 ? 75 ? 48 8d 4c 24"_sig,
		"GpuInfo"};

	// ref: RakPeer vtable
	inline static SigImpl RakPeer_GetAveragePing{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 81 EC ? ? ? ? 0F 10 4A ? 4C 8B 1A 4C 3B 1D ? ? ? ? 0F 29 70 ? 0F 29 78 ? 0F 10 7A ? 0F 11 4C 24 ? 74 ? 44 8B 49"_sig,
		"RakPeer::GetAveragePing"};

	inline static SigImpl LocalPlayer_applyTurnDelta{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 89 58 ? 48 89 70 ? 55 57 41 54 41 56 41 57 48 8D 68 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 58"_sig,
		"LocalPlayer::applyTurnDelta"};

	// see what accesses things in moveinputhandler
	inline static SigImpl ClientInputUpdateSystem_tickBaseInput{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 7D"_sig,
		"ClientInputUpdateSystem::tickBaseInput"};

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
		"48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B FA 48 8B F1 48 89 54 24 ? 33 DB"_sig,
		"ItemStackBase::getHoverName"};


	inline static SigImpl Tessellator_vertex{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13"_sig,
		"Tessellator::vertex"};

	inline static SigImpl Tessellator_begin{[](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 55 48 83 EC ? 80 B9 ? ? ? ? ? 44 0F B6 D2"_sig,
		"Tessellator::begin"};

	inline static SigImpl Tessellator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 b9 ? ? ? ? ? 4c 8b c1 75"_sig,
		"Tessellator::color"};

	inline static SigImpl MeshHelpers_renderMeshImmediately{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 41 C6 44 24 ? ? F3 0F 10 1D"_sig,
		"MeshHelpers::renderMeshImmediately"};

	inline static SigImpl BaseActorRenderContext_BaseActorRenderContext{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 4C 24 ? 57 48 83 EC ? 49 8B F8 48 8B DA 48 8B F1 48 8D 05"_sig,
		"BaseActorRenderContext::BaseActorRenderContext"};

	inline static SigImpl ItemRenderer_renderGuiItemNew{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 89 4D ? 49 8B F8"_sig,
		"ItemRenderer::renderGuiItemNew"};

	inline static SigImpl BaseAttributeMap_getInstance{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 48 89 44 24 ? 48 8D 55 ? 48 8B 4C 24"_sig,
		"BaseAttributeMap::getInstance"};

	inline static SigImpl UIControl_getPosition{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? F3 0F 10 7E"_sig,
		"UIControl::getPosition"};

	inline static SigImpl MinecraftGame_getPrimaryClientInstance{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B 91 ? ? ? ? 48 8B CA 48 8B 42 ? 80 78 ? ? 75 ? 48 8B C8 48 8B 00 80 78 ? ? 74 ? 80 79 ? ? 75 ? 80 79 ? ? 76 ? 48 8B CA 48 3B CA"_sig,
		"MinecraftGame::getPrimaryClientInstance"};

	inline static SigImpl ActorRenderDispatcher_render{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 4C 89 4C 24 ? 49 8B F8"_sig,
		"ActorRenderDispatcher::render"}; // "No renderer found - have you set the entity's description:identifier correctly?"

	inline static SigImpl LevelRendererPlayer_renderOutlineSelection{[](memory::signature_store& storage, uintptr_t) { return storage.deref(1); },
		"E8 ? ? ? ? 90 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8B 86 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 80 38 ? 0F 84 ? ? ? ? 48 8B 86 ? ? ? ? 48 85 C0 74 ? F0 FF 40 ? 48 8B 86"_sig,
		"LevelRendererPlayer::renderOutlineSelection"};
	
	inline static SigImpl Dimension_getTimeOfDay{[](memory::signature_store&, uintptr_t res) { return res; },
		"44 8B C2 B8 ? ? ? ? F7 EA"_sig,
		"Dimension::getTimeOfDay"};
	
	inline static SigImpl Dimension_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 48 8B F1 48 8B 99"_sig,
		"Dimension::tick"};
	
	inline static SigImpl Dimension_getSkyColor{[](memory::signature_store&, uintptr_t res) { return res; },
		"41 0F 10 08 48 8B C2 0F 28 D3"_sig,
		"Dimension::getSkyColor"};

	inline static SigImpl ItemStackBase_getDamageValue{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 48 83 EC ? 48 8B 51 ? 33 DB 48 85 D2"_sig,
		"ItemStackBase::getDamageValue" };

	inline static SigImpl MinecraftPackets_createPacket{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B D9 48 89 4C 24 ? 33 F6"_sig,
		"MinecraftPackets::createPacket" };

	inline static SigImpl Actor_attack{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 41 0F B6 F1"_sig,
		"Actor::attack" };

	inline static SigImpl GuiData__addMessage{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 53 55 56 57 41 54 41 56 41 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 45 8B F8"_sig,
		"GuiData::_addMessage(MessageContext*, UIProfanityContext)" };

	inline static SigImpl Actor_getArmor{ [](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"e8 ? ? ? ? 48 85 c0 0f 84 ? ? ? ? 48 8b 08 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 48 8b f8 80 78 ? ? 0f 84 ? ? ? ? 48 8b 40 ? 48 85 c0 0f 84"_sig,
		"Actor::getArmor" };

	inline static SigImpl _updatePlayer{ [](memory::signature_store&, uintptr_t res) { return res; },
		"4C 8B DC 49 89 7B"_sig,
		"UpdatePlayerFromCameraSystemUtil::_updatePlayer" };

	inline static SigImpl GameArguments__onUri{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B F2 48 8B F1 45 33 FF"_sig,
		"GameArguments::_onUri" };

	inline static SigImpl _bobHurt{ [](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5C 24 ? 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B 00"_sig,
		"anonymous namespace::_bobHurt" };

	inline static SigImpl RenderMaterialGroup__common{ [](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8B 05 ? ? ? ? 48 8D 55 ? 48 8D 0D ? ? ? ? 48 8B 40 ? FF 15 ? ? ? ? 48 8B D8 48 8B 50 ? 48 85 D2 0F 84 ? ? ? ? 8B 42 08 85 C0 0F 84 ? ? ? ? 0F 1F 00"_sig,
		"mce::RenderMaterialGroup::common" };

	inline static SigImpl GuiData_displayClientMessage{ [](memory::signature_store&, uintptr_t res) { return res; },
		"40 55 53 56 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 41 0F B6 ? 49 8B D8"_sig,
		"GuiData::displayClientMessage" };
};

// after adding sigs here, add them in latite.cpp