#include "pch.h"
#include "MotionBlur.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"

#include "client/event/impl/RendererInitEvent.h"

MotionBlur::MotionBlur() : Module("MotionBlur", LocalizeString::get("client.module.motionBlur.name"),
    LocalizeString::get("client.module.motionBlur.desc"), GAME) {
    addSetting("usePixelAverage", LocalizeString::get("client.module.motionBlur.usePixelAverage.name"), LocalizeString::get("client.module.motionBlur.usePixelAverage.desc"), usePixelAverage);
    addSliderSetting("intensity", LocalizeString::get("client.module.motionBlur.intensity.name"), LocalizeString::get("client.module.motionBlur.intensity.desc"), intensity,
        FloatValue(0.f), FloatValue(20.f), FloatValue(1.f));
    addSliderSetting("opacity", LocalizeString::get("client.module.motionBlur.opacity.name"), LocalizeString::get("client.module.motionBlur.opacity.desc"), opacity,
        FloatValue(2.f), FloatValue(16.f), FloatValue(1.f));

    listen<RendererCleanupEvent>(&MotionBlur::onCleanup);
    listen<RenderOverlayEvent>(&MotionBlur::onRender, true, 10);
    listen<RendererInitEvent>(&MotionBlur::onRendererInit, true);
}

void MotionBlur::clearFrames() {
    SafeRelease(&m_previousFrameBitmap);
    for (auto& frame : m_frameHistory) {
        SafeRelease(&frame);
    }
	
    m_frameHistory.clear();
}

void MotionBlur::onEnable() {
    clearFrames();
}

void MotionBlur::onDisable() {
    clearFrames();
}

void MotionBlur::onCleanup(Event&) {
    clearFrames();
}

void MotionBlur::onRendererInit(Event&) {
    clearFrames();
}

void MotionBlur::onRender(Event& genericEv) {
    if (!this->isEnabled()) {
        return;
    }

    bool currentModeIsPixelAverage = std::get<BoolValue>(usePixelAverage);
    if (currentModeIsPixelAverage != m_lastModeWasPixelAverage) {
        clearFrames();
        m_lastModeWasPixelAverage = currentModeIsPixelAverage;
    }

    RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(genericEv);

    ID2D1DeviceContext* ctx = ev.getDeviceContext();
    Renderer* renderer = &Latite::getRenderer();
    D2D1_SIZE_F screenSize = renderer->getScreenSize();
    D2D1_RECT_F rc = D2D1::RectF(0.f, 0.f, screenSize.width, screenSize.height);
    float opacityValue = std::get<FloatValue>(opacity);

    if (currentModeIsPixelAverage) {
        if (!m_frameHistory.empty() && opacityValue > 0) {
            float maxOpacity = opacityValue / 10.f;
            if (maxOpacity > 1.f) maxOpacity = 1.f;

            size_t frameCount = m_frameHistory.size();

            for (size_t i = 0; i < frameCount; ++i) {
                ID2D1Bitmap1* frame = m_frameHistory[i];
                if (frame) {
                    float ageFactor = frameCount > 1 ? static_cast<float>(i) / (frameCount - 1) : 1.f;
                    float finalOpacity = maxOpacity * powf(ageFactor, 3.f);

                    ctx->DrawBitmap(frame, &rc, finalOpacity);
                }
            }
        }

        ID2D1Bitmap1* currentFrame = renderer->copyCurrentBitmap();
        if (currentFrame) {
            m_frameHistory.push_back(currentFrame);
        }

        size_t intensityValue = static_cast<size_t>(std::get<FloatValue>(intensity).getInt());
        while (m_frameHistory.size() > intensityValue) {
            SafeRelease(&m_frameHistory.front());
            m_frameHistory.erase(m_frameHistory.begin());
        }
    } else {
        if (m_previousFrameBitmap) {
            float blendOpacity = opacityValue / 12.f;
            if (blendOpacity > 1.f) blendOpacity = 1.f;
            ctx->DrawBitmap(m_previousFrameBitmap, &rc, blendOpacity);
        }

        SafeRelease(&m_previousFrameBitmap);
        m_previousFrameBitmap = renderer->copyCurrentBitmap();
    }
}