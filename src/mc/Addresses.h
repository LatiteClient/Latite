#pragma once

#ifdef LATITE_DEBUG
#define API_NAMES
#endif

#include "util/signature.h"

class SigImpl : public memory::signature_store {
public:
    explicit SigImpl()
        : signature_store(nullptr, this->on_resolve, std::nullopt, "") {};

    explicit SigImpl(decltype(on_resolve) onResolve, mnem::signature sig, std::string_view name)
        : signature_store("Minecraft.Windows.exe", onResolve, sig, name) {}
};

class Signatures {
public:
    struct Misc {
        inline static SigImpl Platform_GameCore { [](memory::signature_store& store, uintptr_t) {
                                                     return store.deref(3);
                                                 },
                                                  "4C 89 3D ? ? ? ? 4D 85 FF"_sig, "Platform_GameCore" };

        inline static SigImpl mouseDevice { [](memory::signature_store& store, uintptr_t) {
                                               return store.deref(2);
                                           },
                                            "89 15 ? ? ? ? C7 47"_sig, "MouseDevice::_instance" };

        inline static SigImpl thirdPersonNametag {
            [](memory::signature_store&, uintptr_t res) {
                return res;
            },
            "74 ? 48 8B 03 48 8B 80 ? ? ? ? 48 89 D9 FF 15 ? ? ? ? 84 C0 75 ? 48 8B 4B"_sig, "ThirdPersonNametag"
        };
    };

    struct Vtable {
        // "Client{} camera ticking systems"
        // Right below is a func allocating memory followed by a nullptr check and a memset with 0x800-ish bytes
        // The function called after the memset with the ptr that's been cleared is the MultiLevelPlayer ctor
        // 1st of 3 data LEA's
        inline static SigImpl Level {
            [](memory::signature_store& store, uintptr_t) {
                return store.deref(3);
            },
            "48 8D 05 ? ? ? ? 48 89 07 48 8D 05 ? ? ? ? 48 89 47 ? 48 8D 05 ? ? ? ? 48 89 BD"_sig,
            "const Level::`vftable'"
        };
    };

    inline static SigImpl LevelRenderer_renderLevel { [](memory::signature_store& store, uintptr_t) {
                                                         return store.deref(1);
                                                     },
                                                      "E8 ? ? ? ? 45 31 E4 48 83 BE"_sig,
                                                      "LevelRenderer::renderLevel" };

    inline static SigImpl MainWindow__windowProcCallback {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 89 D6 4C 8B 3D"_sig,
        "MainWindow::_windowProcCallback"
    };

    inline static SigImpl Options_getGamma {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "48 83 EC 38 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 08 48 8D 54 24 ? 41 B8 35 00 00 00"_sig,
        "Options::getGamma"
    };

    inline static SigImpl Options_getPerspective {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "48 83 EC 38 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 08 48 8D 54 24 ? 41 B8 03 00 00 00"_sig,
        "Options::getPerspective"
    };

    inline static SigImpl Options_getHideHand {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "48 83 EC 38 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 01 48 8B 40 08 48 8D 54 24 ? 41 B8 AB 01 00 00"_sig, // Will probably die every update from now on, but a good sig would be thousands of bytes long
        "Options::getHideHand"
    };

    inline static SigImpl ClientInstance_grabCursor {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "56 48 83 EC ? 48 89 CE 48 8B 01 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8E ? ? ? ? 48 8B 01 48 8B 80 ? ? ? ? 48 8B 15 ? ? ? ? 48 83 C4 ? 5E 48 FF E2 90 48 83 C4 ? 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 56 48 83 EC"_sig,
        "ClientInstance::grabCursor"
    };

    inline static SigImpl ClientInstance_releaseCursor {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "56 48 83 EC ? 48 89 CE 48 8B 01 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B 8E ? ? ? ? 48 8B 01 48 8B 80 ? ? ? ? 48 8B 15 ? ? ? ? 48 83 C4 ? 5E 48 FF E2 90 48 83 C4 ? 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC 56 53"_sig,
        "ClientInstance::releaseCursor"
    };

    inline static SigImpl MultiPlayerLevel__subTick {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 44 0F 29 6D"_sig, "MultiPlayerLevel::_subTick"
    };

    inline static SigImpl ChatScreenController_sendChatMessage {
        [](memory::signature_store& storage, uintptr_t) {
            return storage.deref(1);
        },
        "E8 ? ? ? ? 3C ? 75 ? 48 8B 8E ? ? ? ? 48 C7 45 ? ? ? ? ? 48 8B 01 48 8B 40 ? 48 8D 55 ? FF 15 ? ? ? ? 48 8B 4D"_sig,
        "ClientInstanceScreenModel::sendChatMessage"
    };

    inline static SigImpl MinecraftGame_onDeviceLost {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 CF 48 8D B1 ? ? ? ? 48 8B 81"_sig,
        "MinecraftGame::onDeviceLost"
    };

    inline static SigImpl GameCore_handleMouseInput {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 44 0F 29 BD ? ? ? ? 44 0F 29 B5 ? ? ? ? 44 0F 29 AD ? ? ? ? 44 0F 29 A5 ? ? ? ? 44 0F 29 9D ? ? ? ? 44 0F 29 95 ? ? ? ? 44 0F 29 8D ? ? ? ? 44 0F 29 85 ? ? ? ? 0F 29 BD ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 48 89 CE 8B 05"_sig,
        "GameCore::handleMouseInput"
    };

    inline static SigImpl RenderController_getOverlayColor { [](memory::signature_store& store, uintptr_t) {
                                                                return store.deref(1);
                                                            },
                                                             "E8 ? ? ? ? 0F 11 7D ? 4C 89 F9"_sig,
                                                             "RenderController::getOverlayColor" };

    inline static SigImpl ScreenView_setupAndRender {
        [](memory::signature_store& store, uintptr_t) {
            return store.deref(1);
        },
        "E8 ? ? ? ? 48 8B 4B ? 48 85 C9 74 ? 48 8B 01 48 8B 40 ? 48 89 FA FF 15 ? ? ? ? 48 8D 4D"_sig,
        "ScreenView::setupAndRender"
    };

    inline static SigImpl KeyMap { [](memory::signature_store& store, uintptr_t) {
                                      return store.deref(3);
                                  },
                                   "48 8D 3D ? ? ? ? C7 04 B7"_sig, "KeyMap" };

    inline static SigImpl MinecraftGame__update { [](memory::signature_store& store, uintptr_t) {
                                                     return store.deref(1);
                                                 },
                                                  "E8 ? ? ? ? 48 8B 8F ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 48 8B 9F"_sig,
                                                  "MinecraftGame::_update" };

    // ref: your GPU ("AMD Radeon RX 5500")
    inline static SigImpl GpuInfo { [](memory::signature_store& store, uintptr_t) {
                                       return store.deref(3);
                                   },
                                    "48 8D 0D ? ? ? ? 4C 8D 44 24 ? BA ? ? ? ? 41 B9 ? ? ? ? E8 ? ? ? ? 48 C7 05"_sig,
                                    "GpuInfo" };

    // ref: RakPeer vtable; 88 51 12 c3 -> xref -> third func above it
    inline static SigImpl RakPeer_GetAveragePing {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 84 24 ? ? ? ? 4C 8B 02 4C 3B 05 ? ? ? ? 0F 85 ? ? ? ? 0F B7 42 ? 44 0F B7 82 ? ? ? ? 44 0F B7 8A ? ? ? ? 66 89 44 24 ? 0F 10 42 ? 0F 10 4A ? 0F 10 52 ? 0F 10 5A ? 0F 11 44 24 ? 0F 11 4C 24 ? 0F 11 54 24 ? 0F 11 5C 24 ? 0F 10 42 ? 0F 11 44 24 ? 0F 10 42 ? 0F 11 44 24 ? 0F 10 42 ? 0F 11 84 24 ? ? ? ? 0F 10 82 ? ? ? ? 0F 11 84 24 ? ? ? ? 66 44 89 8C 24 ? ? ? ? 66 44 89 84 24 ? ? ? ? 48 8D 54 24 ? 45 31 C0 45 31 C9 E8 ? ? ? ? BA"_sig,
        "RakPeer::GetAveragePing"
    };

    inline static SigImpl LocalPlayer_applyTurnDelta {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 56 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 44 0F 29 5D ? 44 0F 29 55 ? 44 0F 29 4D ? 44 0F 29 45 ? 0F 29 7D ? 0F 29 75 ? 48 C7 45 ? ? ? ? ? 48 89 D7 48 89 CE 48 8B 89"_sig,
        "LocalPlayer::applyTurnDelta"
    };

    // see what accesses things in moveinputhandler
    inline static SigImpl ClientInputUpdateSystemInternal_tickUpdateClientInput {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 48 8B 84 24"_sig,
        "ClientInputUpdateSystemInternal::tickUpdateClientInput"
    };

    inline static SigImpl ItemStackBase_getHoverName {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 D6 48 8D 55"_sig,
        "ItemStackBase::getHoverName"
    };

    inline static SigImpl I18n_getI18n { [](memory::signature_store& store, uintptr_t) {
                                            return store.deref(3);
                                        },
                                         "48 8D 0D ? ? ? ? 48 8D B5 ? ? ? ? 48 89 F2 4C 8D 85"_sig, "I18n::sI18n" };

    inline static SigImpl ItemStack_ItemStackBlock {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 56 57 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 4C 89 CE 48 8D 05"_sig,
        "ItemStackBase::ItemStackBase(Block const&, int, CompoundTag const*)"
    };

    inline static SigImpl ItemStackVtable { [](memory::signature_store& store, uintptr_t) {
                                               return store.deref(3);
                                           },
                                            "48 8D 1D ? ? ? ? 48 89 5D ? 48 89 F9"_sig, "ItemStackVtable" };

    inline static SigImpl ItemStackBase_destructor {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "56 57 48 83 EC 28 48 89 CE 48 8D 05 ? ? ? ? 48 89 01 48 8B 49 78 48 85 C9 74 11 48 8B 01 48 8B 00 BA 01 00 00 00 FF 15"_sig,
        "ItemStackBase::~ItemStackBase"
    };

    inline static SigImpl Tessellator_vertex {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 44 0F 29 8C 24 ? ? ? ? 44 0F 29 44 24 ? 0F 29 7C 24 ? 0F 29 74 24 ? 8B 81"_sig,
        "Tessellator::vertex"
    };

    inline static SigImpl Tessellator_begin {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 80 B9 ? ? ? ? ? 0F 85 ? ? ? ? 80 B9"_sig,
        "Tessellator::begin"
    };

    inline static SigImpl Tessellator_color { [](memory::signature_store&, uintptr_t res) {
                                                 return res;
                                             },
                                              "80 B9 ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 05 ? ? ? ? F3 0F 10 0A"_sig,
                                              "Tessellator::color" };

    inline static SigImpl MeshHelpers_renderMeshImmediately {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 80 BA ? ? ? ? ? 0F 85 ? ? ? ? 4C 89 CF"_sig,
        "MeshHelpers::renderMeshImmediately"
    };

    inline static SigImpl BaseActorRenderContext_BaseActorRenderContext {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 56 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 4C 89 C6 48 89 D7 49 89 CE 48 8D 05 ? ? ? ? 48 89 01 0F 57 C0"_sig,
        "BaseActorRenderContext::BaseActorRenderContext"
    };

    inline static SigImpl ItemRenderer_renderGuiItemNew {
        [](memory::signature_store& store, uintptr_t) {
            return store.deref(1);
        },
        "E8 ? ? ? ? 48 8D 55 ? 4C 8D 85 ? ? ? ? 48 89 F1 E8 ? ? ? ? 80 BF"_sig, "ItemRenderer::renderGuiItemNew"
    };

    inline static SigImpl ActorRenderDispatcher_render {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 7D ? 0F 29 75 ? 48 C7 45 ? ? ? ? ? 4C 89 C6 48 89 D7 48 89 CB"_sig,
        "ActorRenderDispatcher::render"
    }; // "No renderer found - have you set the entity's description:identifier correctly?"

    inline static SigImpl MolangVariable__findOrAddVariableIndex {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 45 ? ? ? ? ? 48 85 D2 0F 84 ? ? ? ? 44 88 45"_sig,
        "MolangVariable::_findOrAddVariableIndex"
    };

    inline static SigImpl MolangVariableMap__getOrAddMolangVariable {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 66 83 FA"_sig,
        "MolangVariableMap::_getOrAddMolangVariable"
    };

    inline static SigImpl LevelRendererPlayer_renderOutlineSelection {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 44 0F 29 95 ? ? ? ? 44 0F 29 8D ? ? ? ? 44 0F 29 85 ? ? ? ? 0F 29 BD ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 4C 89 CB 4D 89 C7"_sig,
        "LevelRendererPlayer::renderOutlineSelection"
    };

    inline static SigImpl Dimension_getTimeOfDay { [](memory::signature_store&, uintptr_t res) {
                                                      return res;
                                                  },
                                                   "48 63 C2 48 69 C8 ? ? ? ? 48 89 CA 48 C1 EA ? 48 C1 F9"_sig,
                                                   "Dimension::getTimeOfDay" };

    inline static SigImpl Dimension_tick {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 BD ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 48 89 CE 48 8B 81 ? ? ? ? 48 3B 81"_sig,
        "Dimension::tick"
    };

    inline static SigImpl Dimension_getSkyColor { [](memory::signature_store&, uintptr_t res) {
                                                     return res;
                                                 },
                                                  "48 89 D0 41 0F 10 00 0F 11 02 F3 0F 10 05"_sig,
                                                  "Dimension::getSkyColor" };

    inline static SigImpl ItemStackBase_getDamageValue {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "56 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 ? 48 8B 41 ? 48 85 C0 74 ? 48 83 38"_sig,
        "ItemStackBase::getDamageValue"
    };

    inline static SigImpl MinecraftPackets_createPacket { [](memory::signature_store&, uintptr_t res) {
                                                             return res;
                                                         },
                                                          "56 48 83 EC ? 48 89 CE 81 FA"_sig,
                                                          "MinecraftPackets::createPacket" };

    inline static SigImpl Actor_attack {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 4D 89 CF 4D 89 C6 48 89 D6 48 89 CF 41 8B 80"_sig,
        "Actor::attack"
    };

    inline static SigImpl GuiData__addMessage { [](memory::signature_store& store, uintptr_t) {
                                                   return store.deref(1);
                                               },
                                                "E8 ? ? ? ? 4C 8B B6 ? ? ? ? 48 8B BE ? ? ? ? 48 89 F8"_sig,
                                                "GuiData::_addMessage(MessageContext*, UIProfanityContext)" };

    inline static SigImpl Actor_setNameTag {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 56 57 53 48 83 EC ? 48 8D 6C 24 ? 48 C7 45 ? ? ? ? ? 48 89 D7 48 89 CE 48 81 C1 ? ? ? ? 48 8D 55"_sig,
        "Actor::setNameTag"
    };

    inline static SigImpl _updatePlayer {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 44 0F 29 94 24 ? ? ? ? 44 0F 29 8C 24 ? ? ? ? 44 0F 29 84 24 ? ? ? ? 0F 29 BC 24 ? ? ? ? 0F 29 B4 24 ? ? ? ? 4C 89 C6"_sig,
        "UpdatePlayerFromCameraSystemUtil::_updatePlayer"
    };

    // showHowToPlayScreen
    inline static SigImpl GameArguments__onUri {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 41 57 41 56 41 55 41 54 56 57 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 0F 29 B5 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 49 89 D4 49 89 CD"_sig,
        "GameArguments::_onUri"
    };

    inline static SigImpl RenderMaterialGroup__common {
        [](memory::signature_store& store, uintptr_t) {
            return store.deref(3);
        },
        "48 8D 0D ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 40 ? 48 8D 55 ? FF 15 ? ? ? ? 48 89 C1 48 8B 50 ? 48 85 D2 48 8B 7D ? 0F 84 ? ? ? ? 8B 42 ? 66 66 66 66 66 2E 0F 1F 84 00 ? ? ? ? 85 C0 0F 84 ? ? ? ? 44 8D 40 ? F0 44 0F B1 42 ? 75 ? 48 8B 01 48 8B 49 ? 48 89 87 ? ? ? ? 48 8B B7 ? ? ? ? 48 89 8F ? ? ? ? 48 85 F6 74 ? F0 FF 4E ? 75 ? 48 8B 06 48 8B 00 48 89 F1 FF 15 ? ? ? ? F0 FF 4E ? 75 ? 48 8B 06 48 8B 40 ? 48 89 F1 FF 15 ? ? ? ? 48 8B 45 ? 48 83 F8 ? 72 ? 48 8B 4D ? 48 8D 50 ? 48 81 FA ? ? ? ? 72 ? 4C 8B 41 ? 48 83 C1 ? 4C 29 C1 48 83 F9 ? 73"_sig,
        "mce::RenderMaterialGroup::common"
    };

    inline static SigImpl GuiData_displayClientMessage {
        [](memory::signature_store&, uintptr_t res) {
            return res;
        },
        "55 56 53 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 C7 85 ? ? ? ? ? ? ? ? 44 88 CB"_sig,
        "GuiData::displayClientMessage"
    };

    inline static SigImpl BaseActorRenderer_renderText { [](memory::signature_store& store, uintptr_t) {
                                                            return store.deref(1);
                                                        },
                                                         "E8 ? ? ? ? 49 81 C7 ? ? ? ? 4D 39 E7 75 ? 48 83 C4"_sig,
                                                         "BaseActorRenderer_renderText" };

    inline static SigImpl AppPlatformGDK_releaseMouse { [](memory::signature_store&, uintptr_t res) {
                                                           return res;
                                                       },
                                                        "56 57 48 83 EC ? 48 89 CE B9 ? ? ? ? FF 15"_sig,
                                                        "AppPlatformGDK_releaseMouse" };
};

// after adding sigs here, add them in latite.cpp
