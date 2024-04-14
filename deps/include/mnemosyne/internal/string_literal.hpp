#pragma once
#ifndef MNEMOSYNE_INTERNAL_STRING_LITERAL_HPP
#define MNEMOSYNE_INTERNAL_STRING_LITERAL_HPP

#include <algorithm>
#include <string_view>

namespace mnem::internal {
    template <size_t N>
    struct string_literal {
        constexpr string_literal(const char(&str)[N]) { // NOLINT(google-explicit-constructor)
            std::copy_n(str, N, data);
        }

        [[nodiscard]] constexpr std::string_view stringview() const {
            return { data, N - 1 };
        }

        char data[N]{};
    };
}

#endif
