#include "pch.h"
#include "WaypointPopupScreen.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/ClickEvent.h"

#include "client/feature/module/impl/hud/Waypoints.h"

WaypointPopupScreen::WaypointPopupScreen() {
    Eventing::get().listen<RenderOverlayEvent>(this, static_cast<EventListenerFunc>(&WaypointPopupScreen::onRender));
    Eventing::get().listen<ClickEvent>(this, static_cast<EventListenerFunc>(&WaypointPopupScreen::onClick));
    Eventing::get().listen<KeyUpdateEvent>(this, static_cast<EventListenerFunc>(&WaypointPopupScreen::onKey));

    formFields = {
        { L"Name:", &nameTextBox },
        { L"Initials:", &initialsTextBox },
        { L"X:", &xTextBox },
        { L"Y:", &yTextBox },
        { L"Z:", &zTextBox }
    };

    for (FormField const& field : formFields) {
        Latite::get().addTextBox(field.textBox);
    }

    // TODO: fix this silliness with something a bit more elegant
    Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
        if (mod->name() == "Waypoints") {
            this->parentModule = static_cast<Waypoints*>(mod.get());
        }
    });
}

void WaypointPopupScreen::onEnable(bool ignoreAnims) {
    if (ignoreAnims) blurAnim = 1.f;
    else blurAnim = 0.f;
    mouseButtons = {};
    justClicked = {};

    if (auto player = SDK::ClientInstance::get()->getLocalPlayer()) {
        Vec3 pos = player->getPos();
        xTextBox.setText(std::to_wstring(static_cast<int>(pos.x)));
        // very hacky fix to get vanilla y coordinate (probably not 100% accurate)
        yTextBox.setText(std::to_wstring(lroundf(pos.y - 1.62f)));
        zTextBox.setText(std::to_wstring(static_cast<int>(pos.z)));
    }

    nameTextBox.setText(L"New Waypoint");
    initialsTextBox.setText(L"N");
    SDK::ClientInstance::get()->releaseCursor();
}

void WaypointPopupScreen::onDisable() {
    for (FormField const& field : formFields) {
        field.textBox->setSelected(false);
        field.textBox->reset();
    }
}

void WaypointPopupScreen::onKey(Event& evG) {
    KeyUpdateEvent& ev = reinterpret_cast<KeyUpdateEvent&>(evG);
    ev.setCancelled(true);

    if (ev.isDown() && ev.getKey() == VK_ESCAPE) close();
}

void WaypointPopupScreen::onClick(Event& evG) {
    ClickEvent& ev = reinterpret_cast<ClickEvent&>(evG);
    if (ev.getMouseButton() > 0) {
        ev.setCancelled(true);
    }
}

void WaypointPopupScreen::onRender(Event& evG) {
    if (!isActive()) justClicked = { false, false, false };
    D2DUtil dc;

    if (isActive()) {
        float alpha = Latite::getRenderer().getDeltaTime() / 10.f;
        blurAnim = std::lerp(blurAnim, 1.f, alpha);

        float toBlur = Latite::get().getMenuBlur().value_or(0.f);
        if (Latite::get().getMenuBlur()) dc.drawGaussianBlur(toBlur * blurAnim);
    }

    D2D1_SIZE_F ss = Latite::getRenderer().getScreenSize();

    float popupWidth = 400.f;
    float popupHeight = 390.f;

    popupRect = {
        (ss.width - popupWidth) / 2.f,
        (ss.height - popupHeight) / 2.f,
        (ss.width + popupWidth) / 2.f,
        (ss.height + popupHeight) / 2.f
    };

    dc.fillRoundedRectangle(popupRect, d2d::Color::RGB(7, 7, 7).asAlpha(0.85f), 10.f);
    dc.drawRoundedRectangle(popupRect, d2d::Color::RGB(0, 0, 0).asAlpha(0.28f), 10.f, 2.f,
                            DrawUtil::OutlinePosition::Outside);

    float formFieldsTopY = popupRect.top + 60.f;

    float fieldHeight = 25.f;
    float padding = 20.f;
    float labelWidth = 60.f;
    float spacing = 15.f;

    d2d::Rect titleRect = { popupRect.left, popupRect.top, popupRect.right, formFieldsTopY };
    dc.drawText(titleRect, L"Add Waypoint", d2d::Colors::WHITE, Renderer::FontSelection::PrimaryLight, 22.f,
                DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    float inputWidth = popupRect.getWidth() - (padding * 2) - labelWidth;
    Vec2 currentPos = { popupRect.left + padding, formFieldsTopY };

    d2d::Color labelColor = d2d::Color(1.f, 1.f, 1.f, 0.8f);
    d2d::Color fieldBgColor = d2d::Color::RGB(32, 32, 32).asAlpha(0.5f);
    d2d::Color fieldTextColor = d2d::Colors::WHITE;

    for (FormField& field : formFields) {
        d2d::Rect labelRect = { currentPos.x, currentPos.y, currentPos.x + labelWidth, currentPos.y + fieldHeight };
        field.inputRect = { labelRect.right, currentPos.y, labelRect.right + inputWidth, currentPos.y + fieldHeight };

        dc.drawText(labelRect, field.label, labelColor, Renderer::FontSelection::PrimaryRegular, 16.f,
                    DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        field.textBox->setRect(field.inputRect);
        field.textBox->render(dc, 5.f, fieldBgColor, fieldTextColor);

        currentPos.y += fieldHeight + spacing;
    }

    currentPos.y += 5;
    d2d::Rect colorLabelRect = { currentPos.x, currentPos.y, currentPos.x + labelWidth, currentPos.y + fieldHeight };
    dc.drawText(colorLabelRect, L"Color:", labelColor, Renderer::FontSelection::PrimaryRegular, 16.f,
        DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    float swatchAreaX = colorLabelRect.right;
    float swatchAreaY = currentPos.y;

    std::vector<std::pair<d2d::Color, d2d::Rect>> colorSwatches;

    for (int i = 0; i < waypointColors.size(); ++i) {
        int swatchesPerRow = 8;
        float swatchSpacing = 6.f;
        float swatchSize = 22.f;
        int row = i / swatchesPerRow;
        int col = i % swatchesPerRow;
        d2d::Color color = waypointColors[i];

        d2d::Rect swatchRect = {
            swatchAreaX + col * (swatchSize + swatchSpacing),
            swatchAreaY + row * (swatchSize + swatchSpacing),
            swatchAreaX + col * (swatchSize + swatchSpacing) + swatchSize,
            swatchAreaY + row * (swatchSize + swatchSpacing) + swatchSize
        };

        dc.fillRectangle(swatchRect, color);
        if (color.getHex() == selectedColor.getHex()) {
            dc.drawRectangle(swatchRect, d2d::Colors::WHITE, 2.f);
        }
        colorSwatches.push_back({ color, swatchRect });
    }

    float buttonWidth = (popupRect.getWidth() - (padding * 2) - spacing) / 2.f;
    float buttonHeight = 35.f;

    d2d::Rect cancelBtnRect = {
        popupRect.left + padding, popupRect.bottom - padding - buttonHeight, popupRect.left + padding + buttonWidth,
        popupRect.bottom - padding
    };
    d2d::Rect createBtnRect = {
        cancelBtnRect.right + spacing, cancelBtnRect.top, cancelBtnRect.right + spacing + buttonWidth,
        cancelBtnRect.bottom
    };

    dc.fillRoundedRectangle(cancelBtnRect, d2d::Color::RGB(251, 54, 54).asAlpha(0.8f), 5.f);
    dc.drawText(cancelBtnRect, L"Cancel", fieldTextColor, Renderer::FontSelection::PrimaryRegular, 16.f,
                DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    dc.fillRoundedRectangle(createBtnRect, d2d::Color(Latite::get().getAccentColor().getMainColor()).asAlpha(0.8f),
                            5.f);
    dc.drawText(createBtnRect, L"Create", fieldTextColor, Renderer::FontSelection::PrimaryRegular, 16.f,
                DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    if (this->justClicked[0]) {
        Vec2& cursorPos = SDK::ClientInstance::get()->cursorPos;

        for (FormField const& field : formFields) {
            field.textBox->setSelected(false);
        }

        bool elementClicked = false;
        for (FormField const& field : formFields) {
            if (field.inputRect.contains(cursorPos)) {
                playClickSound();
                field.textBox->setSelected(true);
                field.textBox->setCaretLocation(static_cast<int>(field.textBox->getText().size()));
                elementClicked = true;
                break;
            }
        }

        if (!elementClicked) {
            for (const auto& swatch : colorSwatches) {
                if (swatch.second.contains(cursorPos)) {
                    selectedColor = swatch.first;
                    elementClicked = true;
                    playClickSound();
                    break;
                }
            }

            if (cancelBtnRect.contains(cursorPos)) {
                playClickSound();
                close();
            } else if (createBtnRect.contains(cursorPos)) {
                try {
                    WaypointData newWaypoint;
                    newWaypoint.name = nameTextBox.getText();
                    newWaypoint.initials = initialsTextBox.getText();
                    newWaypoint.color = selectedColor;
                    newWaypoint.position.x = std::stof(xTextBox.getText());
                    newWaypoint.position.y = std::stof(yTextBox.getText());
                    newWaypoint.position.z = std::stof(zTextBox.getText());

                    if (auto player = SDK::ClientInstance::get()->getLocalPlayer(); player && player->dimension) {
                        newWaypoint.dimension = util::StrToWStr(player->dimension->dimensionName);
                    }
                    parentModule->addWaypoint(newWaypoint);
                }
                catch (...) {
                    // TODO: handle this in some non scuffed way
                }
                playClickSound();
                close();
            }
        }
    }
}
