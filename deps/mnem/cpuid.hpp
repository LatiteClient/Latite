#pragma once
#include <string_view>

namespace mnem::internal {
    struct cpuinfo {
        static std::string_view vendor();
        static std::string_view brand();
        static bool sse4_2();
        static bool avx2();
        static bool bmi1();
    };
}