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

    // TODO: custom colors
    const std::vector<d2d::Color> waypointColors = {
        // White
        d2d::Color::Hex("FFFFFF"),
        // Bright Yellow
        d2d::Color::Hex("FFFF00"),
        // Cyan
        d2d::Color::Hex("00FFFF"),
        // Lime Green
        d2d::Color::Hex("00FF00"),
        // Magenta
        d2d::Color::Hex("FF00FF"),
        // Orange
        d2d::Color::Hex("FFA500"),
        // Light Blue
        d2d::Color::Hex("87CEEB"),
        // Goldenrod
        d2d::Color::Hex("DAA520"),
        // Hot Pink
        d2d::Color::Hex("FF69B4"),
        // Mint Green
        d2d::Color::Hex("98FF98"),
    };
};
