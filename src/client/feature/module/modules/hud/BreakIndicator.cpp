#include "pch.h"
#include "BreakIndicator.h"

BreakIndicator::BreakIndicator() : TextModule("BreakProgress",
                                              LocalizeString::get("client.textmodule.breakIndicator.name"),
                                              LocalizeString::get("client.textmodule.breakIndicator.desc"), HUD, 400.f,
                                              0, true) {
    addSetting("last", LocalizeString::get("client.textmodule.breakIndicator.lastProgress.name"),
               LocalizeString::get("client.textmodule.breakIndicator.lastProgress.desc"), this->last);
    addSetting("visual", LocalizeString::get("client.textmodule.breakIndicator.visual.name"),
               LocalizeString::get("client.textmodule.breakIndicator.visual.desc"), this->visual);
    addSetting("fgCol", LocalizeString::get("client.textmodule.breakIndicator.fgCol.name"),
               LocalizeString::get("client.textmodule.breakIndicator.fgCol.desc"), this->indicatorCol2,
               "visual"_istrue);
    addSetting("bgCol", LocalizeString::get("client.textmodule.breakIndicator.bgCol.name"),
               LocalizeString::get("client.textmodule.breakIndicator.bgCol.desc"), this->indicatorCol, "visual"_istrue);
    addSetting("horizontal", LocalizeString::get("client.textmodule.breakIndicator.horizontal.name"),
               LocalizeString::get("client.textmodule.breakIndicator.horizontal.desc"), this->horizontal,
               "visual"_istrue);

    addSliderSetting("size", LocalizeString::get("client.textmodule.breakIndicator.size.name"),
                     LocalizeString::get("client.textmodule.breakIndicator.size.desc"), indicatorSize, FloatValue(0.f),
                     FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
    addSliderSetting("width", LocalizeString::get("client.textmodule.breakIndicator.width.name"),
                     LocalizeString::get("client.textmodule.breakIndicator.width.desc"), indicatorWidth,
                     FloatValue(0.f),
                     FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
    addSliderSetting("rad", LocalizeString::get("client.textmodule.breakIndicator.rad.name"),
                     LocalizeString::get("client.textmodule.breakIndicator.rad.desc"), indicatorRad, FloatValue(0.f),
                     FloatValue(5.f), FloatValue(1.f), "visual"_istrue);
    addSliderSetting("padding", LocalizeString::get("client.textmodule.breakIndicator.padding.name"),
                     LocalizeString::get("client.textmodule.breakIndicator.padding.desc"), padding, FloatValue(0.f),
                     FloatValue(20.f), FloatValue(1.f), "visual"_istrue);
}

BreakIndicator::~BreakIndicator() {
}

void BreakIndicator::render(DrawUtil& dc, bool isDefault, bool inEditor) {
    if (!std::get<BoolValue>(visual)) {
        TextModule::render(dc, isDefault, inEditor);
        return;
    }

    auto lp = SDK::ClientInstance::get()->getLocalPlayer();
    if (!lp) return;

    float prog = std::get<BoolValue>(last) ? lp->gameMode->lastBreakProgress : lp->gameMode->breakProgress;

    if (prog == 0.f) {
        return;
    }

    bool horiz = std::get<BoolValue>(horizontal);
    float wid = std::get<FloatValue>(indicatorWidth);
    float siz = std::get<FloatValue>(indicatorSize);

    d2d::Rect rc = {0.f, 0.f, horiz ? siz : wid, horiz ? wid : siz};
    float rad = std::get<FloatValue>(indicatorRad) / 10.f * (std::min)(rc.getWidth(), rc.getHeight());

    rect.right = rect.left + rc.getWidth();
    rect.bottom = rect.top + rc.getHeight();

    dc.fillRoundedRectangle(rc, std::get<ColorValue>(indicatorCol).getMainColor(), rad);

    d2d::Rect fillRc = rc;

    float pad = std::get<FloatValue>(padding);
    fillRc.left += pad;
    fillRc.top += pad;
    fillRc.right -= pad;
    fillRc.bottom -= pad;

    if (horiz) {
        fillRc.right = fillRc.left + fillRc.getWidth() * prog;
    }
    else {
        fillRc.top = fillRc.bottom - fillRc.getHeight() * prog;
    }
    dc.fillRoundedRectangle(fillRc, std::get<ColorValue>(indicatorCol2).getMainColor(), rad);
}

std::wstringstream BreakIndicator::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    float prog = 0.f;
    auto lp = SDK::ClientInstance::get()->getLocalPlayer();
    if (lp) {
        prog = std::get<BoolValue>(last) ? lp->gameMode->lastBreakProgress : lp->gameMode->breakProgress;
    }

    wss << std::round(prog * 100.f) << L'%';
    return wss;
}
