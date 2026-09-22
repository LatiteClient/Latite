#pragma once
#include "../../TextModule.h"
#include <chrono>
#include <mutex>

class JitterDisplay : public TextModule {
public:
    JitterDisplay();

    std::wstringstream text(bool isDefault, bool inEditor) override;

    void onPacketReceive(Event& ev);

private:
    std::mutex jitterMutex;
    std::chrono::steady_clock::time_point lastBatchTime {};
    bool hasLastBatch = false;
    double lastInterval = -1.0;
    double jitterMs = 0.0;
};
