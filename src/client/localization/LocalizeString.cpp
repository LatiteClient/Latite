#include "pch.h"
#include "LocalizeString.h"

LocalizedString LocalizeString::get(const std::string& id) {
    return { id, Latite::get().getL10nData().get(id) };
}
