#include "scanner_impls.hpp"
#include "../cpuid.hpp"

#include <iostream>

namespace mnem {
    scan_mode detect_scan_mode() {
        using internal::cpuinfo;

        if (cpuinfo::bmi1() && cpuinfo::avx2())
            return scan_mode::avx2;

        if (cpuinfo::sse4_2())
            return scan_mode::sse4_2;

        return scan_mode::normal;
    }
}

namespace mnem::internal {
    const std::byte* scan_impl_normal(const std::byte* begin, const std::byte* end, signature sig) {
        while (sig.back().mask() == std::byte{0}) {
            sig = sig.subsig(0, sig.size() - 1);
            end--;
            // the sig cannot be empty. like that LITERALLY cannot happen. that would be stupid. dumb even.
        }

        const auto first_elem = sig.front();
        if (first_elem.mask() == std::byte{0xFF}) {
            const auto first = first_elem.byte();

            // Take advantage of the standard library's potential optimizations for finding scalars in a contiguous memory range.
            const auto upper_bound = end - (sig.size() - 1);
            auto ptr = std::find(begin, upper_bound, first);

            while (ptr != upper_bound) [[likely]] {
                if (std::equal(sig.begin(), sig.end(), ptr)) [[unlikely]] {
                    return ptr;
                }

                ptr = std::find(ptr + 1, upper_bound, first);
            }

            return nullptr;
        }

        auto iter = std::search(std::execution::unseq, begin, end, sig.begin(), sig.end());
        return iter == end ? nullptr : iter;
    }

    const std::byte* do_scan(const std::byte* begin, const std::byte* end, signature sig, scan_mode mode) {
        // All scanners require this so we put it here.
        // Right-strip is done by each individual scanner.
        while (sig.front().mask() == std::byte{0}) {
            sig = sig.subsig(1);
            begin++;
            if (sig.empty())
                return (begin > end) ? nullptr : begin;
        }

        if (begin >= end)
            return nullptr;

        switch (mode) {
            case scan_mode::normal:
            default:
                return scan_impl_normal(begin, end, sig);
            case scan_mode::avx2:
                return scan_impl_avx2(begin, end, sig);
        }
    }
}
