#include "pch.h"
#include "GyroCalibrationScreen.h"

#include "client/event/Eventing.h"
#include "client/event/events/ClickEvent.h"
#include "client/event/events/RenderOverlayEvent.h"
#include "client/Latite.h"
#include "client/localization/LocalizeString.h"
#include "client/render/asset/Assets.h"
#include "util/DrawContext.h"
#include "util/Util.h"

#include <algorithm>
#include <utility>

GyroCalibrationScreen::GyroCalibrationScreen() {
    Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&GyroCalibrationScreen::onRender, 1, true);
    Eventing::get().listen<ClickEvent>(this, (EventListenerFunc)&GyroCalibrationScreen::onClick, 4);
}

void GyroCalibrationScreen::showPrompt(Purpose newPurpose, std::wstring newDeviceName,
                                       std::function<void()> newStartAction) {
    purpose = newPurpose;
    content = Content::Prompt;
    deviceName = std::move(newDeviceName);
    summary.clear();
    detail.clear();
    startAction = std::move(newStartAction);
    cancelAction = {};
    sampleCount = 0;
    detailScroll = 0.f;
}

void GyroCalibrationScreen::showProgress(Purpose newPurpose, ProgressStage newStage, std::wstring newDeviceName,
                                         std::chrono::milliseconds newExpectedDuration,
                                         std::function<void()> newCancelAction) {
    purpose = newPurpose;
    progressStage = newStage;
    content = Content::Progress;
    deviceName = std::move(newDeviceName);
    expectedDuration = newExpectedDuration;
    progressStartedAt = std::chrono::steady_clock::now();
    startAction = {};
    cancelAction = std::move(newCancelAction);
    sampleCount = 0;
    detailScroll = 0.f;
}

void GyroCalibrationScreen::updateSampleCount(std::size_t newSampleCount) {
    sampleCount = newSampleCount;
}

void GyroCalibrationScreen::showResult(Purpose newPurpose, bool successful, std::wstring newDeviceName,
                                       std::wstring newSummary, std::wstring newDetail) {
    purpose = newPurpose;
    content = Content::Result;
    resultSuccessful = successful;
    deviceName = std::move(newDeviceName);
    summary = std::move(newSummary);
    detail = std::move(newDetail);
    startAction = {};
    cancelAction = {};
    detailScroll = 0.f;
}

void GyroCalibrationScreen::onEnable(bool) {
    detailScroll = 0.f;
    resetInputState();
}

void GyroCalibrationScreen::onDisable() {
    resetInputState();
    std::function<void()> action;
    if (content == Content::Progress) action = std::move(cancelAction);
    startAction = {};
    cancelAction = {};
    if (action) action();
}

void GyroCalibrationScreen::onRender(Event&) {
    if (!isActive()) return;

    D2DUtil dc;
    D2D1_SIZE_F screenSize = Latite::getRenderer().getScreenSize();
    Vec2 cursorPosition = SDK::ClientInstance::get()->cursorPos;
    d2d::Color accent = d2d::Color(Latite::get().getAccentColor().getMainColor());

    if (Latite::get().getMenuBlur()) dc.drawGaussianBlur(Latite::get().getMenuBlur().value());

    float scale = std::clamp(screenSize.width / 1920.f, 0.72f, 1.1f);
    float panelWidth = std::min(screenSize.width * 0.72f, 760.f * scale);
    float panelHeight = std::min(screenSize.height * 0.76f, 610.f * scale);
    panelRect = { (screenSize.width - panelWidth) * 0.5f, (screenSize.height - panelHeight) * 0.5f,
                  (screenSize.width + panelWidth) * 0.5f, (screenSize.height + panelHeight) * 0.5f };

    float padding = 26.f * scale;
    float radius = 19.f * scale;
    float headerHeight = 86.f * scale;
    float footerHeight = 76.f * scale;
    d2d::Color panelColor = d2d::Color::RGB(0x07, 0x07, 0x07).asAlpha(0.78f);
    d2d::Color outlineColor = d2d::Color::RGB(0x00, 0x00, 0x00).asAlpha(0.3f);

    dc.fillRoundedRectangle(panelRect, panelColor, radius);
    dc.drawRoundedRectangle(panelRect, outlineColor, radius, 4.f * scale, DrawUtil::OutlinePosition::Outside);

    float closeSize = 22.f * scale;
    closeButtonRect = { panelRect.right - padding - closeSize, panelRect.top + 27.f * scale, panelRect.right - padding,
                        panelRect.top + 27.f * scale + closeSize };

    std::string titleKey = "client.screen.gyroCalibration.calibration.title";
    if (purpose == Purpose::Diagnostics) titleKey = "client.screen.gyroCalibration.diagnostics.title";
    std::wstring title = LocalizeString::get(titleKey);
    dc.drawText({ panelRect.left + padding, panelRect.top + 18.f * scale, closeButtonRect.left - 16.f * scale,
                  panelRect.top + 50.f * scale },
                title, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryLight, 25.f * scale,
                DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, false);

    std::wstring displayedDevice = deviceName;
    if (displayedDevice.empty()) displayedDevice = LocalizeString::get("client.screen.gyroCalibration.noDevice");
    std::wstring deviceFormat = LocalizeString::get("client.screen.gyroCalibration.device");
    std::wstring deviceText = util::FormatWString(deviceFormat, { displayedDevice });
    dc.drawSingleLineFitted({ panelRect.left + padding + 1.f * scale, panelRect.top + 50.f * scale,
                              closeButtonRect.left - 16.f * scale, panelRect.top + 74.f * scale },
                            deviceText, d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.72f),
                            Renderer::FontSelection::PrimaryRegular, 14.f * scale);

    if (ID2D1Bitmap* closeIcon = Latite::getAssets().xIcon.getBitmap()) {
        dc.ctx->DrawBitmap(closeIcon, closeButtonRect, closeButtonRect.contains(cursorPosition) ? 1.f : 0.72f);
    }

    contentRect = { panelRect.left + padding, panelRect.top + headerHeight, panelRect.right - padding,
                    panelRect.bottom - footerHeight };
    primaryButtonRect = { panelRect.right - padding - 170.f * scale, panelRect.bottom - 55.f * scale,
                          panelRect.right - padding, panelRect.bottom - 19.f * scale };

    if (content == Content::Prompt)
        renderPrompt(dc, scale);
    else if (content == Content::Progress)
        renderProgress(dc, scale);
    else
        renderResult(dc, scale);

    std::string buttonKey = "client.screen.gyroCalibration.close";
    if (content == Content::Prompt) {
        buttonKey = "client.screen.gyroCalibration.startCalibration";
        if (purpose == Purpose::Diagnostics) buttonKey = "client.screen.gyroCalibration.startDiagnostics";
    } else if (content == Content::Progress) {
        buttonKey = "client.screen.gyroCalibration.cancel";
    }
    std::wstring buttonText = LocalizeString::get(buttonKey);

    bool buttonHovered = primaryButtonRect.contains(cursorPosition);
    d2d::Color buttonColor = buttonHovered ? accent : d2d::Color::RGB(0x38, 0x38, 0x38).asAlpha(0.9f);
    dc.fillRoundedRectangle(primaryButtonRect, buttonColor, primaryButtonRect.getHeight() * 0.23f);
    dc.drawText(primaryButtonRect, buttonText, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
                15.f * scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void GyroCalibrationScreen::renderPrompt(D2DUtil& dc, float scale) {
    d2d::Color cardColor = d2d::Color::RGB(0x12, 0x12, 0x12).asAlpha(0.62f);
    dc.fillRoundedRectangle(contentRect, cardColor, 13.f * scale);

    std::string promptKey = "client.module.gyro.calibration.prompt";
    if (purpose == Purpose::Diagnostics) promptKey = "client.module.gyro.calibration.diagnosticsPrompt";
    std::wstring prompt = LocalizeString::get(promptKey);
    d2d::Rect promptRect { contentRect.left + 24.f * scale, contentRect.top + 25.f * scale,
                           contentRect.right - 24.f * scale, contentRect.top + 118.f * scale };
    dc.drawWrappedTextClipped(promptRect, prompt, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
                              21.f * scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    d2d::Rect tipsRect { contentRect.left + 30.f * scale, contentRect.top + 142.f * scale,
                         contentRect.right - 30.f * scale, contentRect.bottom - 24.f * scale };
    dc.drawWrappedTextClipped(tipsRect, LocalizeString::get("client.screen.gyroCalibration.instructions"),
                              d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.82f), Renderer::FontSelection::PrimaryRegular,
                              16.f * scale);
}

void GyroCalibrationScreen::renderProgress(D2DUtil& dc, float scale) {
    d2d::Color accent = d2d::Color(Latite::get().getAccentColor().getMainColor());
    d2d::Color cardColor = d2d::Color::RGB(0x12, 0x12, 0x12).asAlpha(0.62f);
    dc.fillRoundedRectangle(contentRect, cardColor, 13.f * scale);

    std::string stageKey;
    if (progressStage == ProgressStage::Measuring) {
        stageKey = "client.screen.gyroCalibration.measuring";
    } else if (progressStage == ProgressStage::Verifying) {
        stageKey = "client.screen.gyroCalibration.verifying";
    } else {
        stageKey = "client.screen.gyroCalibration.diagnosing";
    }

    dc.drawText({ contentRect.left + 24.f * scale, contentRect.top + 35.f * scale, contentRect.right - 24.f * scale,
                  contentRect.top + 75.f * scale },
                LocalizeString::get(stageKey), d2d::Colors::WHITE, Renderer::FontSelection::PrimaryLight, 24.f * scale,
                DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, false);
    dc.drawWrappedTextClipped({ contentRect.left + 42.f * scale, contentRect.top + 90.f * scale,
                                contentRect.right - 42.f * scale, contentRect.top + 155.f * scale },
                              LocalizeString::get("client.screen.gyroCalibration.keepStill"),
                              d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.8f), Renderer::FontSelection::PrimaryRegular,
                              16.f * scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    auto elapsed = std::chrono::steady_clock::now() - progressStartedAt;
    float durationSeconds = std::chrono::duration<float>(expectedDuration).count();
    float elapsedSeconds = std::chrono::duration<float>(elapsed).count();
    float progress = durationSeconds > 0.f ? std::clamp(elapsedSeconds / durationSeconds, 0.f, 0.985f) : 0.f;

    d2d::Rect progressTrack { contentRect.left + 48.f * scale, contentRect.top + 185.f * scale,
                              contentRect.right - 48.f * scale, contentRect.top + 199.f * scale };
    d2d::Rect progressFill = progressTrack;
    progressFill.right = progressFill.left + progressTrack.getWidth() * progress;
    dc.fillRoundedRectangle(progressTrack, d2d::Color::RGB(0x55, 0x55, 0x55).asAlpha(0.42f),
                            progressTrack.getHeight() * 0.5f);
    if (progressFill.getWidth() > 0.f) dc.fillRoundedRectangle(progressFill, accent, progressTrack.getHeight() * 0.5f);

    float remainingSeconds = std::max(0.f, durationSeconds - elapsedSeconds);
    std::wstring remainingFormat = LocalizeString::get("client.screen.gyroCalibration.remaining");
    std::wstring remainingValue = std::format(L"{:.1f}", remainingSeconds);
    std::wstring remainingText = util::FormatWString(remainingFormat, { remainingValue });
    std::wstring sampleFormat = LocalizeString::get("client.screen.gyroCalibration.samples");
    std::wstring sampleText = util::FormatWString(sampleFormat, { std::to_wstring(sampleCount) });
    dc.drawText({ progressTrack.left, progressTrack.bottom + 14.f * scale, progressTrack.right,
                  progressTrack.bottom + 41.f * scale },
                remainingText, d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.72f),
                Renderer::FontSelection::PrimaryRegular, 14.f * scale);
    dc.drawText({ progressTrack.left, progressTrack.bottom + 14.f * scale, progressTrack.right,
                  progressTrack.bottom + 41.f * scale },
                sampleText, d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.72f), Renderer::FontSelection::PrimaryRegular,
                14.f * scale, DWRITE_TEXT_ALIGNMENT_TRAILING);
}

void GyroCalibrationScreen::renderResult(D2DUtil& dc, float scale) {
    d2d::Color statusColor = d2d::Color::RGB(0xF4, 0x43, 0x36);
    if (resultSuccessful) statusColor = d2d::Color::RGB(0x4C, 0xAF, 0x50);
    d2d::Rect summaryRect { contentRect.left, contentRect.top, contentRect.right, contentRect.top + 72.f * scale };
    dc.fillRoundedRectangle(summaryRect, statusColor.asAlpha(0.18f), 13.f * scale);
    dc.drawRoundedRectangle(summaryRect, statusColor.asAlpha(0.7f), 13.f * scale, 1.5f * scale);
    dc.drawWrappedTextClipped({ summaryRect.left + 18.f * scale, summaryRect.top + 10.f * scale,
                                summaryRect.right - 18.f * scale, summaryRect.bottom - 10.f * scale },
                              summary, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular, 18.f * scale,
                              DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    detailViewportRect = { contentRect.left + 8.f * scale, summaryRect.bottom + 17.f * scale,
                           contentRect.right - 8.f * scale, contentRect.bottom };
    if (detail.empty()) {
        detailScroll = 0.f;
        detailScrollMax = 0.f;
        return;
    }

    Renderer::FontSelection detailFont = Renderer::FontSelection::PrimaryRegular;
    float detailFontSize = 14.f * scale;
    float detailHeight = dc.getMeasuredTextHeight(detailViewportRect, detail, detailFont, detailFontSize);
    detailScrollMax = std::max(0.f, detailHeight - detailViewportRect.getHeight());
    detailScroll = std::clamp(detailScroll, 0.f, detailScrollMax);

    d2d::Rect detailTextRect = detailViewportRect;
    detailTextRect.top -= detailScroll;
    detailTextRect.bottom = detailTextRect.top + detailHeight;
    dc.ctx->PushAxisAlignedClip(dc.getRect(detailViewportRect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    dc.drawWrappedTextClipped(detailTextRect, detail, d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.84f), detailFont,
                              detailFontSize);
    dc.ctx->PopAxisAlignedClip();

    if (detailScrollMax <= 0.f) return;

    float thumbHeight = detailViewportRect.getHeight() * detailViewportRect.getHeight() / detailHeight;
    thumbHeight = std::max(25.f * scale, thumbHeight);
    float thumbTravel = detailViewportRect.getHeight() - thumbHeight;
    float thumbTop = detailViewportRect.top + thumbTravel * detailScroll / detailScrollMax;
    d2d::Rect scrollThumb { detailViewportRect.right + 6.f * scale, thumbTop, detailViewportRect.right + 10.f * scale,
                            thumbTop + thumbHeight };
    dc.fillRoundedRectangle(scrollThumb, d2d::Color::RGB(0xD2, 0xD2, 0xD2).asAlpha(0.68f),
                            scrollThumb.getWidth() * 0.5f);
}

void GyroCalibrationScreen::onClick(Event& genericEvent) {
    if (!isActive()) return;

    ClickEvent& event = reinterpret_cast<ClickEvent&>(genericEvent);
    ClickEvent::ClickType clickType = event.getClickType();
    if (clickType != ClickEvent::ClickType::None) event.setCancelled(true);

    if (clickType == ClickEvent::ClickType::Wheel) {
        if (content == Content::Result) {
            detailScroll =
                std::clamp(detailScroll - static_cast<float>(event.getWheelDelta()) / 3.f, 0.f, detailScrollMax);
        }
        return;
    }
    if (clickType != ClickEvent::ClickType::Left || !event.isDown()) return;

    Vec2 cursorPosition = SDK::ClientInstance::get()->cursorPos;
    if (closeButtonRect.contains(cursorPosition)) {
        playClickSound();
        close();
        return;
    }
    if (!primaryButtonRect.contains(cursorPosition)) return;

    playClickSound();
    if (content == Content::Prompt) {
        std::function<void()> action = std::move(startAction);
        if (action) action();
        return;
    }
    close();
}
