#include "pch.h"
#include "LocalizeString.h"

#include "English.h"
#include "Spanish.h"


std::wstring LocalizeString::get(std::string id) {
    English english;
    Spanish spanish;

    std::wstring language = Latite::get().getClientLanguage();
    // all this just to make the language string lowercase lol
    std::transform(language.begin(), language.end(), language.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (language == L"english")
        return english.getString(id);
    if (language == L"spanish")
        return spanish.getString(id);
    return english.getString(id);
}
