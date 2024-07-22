#include "pch.h"
#include "Spanish.h"

// {} is a placeholder for a variable, similar to std::format (but I had to implement it myself).
// When using {}, the LocalizeString::get call must be inside a util::FormatWString call to actually format it.

// & is a placeholder for the format character Minecraft uses
// When using &, the LocalizeString::get call must be inside a util::WFormat call to actually format it.

Spanish::Spanish() {
    Spanish::strings = {
        { "client.intro.welcome", L"¡Bienvenidos a Latite Client!" },
        { "client.intro.menubutton", L"Apretón {} en un mundo o servidor abrir el menú de mods" }
    };
}

std::wstring Spanish::getString(std::string id) {
    return Spanish::strings.at(id);
}