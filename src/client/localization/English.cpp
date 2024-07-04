#include "pch.h"
#include "English.h"

// {} is a placeholder for a variable, similar to std::format (but I had to implement it myself)

English::English() {
    English::strings = {
        { "intro.welcome", L"Welcome to Latite Client!" },
        { "intro.menubutton", L"Press {} in a world or server to open the mod menu." },

        { "client.module.enabled.name", L"Enabled" },
        { "client.module.enabled.desc", L"Whether the module is on or not" },
        { "client.module.key.name", L"Key" },
        { "client.module.key.desc", L"The keybind of the module" },
        { "client.hudmodule.pos.name", L"Position" },
        { "client.hudmodule.snapX.name", L"Snap X" },
        { "client.hudmodule.snapY.name", L"Snap Y" },
        { "client.hudmodule.size.name", L"Size" },
        { "client.hudmodule.forceMcRend.name", L"Force Minecraft Renderer" },
        { "client.hudmodule.forceMcRend.desc", L"Whether or not to use the Minecraft renderer for this module." },

        { "client.module.behindYou.name", L"Behind You!" },
        { "client.module.behindYou.desc", L"Bind a perspective to a key." },
        { "client.module.freelook.name", L"Freelook" },
        { "client.module.freelook.desc", L"Look around freely." },

        // todo: check if this displays diacritics correctly (or even works at all)
        { "client.commands.config.desc", L"Perform actions related to profiles/configs" },
        { "client.commands.config.savedConfig", L"Saved current config!" },
        { "client.commands.config.savedConfigPath", L"Successfully saved config to &7{}.json" },
        { "client.commands.config.saveDuringLoadingError", L"Could not save current config before loading!" },
        { "client.commands.config.configNotFound", L"Could not find config &7{}" },
        { "client.commands.config.loadedConfig", L"Successfully loaded config &7{}" },
        { "client.commands.config.genericError", L"Something went wrong!" },
    };
}

std::wstring English::getString(std::string id) {
    return English::strings.at(id);
}