#pragma once
#include "../../HUDModule.h"

class DirectionHud : public HUDModule {
public:
    DirectionHud();

    void render(DrawUtil& dc, bool isDefault, bool inEditor) override;

private:
    enum Anchor {
        AnchorBottom = 0,
        AnchorCenter = 1,
        AnchorTop = 2
    };

    ValueType span = FloatValue(120.f);
    ValueType showLabels = BoolValue(true);
    ValueType showIntercardinals = BoolValue(true);
    ValueType showTicks = BoolValue(true);
    ValueType showMarker = BoolValue(true);
    ValueType showHeading = BoolValue(false);
    ValueType fadeEdges = BoolValue(true);

    ValueType fillBg = BoolValue(true);
    ValueType bgColor = ColorValue(0.f, 0.f, 0.f, 0.5f);
    ValueType bgRadius = FloatValue(6.f);

    ValueType tickHeightRatio = FloatValue(0.4f);
    ValueType minorTickHeightRatio = FloatValue(0.2f);
    ValueType tickWidth = FloatValue(2.f);
    ValueType minorTickWidth = FloatValue(1.f);
    ValueType minorTicksPerMajor = FloatValue(2.f);
    ValueType markerWidth = FloatValue(2.f);
    ValueType labelFontSize = FloatValue(18.f);
    ValueType intercardinalFontSize = FloatValue(14.f);
    ValueType headingFontSize = FloatValue(14.f);

    ValueType tickOffset = FloatValue(0.f);
    ValueType labelOffset = FloatValue(6.f);
    ValueType headingOffset = FloatValue(0.f);

    ValueType textColor = ColorValue(1.f, 1.f, 1.f, 1.f);
    ValueType intercardinalColor = ColorValue(1.f, 1.f, 1.f, 1.f);
    ValueType headingColor = ColorValue(1.f, 1.f, 1.f, 1.f);
    ValueType markerColor = ColorValue(1.f, 0.33f, 0.33f, 1.f);
    ValueType tickColor = ColorValue(0.67f, 0.67f, 0.67f, 1.f);

    ValueType compassW = FloatValue(300.f);
    ValueType compassH = FloatValue(50.f);

    EnumData tickAnchor;
    EnumData labelPos;
    EnumData degPos;

    float fadedAlpha(float delta, float halfSpan);
    static void zoneRect(int anchor, float h, float& top, float& bot);
    static float wrapDegrees(float angle);
    static float bearingOf(float yaw);
};
