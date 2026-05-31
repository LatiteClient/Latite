#pragma once

#ifdef LATITE_DEBUG
#define API_NAMES
#endif

#include "util/signature.h"

class SigImpl : public memory::signature_store {
public:
	explicit SigImpl() : signature_store(nullptr, this->on_resolve, std::nullopt, "") {};

	explicit SigImpl(decltype(on_resolve) onResolve, mnem::signature sig, std::string_view name) : signature_store("Minecraft.Windows.exe", onResolve, sig, name) {
	}
};

class Signatures {
public:
	struct Misc {
		inline static SigImpl Platform_GameCore{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 89 0D ? ? ? ? 48 85 C9"_sig,
			"Platform_GameCore"
		};

		inline static SigImpl mouseDevice{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"4C 8D 0D ? ? ? ? 89 CA"_sig, // MouseDevice::_instance
			"ClickMap"};

		inline static SigImpl thirdPersonNametag{ [](memory::signature_store&, uintptr_t res) { return res; },
			"74 ? 48 8B 8D ? ? ? ? 48 89 DA E8 ? ? ? ? 84 C0"_sig,
			"ThirdPersonNametag" };

	};

	struct Vtable {
		// "Client{} camera ticking systems"
		// Right below is a func allocating memory followed by a nullptr check and a memset with 0x800-ish bytes
		// The function called after the memset with the ptr that's been cleared is the MultiLevelPlayer ctor
		// 1st of 3 data LEA's
		inline static SigImpl Level{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8D 05 ? ? ? ? 48 89 07 48 8D 05 ? ? ? ? 48 89 47 ? 48 8D 05 ? ? ? ? 48 89 BD"_sig,
			"const Level::`vftable'"};
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 45 31 FF 48 83 BE"_sig,
		"LevelRenderer::renderLevel"};

	inline static SigImpl MainWindow__windowProcCallback{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? 4C 89 C0 41 89 D0 48 89 CA 48 8B 0D"_sig,
		"MainWindow::_windowProcCallback"};

	inline static SigImpl Options_getGamma{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC 38 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 08 48 8D 54 24 ? 41 B8 36 00 00 00"_sig,
		"Options::getGamma"};

	inline static SigImpl Options_getPerspective{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC 38 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 08 48 8D 54 24 ? 41 B8 03 00 00 00"_sig,
		"Options::getPerspective"};

	inline static SigImpl Options_getHideHand{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC 38 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 08 48 8D 54 24 ? 41 B8 AA 01 00 00"_sig, // Will probably die every update from now on, but a good sig would be thousands of bytes long
		"Options::getHideHand"};

	inline static SigImpl ClientInstance_grabCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 48 83 EC ? 48 89 CE 48 8B 01 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8E ? ? ? ? 48 8B 01 48 8B 80 ? ? ? ? 48 8B 15 ? ? ? ? 48 83 C4 ? 5E 48 FF E2 90 48 83 C4 ? 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 56 48 83 EC"_sig,
		"ClientInstance::grabCursor"};

	inline static SigImpl ClientInstance_releaseCursor{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 48 83 EC ? 48 89 CE 48 8B 01 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8E ? ? ? ? 48 8B 01 48 8B 80 ? ? ? ? 48 8B 15 ? ? ? ? 48 83 C4 ? 5E 48 FF E2 90 48 83 C4 ? 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 56 53"_sig,
		"ClientInstance::releaseCursor"};

	inline static SigImpl MultiPlayerLevel__subTick{ [](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 56 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 48 89 CE E8 ? ? ? ? 48 8B 06"_sig,
		"MultiPlayerLevel::_subTick" };

	inline static SigImpl ChatScreenController_sendChatMessage{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 48 89 D7 48 89 CE 48 83 79"_sig,
		"ClientInstanceScreenModel::sendChatMessage"};

	inline static SigImpl MinecraftGame_onDeviceLost{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 CF 48 8D B1 ? ? ? ? 48 8B 81"_sig,
		"MinecraftGame::onDeviceLost"};

	inline static SigImpl GameCore_handleMouseInput{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 4C 89 45 ? 4C 8B 21"_sig,
		"GameCore::handleMouseInput"};

	inline static SigImpl RenderController_getOverlayColor{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 44 0F 29 45 ? 4C 89 F1"_sig,
		"RenderController::getOverlayColor"};

	inline static SigImpl ScreenView_setupAndRender{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 48 89 F9 48 89 F2 E8 ? ? ? ? 48 8D 4D ? E8 ? ? ? ? 48 8D 4D ? E8 ? ? ? ? F2 0F 10 45"_sig,
		"ScreenView::setupAndRender"};

	inline static SigImpl KeyMap{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8D 3D ? ? ? ? C7 04 B7"_sig,
		"KeyMap"};

	inline static SigImpl MinecraftGame__update{[](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 48 8B 8E ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 48 8B B6"_sig,
		"MinecraftGame::_update"};

	// ref: your GPU ("AMD Radeon RX 5500")
	inline static SigImpl GpuInfo{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8D 0D ? ? ? ? 4C 8D 44 24 ? BA ? ? ? ? 41 B9 ? ? ? ? E8 ? ? ? ? 48 C7 05"_sig,
		"GpuInfo"};

	// ref: RakPeer vtable; 88 51 12 c3 -> xref -> third func above it
	inline static SigImpl RakPeer_GetAveragePing{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 84 24 ? ? ? ? 48 8B 02 48 3B 05 ? ? ? ? 0F 85"_sig,
		"RakPeer::GetAveragePing"};

	inline static SigImpl LocalPlayer_applyTurnDelta{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 56 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 7D ? 0F 29 75 ? 48 C7 45 ? ? ? ? ? 48 89 D7 48 89 CE"_sig,
		"LocalPlayer::applyTurnDelta"};

	// see what accesses things in moveinputhandler
	inline static SigImpl ClientInputUpdateSystem_tickBaseInput{[](memory::signature_store&, uintptr_t res) { return res; },
		"41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 0F 29 7C 24 ? 0F 29 74 24 ? 4C 89 4C 24 ? 4C 89 44 24"_sig,
		"ClientInputUpdateSystem::tickBaseInput"};

	inline static SigImpl ItemStackBase_getHoverName{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 56 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 48 89 D6 48 8D 7D ? 48 89 FA E8 ? ? ? ? 48 89 F9"_sig,
		"ItemStackBase::getHoverName"};

	inline static SigImpl ItemStack_ItemStackBlock{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 56 48 83 EC 38 48 8D 6C 24 30 48 C7 45 00 FE FF FF FF 48 89 CE E8 35 C3 FF FF 48 8D 05 ? ? ? ? 48 89 06 48 89 75 F8 48 8D 8E 80 00 00 00 E8 ? ? ? ? 48 8B 45 F8 48 83 C4 38 5E 5D C3"_sig,
		"ItemStack::ItemStack(Block const&, int, CompoundTag const*)"};

	inline static SigImpl ItemStackBase_destructor{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 57 48 83 EC 28 48 89 CE 48 8D 05 ? ? ? ? 48 89 01 48 8B 49 78 48 85 C9 74 11 48 8B 01 48 8B 00 BA 01 00 00 00 FF 15"_sig,
		"ItemStackBase::~ItemStackBase"};


	inline static SigImpl Tessellator_vertex{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 57 48 83 EC ? 0F 29 7C 24 ? 0F 29 74 24 ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 8B 81"_sig,
		"Tessellator::vertex"};

	inline static SigImpl Tessellator_begin{[](memory::signature_store&, uintptr_t res) { return res; },
		"41 56 56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 80 B9 ? ? ? ? ? 0F 85"_sig,
		"Tessellator::begin"};

	inline static SigImpl Tessellator_color{[](memory::signature_store&, uintptr_t res) { return res; },
		"80 B9 ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 05 ? ? ? ? F3 0F 10 0A"_sig,
		"Tessellator::color"};

	inline static SigImpl MeshHelpers_renderMeshImmediately{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 4D 89 CE 4C 89 C7 48 89 D6 48 89 CB 48 89 D1 E8 ? ? ? ? 84 C0 75"_sig,
		"MeshHelpers::renderMeshImmediately"};

	inline static SigImpl BaseActorRenderContext_BaseActorRenderContext{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 4C 89 C6 48 89 D7 48 89 CB 48 8D 05 ? ? ? ? 48 89 01 0F 57 C0"_sig,
		"BaseActorRenderContext::BaseActorRenderContext"};

	inline static SigImpl ItemRenderer_renderGuiItemNew{ [](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 4C 8D 45 ? 4C 8D 8D ? ? ? ? 48 89 F2 E8 ? ? ? ? 80 BF"_sig,
		"ItemRenderer::renderGuiItemNew"};

	inline static SigImpl BlockGraphics_getForBlock{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 48 83 EC ? 48 89 CE E8 ? ? ? ? 48 85 C0 74 ? 48 83 C4 ? 5E C3 48 89 F1"_sig,
		"BlockGraphics::getForBlock"};

	inline static SigImpl BlockGraphics_getTexture{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 41 ? 4C 8B 49 ? 49 29 C1 74 ? 49 C1 F9 ? 49 BA ? ? ? ? ? ? ? ? 4D 0F AF D1 49 FF CA 49 39 D2 4C 0F 43 D2 4F 8D 0C 52 49 C1 E1 ? 4A 8B 54 08 ? 4A 8B 44 08 ? 48 29 D0 48 C1 E8 ? 69 C0 ? ? ? ? 85 C0 7E ? 31 C9 41 39 C0 41 0F 42 C8 48 8D 04 49 48 8D 04 C2 EB"_sig,
		"BlockGraphics::getTexture"};

	inline static SigImpl BlockGraphics_getTextureAtPos{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 57 48 83 EC ? 48 89 D6 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 41 ? 48 8B 51"_sig,
		"BlockGraphics::getTexture(BlockPos)"};

	inline static SigImpl UIControl_updateCachedPosition{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 56 57 48 83 EC ? 48 8D 6C 24 ? 0F 29 75 ? 48 C7 45 ? ? ? ? ? 48 89 CE 0F 57 F6 0F 29 75"_sig,
		"UIControl::updateCachedPosition"};

	inline static SigImpl ActorRenderDispatcher_render{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 75 ? 48 C7 45 ? ? ? ? ? 4D 89 CE 4C 89 C6 48 89 D7"_sig,
		"ActorRenderDispatcher::render"}; // "No renderer found - have you set the entity's description:identifier correctly?"

	inline static SigImpl LevelRendererPlayer_renderOutlineSelection{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 45 ? ? ? ? ? 4D 89 CE 4D 89 C7"_sig,
		"LevelRendererPlayer::renderOutlineSelection"};

	inline static SigImpl Dimension_getTimeOfDay{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 EC ? 0F 29 7C 24 ? 0F 29 74 24 ? 48 63 C2"_sig,
		"Dimension::getTimeOfDay"};

	inline static SigImpl Dimension_tick{[](memory::signature_store&, uintptr_t res) { return res; },
		"55 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 48 89 CE 48 8B 89 ? ? ? ? E8 ? ? ? ? 48 8B 8E"_sig,
		"Dimension::tick"};

	inline static SigImpl Dimension_getSkyColor{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 D0 41 0F 10 00 0F 11 02 F3 0F 10 05"_sig,
		"Dimension::getSkyColor"};

	inline static SigImpl ItemStackBase_getDamageValue{ [](memory::signature_store&, uintptr_t res) { return res; },
		"56 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 41 ? 48 85 C0 74 ? 48 83 38"_sig,
		"ItemStackBase::getDamageValue" };

	inline static SigImpl MinecraftPackets_createPacket{ [](memory::signature_store&, uintptr_t res) { return res; },
		"56 48 83 EC ? 48 89 CE 81 FA"_sig,
		"MinecraftPackets::createPacket" };

	inline static SigImpl Actor_attack{ [](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 4D 89 CD 4C 89 C3"_sig,
		"Actor::attack" };

	inline static SigImpl GuiData__addMessage{ [](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 55 41 54 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 44 89 C7 48 89 CE 48 81 C1"_sig,
		"GuiData::_addMessage(MessageContext*, UIProfanityContext)" };

	inline static SigImpl Actor_setNameTag{ [](memory::signature_store&, uintptr_t res) { return res; },
		"55 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 48 89 D7 48 89 CE 48 81 C1 ? ? ? ? 48 8D 55"_sig,
		"Actor::setNameTag" };

	inline static SigImpl _updatePlayer{ [](memory::signature_store&, uintptr_t res) { return res; },
		"41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 0F 29 74 24 ? 4C 89 C6 48 89 C8"_sig,
		"UpdatePlayerFromCameraSystemUtil::_updatePlayer" };

	// showHowToPlayScreen
	inline static SigImpl GameArguments__onUri{ [](memory::signature_store&, uintptr_t res) { return res; },
		"55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 49 89 D4 49 89 CD"_sig,
		"GameArguments::_onUri" };

	inline static SigImpl RenderMaterialGroup__common{ [](memory::signature_store& store, uintptr_t) { return store.deref(3); },
		"48 8D 15 ? ? ? ? 48 8D 4D ? 4D 89 F8 E8"_sig,
		"mce::RenderMaterialGroup::common" };

	inline static SigImpl GuiData_displayClientMessage{ [](memory::signature_store&, uintptr_t res) { return res; },
		"55 56 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 44 88 CB"_sig,
		"GuiData::displayClientMessage" };

	inline static SigImpl BaseActorRenderer_renderText{ [](memory::signature_store& store, uintptr_t) { return store.deref(1); },
		"E8 ? ? ? ? 49 81 C7 ? ? ? ? 4D 39 E7 75 ? 48 83 C4"_sig,
		"BaseActorRenderer_renderText" };

	inline static SigImpl AppPlatformGDK_releaseMouse{[](memory::signature_store&, uintptr_t res) { return res; },
		"56 57 48 83 EC ? 48 89 CE B9 ? ? ? ? FF 15"_sig,
		"AppPlatformGDK_releaseMouse" };
};

// after adding sigs here, add them in latite.cpp
