#pragma once

#include "api/eventing/Event.h"
#include "sdk/common/client/renderer/ItemRenderer.h"
#include "sdk/common/world/actor/ActorRenderData.h"

class PreRenderItemEvent : public Event {
public:
    static const uint32_t hash = TOHASH(PreRenderItemEvent);

    PreRenderItemEvent(SDK::ItemRenderer* renderer, SDK::BaseActorRenderContext* renderContext, SDK::ActorRenderData* renderData) :
            renderer(renderer), renderContext(renderContext), renderData(renderData) {}

    [[nodiscard]] SDK::ItemRenderer* getRenderer() const {
        return renderer;
    }

    [[nodiscard]] SDK::BaseActorRenderContext* getRenderContext() const {
        return renderContext;
    }

    [[nodiscard]] SDK::ActorRenderData* getRenderData() const {
        return renderData;
    }

protected:
    SDK::ItemRenderer* renderer;
    SDK::BaseActorRenderContext* renderContext;
    SDK::ActorRenderData* renderData;
};
