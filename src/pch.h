#ifndef PCH_H
#define PCH_H

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

#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/world/level/Dimension.h"
#include "sdk/signature/storage.h"

#include "util/Util.h"
#include "util/Logger.h"
#include "util/XorString.h"
#include "util/ChakraUtil.h"
#include "util/DxUtil.h"
#include "util/LMath.h"
#include "util/Json.h"
#include "util/DxContext.h"
#endif