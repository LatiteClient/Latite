#include "pch.h"
#include "English.h"

// {} is a placeholder for a variable, similar to std::format (but I had to implement it myself)

English::English() {
    English::strings = {
        { "intro.welcome", L"Welcome to Latite Client!" },
        { "intro.menubutton", L"Press {} in a world or server to open the mod menu." },

        { "client.module.enabled.name", L"Enabled" },
        { "client.module.key.name", L"Key" },
        { "client.module.enabled.desc", L"Whether the module is on or not" },
        { "client.module.key.desc", L"The keybind of the module" },

        { "client.hudmodule.pos.name", L"Position" },
        { "client.hudmodule.snapX.name", L"Snap X" },
        { "client.hudmodule.snapY.name", L"Snap Y" },
        { "client.hudmodule.size.name", L"Size" },
        { "client.hudmodule.forceMcRend.name", L"Force Minecraft Renderer" },
        { "client.hudmodule.forceMcRend.desc", L"Whether or not to use the Minecraft renderer for this module." },

        { "client.module.behindYou.name", L"Behind You!" },
        { "client.module.behindYouState0.name", L"Third Person Front" },
        { "client.module.behindYouState1.name", L"Third Person Back" },
        { "client.module.behindYouState2.name", L"First Person" },
        { "client.module.behindYou.perspective.name", L"Perspective" },
        { "client.module.behindYou.bind.name", L"Bind" },
        { "client.module.behindYou.desc", L"Bind a perspective to a key." },
        { "client.module.behindYou.bind.desc", L"What to bind the perspective to" },
        { "client.module.behindYou.perspective.desc", L"The perspective" },

        { "client.module.freelook.name", L"Freelook" },
        { "client.module.freelook.desc", L"Look around freely." },

        { "client.module.thirdPersonNametag.name", L"Third Person Nametag" },
        { "client.module.thirdPersonNametag.desc", L"Shows your nametag in third person view." },

        { "client.module.environmentChanger.name", L"Environment Changer" },
        { "client.module.environmentChanger.desc", L"Changes visual features in the environment." },
        { "client.module.environmentChanger.setFogColor.name", L"Set Fog Color" },
        { "client.module.environmentChanger.fogColor.name", L"Fog Color" },
        { "client.module.environmentChanger.setTime.name", L"Set Time" },
        { "client.module.environmentChanger.timeToSet.name", L"Time" },
        { "client.module.environmentChanger.showWeather.name", L"Show Weather" },
        { "client.module.environmentChanger.fogColor.desc", L"The new fog color" },
        { "client.module.environmentChanger.setTime.desc", L"Whether or not to set the time" },
        { "client.module.environmentChanger.timeToSet.desc", L"The new time of day to set" },
        { "client.module.environmentChanger.showWeather.desc", L"Whether or not to show the weather" },

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