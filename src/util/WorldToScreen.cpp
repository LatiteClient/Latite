#pragma once
#include "WorldToScreen.h"
#include "sdk/common/client/game/ClientHMDState.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/gui/GuiData.h"
#include <glm/glm.hpp>

namespace WorldToScreen {
    std::optional<Vec2> convert(const Vec3& worldPos) {
        SDK::ClientInstance* clientInstance = SDK::ClientInstance::get();
        if (!clientInstance) return std::nullopt;

        if (!clientInstance->levelRenderer) return std::nullopt;

        SDK::GuiData* guiData = clientInstance->getGuiData();
        if (!guiData) return std::nullopt;

        SDK::LevelRendererPlayer* levelRendererPlayer = clientInstance->levelRenderer->getLevelRendererPlayer();
        if (!levelRendererPlayer) return std::nullopt;

        SDK::GameRenderer* gameRenderer = clientInstance->minecraftGame->gameRenderer;
        if (!gameRenderer) return std::nullopt;


        const auto& screenSize = guiData->screenSize;
        Vec3 origin = levelRendererPlayer->getOrigin();

        Vec3 pos = worldPos - origin;

        const glm::mat4x4& viewMatrix = gameRenderer->lastViewMatrix._m;
        const glm::mat4x4& projMatrix = gameRenderer->lastProjectionMatrix._m;

        glm::mat4x4 mvp = projMatrix * viewMatrix;
        glm::vec4 clipCoords = mvp * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
        if (clipCoords.w < 0.1f) {
            return std::nullopt;
        }

        glm::vec2 ndc;
        ndc.x = clipCoords.x / clipCoords.w;
        ndc.y = clipCoords.y / clipCoords.w;

        Vec2 screenPos;
        screenPos.x = (ndc.x + 1.0f) * 0.5f * screenSize.x;
        screenPos.y = (1.0f - ndc.y) * 0.5f * screenSize.y;

        return Vec2(screenPos.x, screenPos.y);
    }
}
