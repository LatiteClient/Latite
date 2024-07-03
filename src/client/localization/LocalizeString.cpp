#include "pch.h"
#include "LocalizeString.h"

#include "English.h"
#include "Spanish.h"


std::wstring LocalizeString::get(std::string id) {
    English english;
    Spanish spanish;

    std::string language = util::WStrToStr(Latite::get().getClientLanguage());
    // all this just to make the language string lowercase lol
    std::transform(language.begin(), language.end(), language.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (language == "english")
        return english.getString(id);
    if (language == "spanish")
        return spanish.getString(id);
    return english.getString(id);
}
