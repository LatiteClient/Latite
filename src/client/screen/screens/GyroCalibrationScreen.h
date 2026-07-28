#pragma once

#include "../Screen.h"

#include <chrono>
#include <functional>
#include <string>

class GyroCalibrationScreen final : public Screen {
public:
    enum class Purpose {
        Calibration,
        Diagnostics
    };

    enum class ProgressStage {
        Measuring,
        Verifying,
        Diagnosing
    };

    GyroCalibrationScreen();

    std::string getName() override { return "GyroCalibration"; }

    void showPrompt(Purpose purpose, std::wstring deviceName, std::function<void()> startAction);
    void showProgress(Purpose purpose, ProgressStage stage, std::wstring deviceName,
                      std::chrono::milliseconds expectedDuration, std::function<void()> cancelAction);
    void updateSampleCount(std::size_t sampleCount);
    void showResult(Purpose purpose, bool successful, std::wstring deviceName, std::wstring summary,
                    std::wstring detail = {});

protected:
    void onEnable(bool ignoreAnimations) override;
    void onDisable() override;

private:
    enum class Content {
        Prompt,
        Progress,
        Result
    };

    void onRender(Event& event);
    void onClick(Event& event);
    void renderPrompt(class D2DUtil& dc, float scale);
    void renderProgress(class D2DUtil& dc, float scale);
    void renderResult(class D2DUtil& dc, float scale);

    Purpose purpose = Purpose::Calibration;
    ProgressStage progressStage = ProgressStage::Measuring;
    Content content = Content::Prompt;
    std::wstring deviceName;
    std::wstring summary;
    std::wstring detail;
    std::function<void()> startAction;
    std::function<void()> cancelAction;
    std::chrono::steady_clock::time_point progressStartedAt = {};
    std::chrono::milliseconds expectedDuration = {};
    std::size_t sampleCount = 0;
    d2d::Rect panelRect = {};
    d2d::Rect closeButtonRect = {};
    d2d::Rect contentRect = {};
    d2d::Rect primaryButtonRect = {};
    d2d::Rect detailViewportRect = {};
    float detailScroll = 0.f;
    float detailScrollMax = 0.f;
    bool resultSuccessful = false;
};
