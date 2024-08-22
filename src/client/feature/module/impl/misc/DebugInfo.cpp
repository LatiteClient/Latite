#include "pch.h"
#include "DebugInfo.h"

#include "client/Latite.h"
#include "client/render/Renderer.h"
#include "client/event/impl/DrawHUDModulesEvent.h"
#include "client/event/impl/RenderLayerEvent.h"

#include "sdk/common/world/level/HitResult.h"
#include "sdk/common/client/gui/ScreenView.h"
#include "sdk/common/client/gui/controls/UIControl.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include <util/DxContext.h>

DebugInfo::DebugInfo() : Module("DebugInfo", LocalizeString::get("client.module.debugInfo.name"),
                                LocalizeString::get("client.module.debugInfo.desc"), GAME, VK_F3) {
    listen<RenderLayerEvent>(static_cast<EventListenerFunc>(&DebugInfo::onRenderOverlay));
    listen<DrawHUDModulesEvent>(static_cast<EventListenerFunc>(&DebugInfo::onRenderHUDModules), false, 2);
}

namespace {
    std::string getMinecraftVersion() {
        return std::format("Latite Client {}, Minecraft {}", Latite::get().version, Latite::get().gameVersion);
    }
    std::string getFPS() {
        return std::format("FPS: {}", Latite::get().getTimings().getFPS());
    }
    std::string getDimension() {
        return std::format("Dimension: {}", SDK::ClientInstance::get()->getLocalPlayer()->dimension->dimensionName);
    }
    std::string getCoordinates() {
        Vec3 position = SDK::ClientInstance::get()->getLocalPlayer()->getPos();
        std::string ln1 = std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", position.x, position.y, position.z);
        std::string ln2;
        if (SDK::ClientInstance::get()->getLocalPlayer()->dimension->dimensionName == "Nether") ln2 = std::format("Overworld: {:.1f} / {:.1f} / {:.1f}", position.x * 8.f, position.y, position.z * 8.f);
        return ln1 + "\n" + ln2;
    }
    std::string getVelocity() {
        Vec3 velocity = SDK::ClientInstance::get()->getLocalPlayer()->getVelocity();
        return std::format("Velocity: {:.3f} / {:.3f} / {:.3f}", velocity.x, velocity.y, velocity.z);
    }
    std::string getRotation() {
        Vec2 rotation = SDK::ClientInstance::get()->getLocalPlayer()->getRot();
        return std::format("Facing: {:.3f} / {:.3f}", rotation.x, rotation.y);
    }
    std::string getLookingAt() {
        auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
        std::string fin;
        if (lvl->getHitResult()->hitType == SDK::HitType::BLOCK) {
            BlockPos solidBlock = lvl->getHitResult()->hitBlock;
            fin += std::format("Looking at block: {} {} {}\n", solidBlock.x, solidBlock.y, solidBlock.z);
        }
        if (lvl->getLiquidHitResult()->hitType == SDK::HitType::BLOCK) {
            BlockPos liquidBlock = lvl->getLiquidHitResult()->liquidBlock;
            fin += std::format("Looking at liquid: {} {} {}", liquidBlock.x, liquidBlock.y, liquidBlock.z);
        }
        return fin;
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
        return std::format("Display: Unknown (DirectX{})", Latite::getRenderer().isDX11ByDefault() ? "11/10.1" : "12");
    }
    std::string getCpuInfo() {
        std::string cpuInfo = util::GetProcessorInfo();
        SYSTEM_INFO inf;
        GetSystemInfo(&inf);
        cpuInfo = std::to_string(inf.dwNumberOfProcessors) + "x " + cpuInfo;
        return cpuInfo;
    }
    std::string getRenderPerfInfo() {
        static std::vector<float> arpPerf = {};
        static std::vector<float> d2dPerf = {};
        static std::vector<float> d3dPerf = {};

        auto chkVec = [](std::vector<float>& vec, float add) -> float {
            vec.insert(vec.begin(), add);
            if (vec.size() > 100) {
                vec.pop_back();
            }

            float avg = 0.f;
            for (auto& a : vec) {
                avg += a;
            }
            return avg / (float)vec.size();
        };

        float arp = chkVec(arpPerf, Latite::getRenderer().arpPerf / 1000.f);
        float d2d = chkVec(d2dPerf, Latite::getRenderer().d2dPerf / 1000.f);
        float d3d = chkVec(d3dPerf, Latite::getRenderer().d3dPerf / 1000.f);

        return std::format("\nAverages:\nAcquireWrappedResources: {:.3f}ms\nLatite Direct2D total: {:.3f}ms \nLatite Direct3D total: {:.3f}ms", arp, d2d, d3d);
    }
}
#undef GETFPS

void DebugInfo::onRenderOverlay(Event& evG) {
    RenderLayerEvent& ev = reinterpret_cast<RenderLayerEvent&>(evG);
    MCDrawUtil dc{ ev.getUIRenderContext(), Latite::get().getFont() };

    if (!SDK::ClientInstance::get()->getLocalPlayer()) return;

    if (ev.getScreenView()->visualTree->rootControl->name == "hud_screen") {
        auto [width, height] = SDK::ClientInstance::get()->getGuiData()->screenSize;
        d2d::Rect rect = { 0.f, 0.f, width, height };

        const std::wstring topLeftDebugInfo = util::StrToWStr(std::format("{}\n{}\n\n{}\n{}\n{}\n{}",
            getMinecraftVersion(),
            getFPS(),
            getDimension(),
            getCoordinates(),
            //getVelocity(),
            getRotation(),
            getLookingAt()));
        const std::wstring topRightDebugInfo = util::StrToWStr(std::format("{}\n{}\n{}\n",
            getMemUsage(),
            getGpuInfo(),
            getCpuInfo()));

        dc.drawText(rect, topLeftDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
            28, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);

        dc.drawText(rect, topRightDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
            28, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);

        dc.flush(true, false);
    }
}

void DebugInfo::onRenderHUDModules(Event& evGeneric) {
    auto& ev = reinterpret_cast<DrawHUDModulesEvent&>(evGeneric);
    ev.setCancelled(true);
}
