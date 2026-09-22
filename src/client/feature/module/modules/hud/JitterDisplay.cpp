#include "pch.h"
#include "JitterDisplay.h"
#include "mc/common/network/RemoteConnectorComposite.h"
#include "client/event/events/PacketReceiveEvent.h"
#include "client/event/Eventing.h"
#include <algorithm>
#include <cmath>

JitterDisplay::JitterDisplay()
    : TextModule("JitterDisplay", LocalizeString::get("client.textmodule.jitterDisplay.name"),
                 LocalizeString::get("client.textmodule.jitterDisplay.desc"), HUD) {
    this->suffix = TextValue(L" ms");

    listen<PacketReceiveEvent>((EventListenerFunc)&JitterDisplay::onPacketReceive, true);
}

std::wstringstream JitterDisplay::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    int dJitter = 0;

    if (inEditor) {
        dJitter = 3;
    } else {
        auto* connectionInfo = SDK::RemoteConnectorComposite::getConnectionInfo();
        if (connectionInfo && !connectionInfo->hostIpAddress.empty()) {
            auto now = std::chrono::steady_clock::now();
            double currentJitter = 0.0;
            {
                std::lock_guard lock(jitterMutex);
                if (hasLastBatch && std::chrono::duration<double, std::milli>(now - lastBatchTime).count() < 2000.0) {
                    currentJitter = jitterMs;
                }
            }
            dJitter = std::clamp(static_cast<int>(std::round(currentJitter)), 0, 9999);
        }
    }

    wss << dJitter;

    return wss;
}

void JitterDisplay::onPacketReceive(Event& evGeneric) {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard lock(jitterMutex);

    if (!hasLastBatch) {
        lastBatchTime = now;
        hasLastBatch = true;
        return;
    }

    double dt = std::chrono::duration<double, std::milli>(now - lastBatchTime).count();
    // Packets within the same batch or frame arrive almost simultaneously; ignore intra-batch deltas
    if (dt < 4.0) {
        return;
    }

    lastBatchTime = now;

    // A gap longer than 1000ms likely indicates dimension change, pause, or severe freeze; reset baseline
    if (dt > 1000.0) {
        lastInterval = -1.0;
        return;
    }

    if (lastInterval > 0.0) {
        double delta = std::abs(dt - lastInterval);
        // RFC 3550 jitter filter: J = J + (|D| - J) / 16
        jitterMs += (delta - jitterMs) / 16.0;
    }
    lastInterval = dt;
}
