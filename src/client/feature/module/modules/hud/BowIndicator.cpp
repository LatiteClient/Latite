#include "pch.h"
#include "BowIndicator.h"

BowIndicator::BowIndicator()
    : TextModule("BowIndicator", LocalizeString::get("client.textmodule.bowIndicator.name"),
                 LocalizeString::get("client.textmodule.bowIndicator.desc"), HUD, 400.f, 0, true) {
    addSetting("visual", LocalizeString::get("client.textmodule.bowIndicator.visual.name"),
               LocalizeString::get("client.textmodule.bowIndicator.visual.desc"), this->visual);
    addSetting("fgCol", LocalizeString::get("client.textmodule.bowIndicator.fgCol.name"),
               LocalizeString::get("client.textmodule.bowIndicator.fgCol.desc"), this->indicatorCol2);
    addSetting("bgCol", LocalizeString::get("client.textmodule.bowIndicator.bgCol.name"),
               LocalizeString::get("client.textmodule.bowIndicator.bgCol.desc"), this->indicatorCol);
    addSetting("horizontal", LocalizeString::get("client.textmodule.bowIndicator.horizontal.name"),
               LocalizeString::get("client.textmodule.bowIndicator.horizontal.desc"), this->horizontal,
               "visual"_istrue);
    addSetting("hideWhenCharged", LocalizeString::get("client.textmodule.bowIndicator.hideWhenCharged.name"),
               LocalizeString::get("client.textmodule.bowIndicator.hideWhenCharged.desc"), this->hideWhenCharged,
               "visual"_istrue);

    addSliderSetting("size", LocalizeString::get("client.textmodule.bowIndicator.size.name"),
                     LocalizeString::get("client.textmodule.bowIndicator.size.desc"), indicatorSize, FloatValue(0.f),
                     FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
    addSliderSetting("width", LocalizeString::get("client.textmodule.bowIndicator.width.name"),
                     LocalizeString::get("client.textmodule.bowIndicator.width.desc"), indicatorWidth, FloatValue(0.f),
                     FloatValue(200.f), FloatValue(2.5f), "visual"_istrue);
    addSliderSetting("rad", LocalizeString::get("client.textmodule.bowIndicator.rad.name"),
                     LocalizeString::get("client.textmodule.bowIndicator.rad.desc"), indicatorRad, FloatValue(0.f),
                     FloatValue(5.f), FloatValue(1.f), "visual"_istrue);
    addSliderSetting("padding", LocalizeString::get("client.textmodule.bowIndicator.padding.name"),
                     LocalizeString::get("client.textmodule.bowIndicator.padding.desc"), padding, FloatValue(0.f),
                     FloatValue(20.f), FloatValue(1.f), "visual"_istrue);
}

BowIndicator::~BowIndicator() {
}

void BowIndicator::render(DrawUtil& dc, bool isDefault, bool inEditor) {
    if (!std::get<BoolValue>(visual)) {
        TextModule::render(dc, isDefault, inEditor);
        return;
    }
    auto* clientInstance = SDK::ClientInstance::get();
    if (!clientInstance) return;

    auto* player = clientInstance->getLocalPlayer();
    if (!player || !player->supplies || !player->supplies->inventory) return;

    auto* slot = player->supplies->inventory->getItem(player->supplies->selectedSlot);

    bool horiz = std::get<BoolValue>(horizontal);
    float wid = std::get<FloatValue>(indicatorWidth);
    float siz = std::get<FloatValue>(indicatorSize);

    d2d::Rect rc = { 0.f, 0.f, horiz ? siz : wid, horiz ? wid : siz };
    float rad = std::get<FloatValue>(indicatorRad) / 10.f * (std::min)(rc.getWidth(), rc.getHeight());

    rect.right = rect.left + rc.getWidth();
    rect.bottom = rect.top + rc.getHeight();

    if (auto percent = getBowCharge(slot)) {
        if (std::get<BoolValue>(hideWhenCharged) && percent > 0.95f) {
            return;
        }

        dc.fillRoundedRectangle(rc, std::get<ColorValue>(indicatorCol).getMainColor(), rad);

        d2d::Rect fillRc = rc;

        float pad = std::get<FloatValue>(padding);
        fillRc.left += pad;
        fillRc.top += pad;
        fillRc.right -= pad;
        fillRc.bottom -= pad;

        if (horiz) {
            fillRc.right = fillRc.left + fillRc.getWidth() * percent.value();
        } else {
            fillRc.top = fillRc.bottom - fillRc.getHeight() * percent.value();
        }
        dc.fillRoundedRectangle(fillRc, std::get<ColorValue>(indicatorCol2).getMainColor(), rad);
    }
}

std::wstringstream BowIndicator::text(bool, bool) {
    std::wstringstream wss;
    auto* clientInstance = SDK::ClientInstance::get();
    if (!clientInstance) return wss;

    auto* player = clientInstance->getLocalPlayer();
    if (!player || !player->supplies || !player->supplies->inventory) return wss;

    auto* slot = player->supplies->inventory->getItem(player->supplies->selectedSlot);
    const auto charge = getBowCharge(slot);
    wss << std::round(charge.value_or(0.f) * 100.f) << "%";
    return wss;
}

std::optional<float> BowIndicator::getBowCharge(SDK::ItemStack* slot) {
    if (!slot || !slot->item) return std::nullopt;

    auto* item = *slot->item;
    if (!item) return std::nullopt;

    const auto itemId = item->id.hash;

    const bool isSupportedItem = itemId == "bow"_fnv64 || itemId == "crossbow"_fnv64 || itemId == "trident"_fnv64;
    if (!isSupportedItem) return std::nullopt;

    auto* clientInstance = SDK::ClientInstance::get();
    if (!clientInstance) return std::nullopt;

    auto* player = clientInstance->getLocalPlayer();
    if (!player) return std::nullopt;

    const int remainingUseTicks = player->getItemUseDuration();
    if (remainingUseTicks == 0) return std::nullopt;

    const int maxUseTicks = item->getMaxUseDuration(slot);

    float fullChargeTicks = 20.f;
    if (itemId == "crossbow"_fnv64) {
        // CrossbowItem accounts for the Quick Charge level in its max-use duration.
        fullChargeTicks = static_cast<float>(maxUseTicks);
    } else if (itemId == "trident"_fnv64) {
        fullChargeTicks = 10.f;
    }

    const float elapsedUseTicks = static_cast<float>(maxUseTicks - remainingUseTicks);
    return std::clamp(elapsedUseTicks / fullChargeTicks, 0.f, 1.f);
}
