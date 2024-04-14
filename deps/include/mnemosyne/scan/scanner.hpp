#pragma once
#ifndef MNEMOSYNE_SCAN_SCANNER_HPP
#define MNEMOSYNE_SCAN_SCANNER_HPP

#include "signature.hpp"
#include "../core/memory_range.hpp"

#include <algorithm>
#include <execution>

namespace mnem {
    // TODO: Execution policies to give the user better control over threading
    // TODO: Scan alignment for results we know are aligned in memory
    // TODO: Variants supporting multiple results and batches of signatures
    // TODO: Scan for scalar types and simple byte arrays
    // TODO: Result type instead of pointer

    /// Describes which scanning mode to use.
    enum class scan_mode : int {
        automatic,  // Detect the best mode based on available CPU features
        normal,     // Pure C++ scanner, which uses no CPU extensions or intrinsics.
        sse4_2,     // 128-bit SSE 4.2 scanner, works on almost all modern CPUs
        avx2,       // 256-bit AVX2 scanner, works on most modern CPUs
        avx512,     // 512-bit AVX512 scanner, only works on the most recent CPUs
        max,         // Maximum possible value of this enum
    };

    scan_mode detect_scan_mode();

    namespace internal {
        const std::byte* do_scan(const std::byte* begin, const std::byte* end, signature sig, scan_mode mode);

        inline std::byte* do_scan(std::byte* begin, std::byte* end, signature sig, scan_mode mode) {
            return const_cast<std::byte*>(do_scan( // rare const_cast use case?!?!?!
                    static_cast<const std::byte*>(begin),
                    static_cast<const std::byte*>(end),
                    sig,
                    mode));
        }
    }

    template <memory_range Range = const_memory_span>
    class scanner {
    public:
        explicit scanner(Range range) noexcept : range_(std::move(range)) {}

        [[nodiscard]] memory_range_element_t<Range>* scan_signature(signature sig, scan_mode mode = scan_mode::automatic) const noexcept {
            if (sig.container().empty())
                return nullptr;

            if (mode == scan_mode::automatic)
                mode = detect_scan_mode();

            for (auto& i : range_) {
                if (std::distance(i.begin(), i.end()) < sig.size())
                    continue;

                if (auto result = internal::do_scan(std::to_address(i.begin()), std::to_address(i.end()), sig, mode); result)
                    return result;
            }

            return nullptr;
        }

        [[nodiscard]] auto& range() noexcept { return range_; }
        [[nodiscard]] auto& range() const noexcept { return range_; }

    private:
        Range range_;
    };

    template <std::copyable Range>
    scanner(Range range) -> scanner<Range>;
}

#endif
