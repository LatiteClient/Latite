#include "DebugInfo.h"
#include "Windows.h"

#include "client/Latite.h"
#include "client/event/impl/DrawHUDModulesEvent.h"
#include "client/event/impl/RenderOverlayEvent.h"

#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/world/level/Dimension.h"
#include "sdk/signature/storage.h"

DebugInfo::DebugInfo() : Module("DebugInfo", "Debug Info", "See some craaazy info (send help)", GAME, VK_F3) {
    listen<RenderOverlayEvent>((EventListenerFunc)&DebugInfo::onRenderOverlay);
    listen<DrawHUDModulesEvent>((EventListenerFunc)&DebugInfo::onRenderHUDModules, false, 2);
}

namespace {
    std::string getMinecraftVersion() {
        return std::format("Latite Client {}, Minecraft {}", Latite::get().version, Latite::get().gameVersion);
    }
    std::string getFPS() {
        return std::format("FPS: {} ({} MSPF)", Latite::get().getTimings().getFPS(), 1000.f / static_cast<float>(Latite::get().getTimings().getFPS()));
    }
    std::string getDimension() {
        return std::format("Dimension: {}", SDK::ClientInstance::get()->getLocalPlayer()->dimension->dimensionName);
    }
    std::string getCoordinates() {
        Vec3 position = SDK::ClientInstance::get()->getLocalPlayer()->getPos();
        return std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", position.x, position.y, position.z);
    }
    std::string getCPUInfo() {
        std::string cpuInfo = util::GetProcessorInfo();
        SYSTEM_INFO inf;
        GetSystemInfo(&inf);
        cpuInfo = std::to_string(inf.dwNumberOfProcessors) + "x " + cpuInfo;
        return cpuInfo;
    }
    // TODO: block info, tps info, tick speed info, biome info, days ran on server.


    std::string getMemUsage() {
        // Get the total available memory
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);
        GlobalMemoryStatusEx(&memStatus);

        // Convert memory usage values to GB
        double usedMemoryGB = static_cast<double>(memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024 * 1024 * 1024); // Convert to GB
        double totalMemoryGB = static_cast<double>(memStatus.ullTotalPhys) / (1024 * 1024 * 1024); // Convert to GB

        // Calculate the ratio of used memory to total memory
        double memoryUsageRatio = usedMemoryGB / totalMemoryGB;

        return std::format("Memory Usage: {:.2f} GB / {:.2f} GB", usedMemoryGB, totalMemoryGB);
    }
    std::string getGpuInfo() {
        return std::format("Display: {} (DirectX{})", reinterpret_cast<const char*>(Signatures::GpuInfo.result), Latite::getRenderer().isDX11ByDefault() ? "11/10.1" : "12");
    }
    std::string getCpuInfo() {
        return std::format("CPU: {}", util::GetProcessorInfo());
    }
}

void DebugInfo::onRenderOverlay(Event& evG) {
    RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(evG);
    DXContext dc;

    if (!SDK::ClientInstance::get()->minecraft->getLevel()) return;

    auto [width, height] = Latite::getRenderer().getScreenSize();
    d2d::Rect rect = { 0.f, 0.f, width, height };

    const std::wstring topLeftDebugInfo = util::StrToWStr(std::format("{}\n{}\n\n{}\n{}",
        getMinecraftVersion(),
        getFPS(),
        getDimension(),
        getCoordinates()));
    const std::wstring topRightDebugInfo = util::StrToWStr(std::format("{}\n{}\n{}",
        getMemUsage(),
        getCPUInfo(),
        getGpuInfo()));

    dc.drawText(rect, topLeftDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular,
        28, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    dc.drawText(rect, topRightDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular,
        28, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void DebugInfo::onRenderHUDModules(Event& evGeneric) {
    auto& ev = reinterpret_cast<DrawHUDModulesEvent&>(evGeneric);
    ev.setCancelled(true);
}
