#pragma once

#include "../Screen.h"
#include "client/ui/TextBox.h"

class Waypoints;
struct WaypointData;

struct FormField {
    std::wstring label;
    ui::TextBox* textBox;
    d2d::Rect inputRect;
};

class WaypointPopupScreen : public Screen {
public:
    WaypointPopupScreen();

    std::string getName() override { return "WaypointPopupScreen"; }
    void onRender(Event& evG);
    void onKey(Event& evG);
    void onClick(Event& evG);

protected:
    void onEnable(bool ignoreAnims) override;
    void onDisable() override;

private:
    // TODO: waypoint manager
    Waypoints* parentModule;

    d2d::Rect popupRect = {};
    ui::TextBox nameTextBox = {};
    ui::TextBox initialsTextBox = { { 0, 0, 0, 0 }, 2 };
    ui::TextBox xTextBox = {};
    ui::TextBox yTextBox = {};
    ui::TextBox zTextBox = {};

    std::vector<FormField> formFields;
    d2d::Color selectedColor;

    float blurAnim = 0.f;

    const std::vector<d2d::Color> waypointColors = {
        // White
        d2d::Color::Hex("FFFFFF"),
        // Gray
        d2d::Color::Hex("A8A8A8"),
        // Dark Gray
        d2d::Color::Hex("545454"),
        // Black
        d2d::Color::Hex("111111"),

        // Yellow
        d2d::Color::Hex("ECEC4E"),
        // Gold
        d2d::Color::Hex("FCA800"),

        // Green
        d2d::Color::Hex("54FC54"),
        // Dark Green
        d2d::Color::Hex("00A800"),

        // Aqua
        d2d::Color::Hex("54FCFC"),
        // Dark Aqua
        d2d::Color::Hex("00A8A8"),

        // Blue
        d2d::Color::Hex("5454FC"),
        // Dark Blue
        d2d::Color::Hex("0000A8"),

        // Light Purple
        d2d::Color::Hex("FC54FC"),
        // Dark Purple
        d2d::Color::Hex("A800A8"),

        // Red
        d2d::Color::Hex("FC5454"),
        // Dark Red
        d2d::Color::Hex("A80000"),
    };
};
