#pragma once

#include <memory>
#include <string>

class Localization;

namespace SDK {
    class I18n {
    public:
        static I18n* get();

        std::string get(std::string const& key);
    };
}
