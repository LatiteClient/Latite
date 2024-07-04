#include "pch.h"
#include "Spanish.h"

Spanish::Spanish() {
    Spanish::strings = {
        { L"intro.welcome", L"¡Bienvenidos a Latite Client!" },
        { L"intro.menubutton", L"Apretón " + util::StrToWStr(util::KeyToString(Latite::get().getMenuKey().value)) + L" en un mundo o servidor abrir el menú de mods" }
    };
}

std::wstring Spanish::getString(std::wstring id) {
    return Spanish::strings.at(id);
}