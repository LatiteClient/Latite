#pragma once

#include "../../Module.h"
#include "client/screen/impl/WaypointPopupScreen.h"

class Waypoints : public Module {
public:
    Waypoints();

private:
    void onRenderOverlay(Event& evG);
    void onKey(Event& evG);

    ValueType addWaypointKeySetting = KeyValue('B');
    ValueType bgColorSetting = ColorValue(0.1f, 0.1f, 0.1f, 0.5f);
};
