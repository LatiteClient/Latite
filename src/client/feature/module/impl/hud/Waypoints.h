#pragma once

#include "../../Module.h"
#include "client/screen/impl/WaypointPopupScreen.h"

struct WaypointData {
    std::wstring name;
    std::wstring initials;
    Color color;
    Vec3 position;
    std::wstring dimension;
};

class Waypoints : public Module {
public:
    Waypoints();
    void addWaypoint(const WaypointData& waypoint);

private:
    void onRenderOverlay(Event& evG);
    void onKey(Event& evG);

    std::vector<WaypointData> waypoints;
    std::optional<Vec2> smoothedScreenPos = std::nullopt;

    ValueType addWaypointKeySetting = KeyValue('B');
    ValueType bgColorSetting = ColorValue(0.1f, 0.1f, 0.1f, 0.5f);
};
