#include "DebugInfo.h"
#include "Windows.h"

#include "client/Latite.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include <format>

#include "sdk/signature/storage.h"

DebugInfo::DebugInfo() : Module("DebugInfo", "Debug Info", "See some craaazy info (send help)", GAME, this->debugInfoKey) {
    listen<RenderOverlayEvent>((EventListenerFunc)&DebugInfo::onRenderOverlay);
}

namespace {
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
    // not including cpu usage cause that's annoying
    std::string getGpuInfo() {
        return std::format("GPU: {}", reinterpret_cast<const char*>(Signatures::GpuInfo.result));
    }
    std::string getMinecraftVersion() {
        return Latite::get().gameVersion;
    }
}

void DebugInfo::onRenderOverlay(Event& evG) {
    RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(evG);
    DXContext dc;

    auto [width, height] = Latite::getRenderer().getScreenSize();
    d2d::Rect rect = { 0.f, 0.f, width, height };
    const std::wstring topLeftDebugInfo = util::StrToWStr(std::format("Latite Client, Minecraft {}", getMinecraftVersion()));
    const std::wstring topRightDebugInfo = util::StrToWStr(std::format("{}\n{}", getMemUsage(), getGpuInfo()));

    dc.drawText(rect, topLeftDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular,
            25, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    dc.drawText(rect, topRightDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular,
        25, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}