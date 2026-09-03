// Visuals inspired by (and partially adapted from) EvergreenHUD's DirectionHud:
// https://github.com/Polyfrost/EvergreenHUD/blob/oneconfig/src/main/kotlin/org/polyfrost/evergreenhud/client/hud/direction/DirectionHud.kt
#include "pch.h"
#include "DirectionHud.h"
#include "client/Latite.h"

DirectionHud::DirectionHud()
    : HUDModule("Direction", LocalizeString::get("client.hudmodule.direction.name"),
                LocalizeString::get("client.hudmodule.direction.desc"), HUD) {
    // Size
    addSliderSetting("compassW", LocalizeString::get("client.hudmodule.direction.width.name"),
                     LocalizeString::get("client.hudmodule.direction.width.desc"), compassW, FloatValue(MIN_W),
                     FloatValue(MAX_W), FloatValue(2.f));
    addSliderSetting("compassH", LocalizeString::get("client.hudmodule.direction.height.name"),
                     LocalizeString::get("client.hudmodule.direction.height.desc"), compassH, FloatValue(MIN_H),
                     FloatValue(MAX_H), FloatValue(2.f));
    addSliderSetting("span", LocalizeString::get("client.hudmodule.direction.span.name"),
                     LocalizeString::get("client.hudmodule.direction.span.desc"), span, FloatValue(45.f),
                     FloatValue(270.f), FloatValue(5.f));

    // Ticks
    addSetting("showTicks", LocalizeString::get("client.hudmodule.direction.showTicks.name"),
               LocalizeString::get("client.hudmodule.direction.showTicks.desc"), showTicks);
    addSliderSetting("tickHeight", LocalizeString::get("client.hudmodule.direction.tickHeight.name"),
                     LocalizeString::get("client.hudmodule.direction.tickHeight.desc"), tickHeightRatio,
                     FloatValue(0.f), FloatValue(1.f), FloatValue(0.05f), "showTicks"_istrue);
    addSliderSetting("minorTickHeight", LocalizeString::get("client.hudmodule.direction.minorTickHeight.name"),
                     LocalizeString::get("client.hudmodule.direction.minorTickHeight.desc"), minorTickHeightRatio,
                     FloatValue(0.f), FloatValue(1.f), FloatValue(0.05f), "showTicks"_istrue);
    addSliderSetting("minorTicks", LocalizeString::get("client.hudmodule.direction.minorTicks.name"),
                     LocalizeString::get("client.hudmodule.direction.minorTicks.desc"), minorTicksPerMajor,
                     FloatValue(0.f), FloatValue(8.f), FloatValue(1.f), "showTicks"_istrue);
    addSliderSetting("tickWidth", LocalizeString::get("client.hudmodule.direction.tickWidth.name"),
                     LocalizeString::get("client.hudmodule.direction.tickWidth.desc"), tickWidth, FloatValue(0.5f),
                     FloatValue(10.f), FloatValue(0.5f), "showTicks"_istrue);
    addSliderSetting("minorTickWidth", LocalizeString::get("client.hudmodule.direction.minorTickWidth.name"),
                     LocalizeString::get("client.hudmodule.direction.minorTickWidth.desc"), minorTickWidth,
                     FloatValue(0.5f), FloatValue(10.f), FloatValue(0.5f), "showTicks"_istrue);
    tickAnchor.replaceEntries({ { AnchorBottom, LocalizeString::get("client.hudmodule.direction.anchorBottom.name") },
                                { AnchorCenter, LocalizeString::get("client.hudmodule.direction.anchorCenter.name") },
                                { AnchorTop, LocalizeString::get("client.hudmodule.direction.anchorTop.name") } },
                              2);
    addEnumSetting("tickAnchor", LocalizeString::get("client.hudmodule.direction.tickAnchor.name"),
                   LocalizeString::get("client.hudmodule.direction.tickAnchor.desc"), this->tickAnchor,
                   "showTicks"_istrue);
    addSliderSetting("tickOffset", LocalizeString::get("client.hudmodule.direction.tickOffset.name"),
                     LocalizeString::get("client.hudmodule.direction.tickOffset.desc"), tickOffset,
                     FloatValue(OFFSET_MIN), FloatValue(OFFSET_MAX), FloatValue(1.f), "showTicks"_istrue);
    addSetting("tickCol", LocalizeString::get("client.hudmodule.direction.tickColor.name"),
               LocalizeString::get("client.hudmodule.direction.tickColor.desc"), tickColor, "showTicks"_istrue);

    // Direction letters (labels)
    addSetting("showLabels", LocalizeString::get("client.hudmodule.direction.showLabels.name"),
               LocalizeString::get("client.hudmodule.direction.showLabels.desc"), showLabels);
    addSetting("showIntercardinals", LocalizeString::get("client.hudmodule.direction.showIntercardinals.name"),
               LocalizeString::get("client.hudmodule.direction.showIntercardinals.desc"), showIntercardinals,
               "showLabels"_istrue);
    addSliderSetting(
        "intercardinalFontSize", LocalizeString::get("client.hudmodule.direction.intercardinalFontSize.name"),
        LocalizeString::get("client.hudmodule.direction.intercardinalFontSize.desc"), intercardinalFontSize,
        FloatValue(8.f), FloatValue(48.f), FloatValue(1.f), "showIntercardinals"_istrue);
    addSetting("intercardinalCol", LocalizeString::get("client.hudmodule.direction.intercardinalColor.name"),
               LocalizeString::get("client.hudmodule.direction.intercardinalColor.desc"), intercardinalColor,
               "showIntercardinals"_istrue);
    labelPos.replaceEntries({ { AnchorBottom, LocalizeString::get("client.hudmodule.direction.posBottom.name") },
                              { AnchorCenter, LocalizeString::get("client.hudmodule.direction.posCenter.name") },
                              { AnchorTop, LocalizeString::get("client.hudmodule.direction.posTop.name") } },
                            2);
    addEnumSetting("labelPos", LocalizeString::get("client.hudmodule.direction.labelPos.name"),
                   LocalizeString::get("client.hudmodule.direction.labelPos.desc"), this->labelPos, "showLabels"_istrue)
        ->defaultValue = EnumValue(AnchorTop);
    addSliderSetting("labelFontSize", LocalizeString::get("client.hudmodule.direction.labelFontSize.name"),
                     LocalizeString::get("client.hudmodule.direction.labelFontSize.desc"), labelFontSize,
                     FloatValue(8.f), FloatValue(48.f), FloatValue(1.f), "showLabels"_istrue);
    addSliderSetting("labelOffset", LocalizeString::get("client.hudmodule.direction.labelOffset.name"),
                     LocalizeString::get("client.hudmodule.direction.labelOffset.desc"), labelOffset,
                     FloatValue(OFFSET_MIN), FloatValue(OFFSET_MAX), FloatValue(1.f), "showLabels"_istrue);
    addSetting("labelCol", LocalizeString::get("client.hudmodule.direction.textColor.name"),
               LocalizeString::get("client.hudmodule.direction.textColor.desc"), textColor, "showLabels"_istrue);

    // Degrees (heading)
    addSetting("showHeading", LocalizeString::get("client.hudmodule.direction.showHeading.name"),
               LocalizeString::get("client.hudmodule.direction.showHeading.desc"), showHeading);
    degPos.replaceEntries({ { AnchorBottom, LocalizeString::get("client.hudmodule.direction.posBottom.name") },
                            { AnchorCenter, LocalizeString::get("client.hudmodule.direction.posCenter.name") },
                            { AnchorTop, LocalizeString::get("client.hudmodule.direction.posTop.name") } },
                          2);
    addEnumSetting("degPos", LocalizeString::get("client.hudmodule.direction.degPos.name"),
                   LocalizeString::get("client.hudmodule.direction.degPos.desc"), this->degPos, "showHeading"_istrue)
        ->defaultValue = EnumValue(AnchorTop);
    addSliderSetting("headingFontSize", LocalizeString::get("client.hudmodule.direction.headingFontSize.name"),
                     LocalizeString::get("client.hudmodule.direction.headingFontSize.desc"), headingFontSize,
                     FloatValue(8.f), FloatValue(48.f), FloatValue(1.f), "showHeading"_istrue);
    addSliderSetting("headingOffset", LocalizeString::get("client.hudmodule.direction.headingOffset.name"),
                     LocalizeString::get("client.hudmodule.direction.headingOffset.desc"), headingOffset,
                     FloatValue(OFFSET_MIN), FloatValue(OFFSET_MAX), FloatValue(1.f), "showHeading"_istrue);
    addSetting("headingCol", LocalizeString::get("client.hudmodule.direction.headingColor.name"),
               LocalizeString::get("client.hudmodule.direction.headingColor.desc"), headingColor, "showHeading"_istrue);

    // Marker
    addSetting("showMarker", LocalizeString::get("client.hudmodule.direction.showMarker.name"),
               LocalizeString::get("client.hudmodule.direction.showMarker.desc"), showMarker);
    addSetting("markerCol", LocalizeString::get("client.hudmodule.direction.markerColor.name"),
               LocalizeString::get("client.hudmodule.direction.markerColor.desc"), markerColor, "showMarker"_istrue);
    addSliderSetting("markerWidth", LocalizeString::get("client.hudmodule.direction.markerWidth.name"),
                     LocalizeString::get("client.hudmodule.direction.markerWidth.desc"), markerWidth, FloatValue(0.5f),
                     FloatValue(10.f), FloatValue(0.5f), "showMarker"_istrue);

    // Appearance
    addSetting("fadeEdges", LocalizeString::get("client.hudmodule.direction.fadeEdges.name"),
               LocalizeString::get("client.hudmodule.direction.fadeEdges.desc"), fadeEdges);
    addSetting("fillBg", LocalizeString::get("client.textmodule.props.background.name"), L"", fillBg);
    addSetting("bgColor", LocalizeString::get("client.textmodule.props.bgColor.name"), L"", bgColor, "fillBg"_istrue);
    addSliderSetting("bgRadius", LocalizeString::get("client.textmodule.props.radius.name"), L"", bgRadius,
                     FloatValue(0.f), FloatValue(10.f), FloatValue(0.5f), "fillBg"_istrue);

    rect = { 0.f, 0.f, std::get<FloatValue>(compassW), std::get<FloatValue>(compassH) };
}

float DirectionHud::wrapDegrees(float angle) {
    float result = std::fmod(angle, 360.f);
    if (result >= 180.f) result -= 360.f;
    if (result < -180.f) result += 360.f;
    return result;
}

float DirectionHud::bearingOf(float yaw) {
    float bearing = std::fmod(yaw + 180.f, 360.f);
    if (bearing < 0.f) bearing += 360.f;
    return bearing;
}

// Returns the vertical zone (top/middle/bottom third) for the given anchor
void DirectionHud::zoneRect(int anchor, float h, float& top, float& bot) {
    float third = h / 3.f;
    switch (anchor) {
    case AnchorTop:
        top = 0.f;
        bot = third;
        break;
    case AnchorBottom:
        top = third * 2.f;
        bot = h;
        break;
    case AnchorCenter:
    default:
        top = third;
        bot = third * 2.f;
        break;
    }
}

void DirectionHud::render(DrawUtil& dc, bool, bool inEditor) {
    float w = std::get<FloatValue>(compassW);
    float h = std::get<FloatValue>(compassH);

    // Background
    if (std::get<BoolValue>(fillBg)) {
        float radius = (std::get<FloatValue>(bgRadius).value / 10.f) * (std::min(w, h) / 2.f);
        dc.fillRoundedRectangle({ 0.f, 0.f, w, h }, std::get<ColorValue>(bgColor).getMainColor(), radius);
    }

    float yaw = 22.5f;
    if (!inEditor) {
        auto lp = SDK::ClientInstance::get()->getLocalPlayer();
        if (lp) yaw = lp->getRot().y;
    }
    float bearing = bearingOf(yaw);

    d2d::Color labelCol = std::get<ColorValue>(textColor).getMainColor();
    d2d::Color intercardinalCol = std::get<ColorValue>(intercardinalColor).getMainColor();
    d2d::Color headingCol = std::get<ColorValue>(headingColor).getMainColor();
    d2d::Color mkrCol = std::get<ColorValue>(markerColor).getMainColor();
    d2d::Color tckCol = std::get<ColorValue>(tickColor).getMainColor();

    float halfSpan = std::get<FloatValue>(span) / 2.f;
    float pixelsPerDegree = w / std::get<FloatValue>(span);
    float centerX = w / 2.f;

    int tickAnchorKey = tickAnchor.getSelectedKey();
    float tickHeightPx = h * std::get<FloatValue>(tickHeightRatio);
    float minorTickHeightPx = h * std::get<FloatValue>(minorTickHeightRatio);
    float tickWidthPx = std::get<FloatValue>(tickWidth);
    float minorTickWidthPx = std::get<FloatValue>(minorTickWidth);
    int minorTickCount = std::clamp(static_cast<int>(std::get<FloatValue>(minorTicksPerMajor)), 0, 8);
    float tickOffsetPx = std::get<FloatValue>(tickOffset);

    auto drawTick = [&](float bearingDeg, float height, float width) {
        float delta = wrapDegrees(bearingDeg - bearing);
        if (std::abs(delta) > halfSpan || height <= 0.001f) return;
        float tickTop, tickBot;
        switch (tickAnchorKey) {
        case AnchorBottom:
            tickBot = h;
            tickTop = h - height;
            break;
        case AnchorTop:
            tickTop = 0.f;
            tickBot = height;
            break;
        case AnchorCenter:
        default:
            float midY = h * 0.5f;
            tickTop = midY - height * 0.5f;
            tickBot = midY + height * 0.5f;
            break;
        }
        tickTop += tickOffsetPx;
        tickBot += tickOffsetPx;

        float tickX = centerX + delta * pixelsPerDegree;
        float alpha = fadedAlpha(delta, halfSpan);
        d2d::Color col = tckCol.asAlpha(tckCol.a * alpha);
        dc.fillRectangle({ tickX - width * 0.5f, tickTop, tickX + width * 0.5f, tickBot }, col);
    };

    if (std::get<BoolValue>(showTicks)) {
        int steps = minorTickCount + 1;
        for (int tickDeg = 0; tickDeg < 360; tickDeg += MAJOR_INTERVAL) {
            drawTick(static_cast<float>(tickDeg), tickHeightPx, tickWidthPx);
            for (int step = 1; step < steps; ++step) {
                float minorDeg = tickDeg + static_cast<float>(MAJOR_INTERVAL) * step / steps;
                drawTick(std::fmod(minorDeg, 360.f), minorTickHeightPx, minorTickWidthPx);
            }
        }
    }

    if (std::get<BoolValue>(showMarker)) {
        float markerHalfWidth = std::get<FloatValue>(markerWidth) * 0.5f;
        dc.fillRectangle({ centerX - markerHalfWidth, 0.f, centerX + markerHalfWidth, h }, mkrCol);
    }

    struct DirEntry {
        float bearing;
        const char* key;
        bool cardinal;
    };
    static const DirEntry dirs[] = {
        { 0.f, "client.hudmodule.direction.north", true },   { 45.f, "client.hudmodule.direction.northEast", false },
        { 90.f, "client.hudmodule.direction.east", true },   { 135.f, "client.hudmodule.direction.southEast", false },
        { 180.f, "client.hudmodule.direction.south", true }, { 225.f, "client.hudmodule.direction.southWest", false },
        { 270.f, "client.hudmodule.direction.west", true },  { 315.f, "client.hudmodule.direction.northWest", false },
    };

    float labelWidth = LABEL_AREA_W;
    float labelTop, labelBot;
    zoneRect(labelPos.getSelectedKey(), h, labelTop, labelBot);
    labelTop += std::get<FloatValue>(labelOffset);
    labelBot += std::get<FloatValue>(labelOffset);
    float labelFont = std::get<FloatValue>(labelFontSize);
    float intercardinalFont = std::get<FloatValue>(intercardinalFontSize);
    bool showLabelsOn = std::get<BoolValue>(showLabels);
    bool showInter = std::get<BoolValue>(showIntercardinals);

    if (showLabelsOn) {
        for (auto& d : dirs) {
            if (!showInter && !d.cardinal) continue;

            float delta = wrapDegrees(d.bearing - bearing);
            if (std::abs(delta) > halfSpan) continue;

            float alpha = fadedAlpha(delta, halfSpan);
            d2d::Color baseCol = d.cardinal ? labelCol : intercardinalCol;
            d2d::Color col = baseCol.asAlpha(baseCol.a * alpha);
            float fontSize = d.cardinal ? labelFont : intercardinalFont;
            float labelX = centerX + delta * pixelsPerDegree - labelWidth / 2.f;
            d2d::Rect labelRect = { labelX, labelTop, labelX + labelWidth, labelBot };
            dc.drawText(labelRect, LocalizeString::get(d.key).value(), col, Renderer::FontSelection::PrimaryLight,
                        fontSize, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    if (std::get<BoolValue>(showHeading)) {
        int deg = static_cast<int>(bearing) % 360;
        if (deg < 0) deg += 360;
        std::wstring headingText = std::to_wstring(deg) + L"\u00B0";
        float headingTop, headingBot;
        zoneRect(degPos.getSelectedKey(), h, headingTop, headingBot);
        headingTop += std::get<FloatValue>(headingOffset);
        headingBot += std::get<FloatValue>(headingOffset);
        d2d::Rect headingRect = { 0.f, headingTop, w, headingBot };
        dc.drawText(headingRect, headingText, headingCol, Renderer::FontSelection::PrimaryLight,
                    std::get<FloatValue>(headingFontSize), DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    rect.right = rect.left + w;
    rect.bottom = rect.top + h;
}

float DirectionHud::fadedAlpha(float delta, float halfSpan) {
    if (!std::get<BoolValue>(fadeEdges) || halfSpan <= 0.f) return 1.f;
    float distance = std::abs(delta) / halfSpan;
    if (distance <= FADE_START) return 1.f;
    return std::max(0.f, 1.f - (distance - FADE_START) / (1.f - FADE_START));
}
