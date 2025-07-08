#ifndef PCH_H
#define PCH_H

// Put commonly included files here to speed up the build process

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <memory>
#include <future> 
#include <execution>
#include <optional>
#include <chrono>
#include <format>
#include <array>

using namespace std::chrono_literals;

// Libraries
#include <glm/mat4x4.hpp>
#include <mnemosyne/scan/signature.hpp>
using namespace mnem::sig_literals;

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <libhat.hpp>

// COM
#include <d2d1.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <d3d12.h>

// WinRT

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/windows.system.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/windows.security.cryptography.h>
#include <winrt/windows.security.cryptography.core.h>

// SDK
#include "mc/Version.h"
#include "mc/common/client/game/ClientInstance.h"
#include "mc/common/client/game/MinecraftGame.h"
#include "mc/common/client/player/LocalPlayer.h"
#include "mc/common/world/Minecraft.h"
#include "mc/common/world/level/Dimension.h"
#include "mc/Addresses.h"
#include "mc/common/client/gui/GuiData.h"
#include "mc/common/network/RakNetConnector.h"
#include "mc/deps/input/MouseDevice.h"

// Client
#include "client/memory/hook/Hook.h"
#include "client/script/JsPlugin.h"
#include "client/script/PluginManager.h"
#include "client/screen/TextBox.h"
#include "client/misc/Timings.h"
#include "client/misc/ClientMessageQueue.h"
#include "client/event/Events.h"
#include "client/Latite.h"

#include "client/event/Event.h"
#include "client/event/Listener.h"
#include "client/feature/Feature.h"
#include "client/manager/Manager.h"
#include "client/manager/StaticManager.h"

// Features
#include "client/feature/command/Command.h"
#include "client/feature/command/CommandManager.h"
#include "client/feature/module/HUDModule.h"
#include "client/feature/module/Module.h"
#include "client/feature/module/ModuleManager.h"
#include "client/feature/module/TextModule.h"

// Utils
#include "util/Util.h"
#include "util/Logger.h"
#include "util/XorString.h"
#include "util/ChakraUtil.h"
#include "util/DxUtil.h"
#include "util/LMath.h"
#include "util/DrawContext.h"
#include "util/Crypto.h"
#include "client/localization/LocalizeString.h"
#include "util/memory.h"
#endif