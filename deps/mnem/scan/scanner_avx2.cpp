#include "scanner_impls.hpp"

#include <iostream>

#include <immintrin.h>

namespace mnem::internal {
    namespace {
        enum class second_byte_kind {
            none,   // Fully masked or not present
            full,   // Fully unmasked
            masked  // Partially masked
        };

        enum class cmp_type {
            none,       // Don't compare
            vector,     // Do vectorized compare
            extended,   // Do vectorized compare, then std::equal
        };

        // Load signature bytes and masks into two 256-bit registers
        std::pair<__m256i, __m256i> load_sig_256(std::span<const mnem::sig_element> sig) {
            std::byte bytes[32]{};
            std::byte masks[32]{};

            for (size_t i = 0; i < 32; i++) {
                if (i < sig.size()) {
                    auto byte = sig[i].byte();
                    auto mask = sig[i].mask();
                    bytes[i] = byte & mask;
                    masks[i] = mask;
                } else {
                    bytes[i] = std::byte{0};
                    masks[i] = std::byte{0};
                }
            }

            return std::make_pair(
                    _mm256_loadu_si256(reinterpret_cast<__m256i*>(&bytes)),
                    _mm256_loadu_si256(reinterpret_cast<__m256i*>(&masks))
            );
        }

        template <bool FirstMask, second_byte_kind SecondByteKind, cmp_type CmpType>
        const std::byte* avx2_main_scan(const std::byte* begin, const std::byte* end, std::span<const mnem::sig_element> sig) {
            __m256i first_bytes, first_masks, second_bytes, second_masks, sig_bytes, sig_masks;
            std::span<const mnem::sig_element> ext_sig;

            first_bytes = _mm256_set1_epi8(static_cast<char>(sig[0].byte()));
            if constexpr (FirstMask)
                first_masks = _mm256_set1_epi8(static_cast<char>(sig[0].mask()));

            if constexpr (SecondByteKind != second_byte_kind::none) {
                second_bytes = _mm256_set1_epi8(static_cast<char>(sig[1].byte()));
                if constexpr (SecondByteKind == second_byte_kind::masked) {
                    second_masks = _mm256_set1_epi8(static_cast<char>(sig[1].mask()));
                }
            }

            if constexpr (CmpType != cmp_type::none) {
                std::tie(sig_bytes, sig_masks) = load_sig_256(sig.subspan(2));

                if constexpr (CmpType == cmp_type::extended)
                    ext_sig = sig.subspan(2 + 32);
            }

            for (auto ptr = begin; ptr != end; ptr += 32) {
                // TODO: THIS HAS ONLY BEEN TESTED ON AMD PROCESSORS!
                // This speeds up the scan by 2-6 GB/s when the buffer is not already in the L3 cache, OR the buffer is entirely in the L1 cache.
                // 4096 seems to be the sweet spot for prefetching, since higher values start to reduce performance instead.
                // Since prefetch doesn't affect program behavior besides performance, we also don't have to care about out-of-bounds pointers.
                _mm_prefetch(reinterpret_cast<const char*>(ptr + 4096), _MM_HINT_NTA);
                auto mem = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));

                auto tmp = mem;
                if constexpr (FirstMask)
                    tmp = _mm256_and_si256(tmp, first_masks);

                uint32_t mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(tmp, first_bytes));

                if constexpr (SecondByteKind != second_byte_kind::none) {
                    tmp = mem;
                    if constexpr (SecondByteKind == second_byte_kind::masked)
                        tmp = _mm256_and_si256(tmp, second_masks);

                    uint32_t mask2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(tmp, second_bytes));
                    mask &= mask2 >> 1 | (1u << 31); // pretend second byte matched at the last position in the vector
                }

                while (mask) {
                    auto match = ptr + _tzcnt_u32(mask);

                    if constexpr (CmpType == cmp_type::none)
                        return match;

                    auto match_mem = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(match + 2));
                    match_mem = _mm256_and_si256(match_mem, sig_masks);
                    if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(match_mem, sig_bytes)) == 0xFFFFFFFF) {
                        if constexpr (CmpType == cmp_type::vector)
                            return match;

                        if (std::equal(ext_sig.begin(), ext_sig.end(), match + 2 + 32))
                            return match;
                    }

                    mask = _blsr_u64(mask);
                }
            }

            return nullptr;
        }
    }

    const std::byte* scan_impl_avx2(const std::byte* begin, const std::byte* end, signature sig) {
        // Benchmarks with synthetic data show that the normal scanner is consistently faster than the AVX2 scanner on buffers below 8kb.
        // Currently unsure if this behavior varies across CPUs and vendors.
        if (end - begin <= 8192)
            return scan_impl_normal(begin, end, sig);

        const size_t main_size = 2 + 32; // First two bytes and the extra 32

        // Strip bytes until they will fit into the AVX registers.
        while (sig.back().mask() == std::byte{0} && sig.size() > main_size) {
            sig = sig.subsig(0, sig.size() - 1);
            end--;
            // can't become empty
        }

        bool first_mask = false;
        second_byte_kind sbk = second_byte_kind::none;
        cmp_type cmptype = cmp_type::none;
        size_t read_size = 1; // How many bytes the main scan will read, used for adjusting end ptr

        if (sig.container()[0].mask() != std::byte{0xFF})
            first_mask = true;

        if (sig.container().size() > 1) {
            read_size = 2;
            auto second = sig.container()[1];

            if (second.mask() == std::byte{0xFF})
                sbk = second_byte_kind::full;
            else if (second.mask() != std::byte{0})
                sbk = second_byte_kind::masked;

            if (sig.container().size() > 2) {
                read_size = main_size;
                cmptype = cmp_type::vector;

                if (sig.container().size() > main_size) {
                    read_size = sig.container().size();
                    cmptype = cmp_type::extended;
                }
            }
        }

        auto a_begin = align_ptr_up<32>(begin);
        if (a_begin > begin) {
            auto small_end = std::min(a_begin + sig.size() - 1, end);
            auto ptr = std::search(begin, small_end, sig.begin(), sig.end());
            if (ptr != small_end)
                return ptr;
        }

        auto a_end = align_ptr<32>(end - (read_size - 1));

        const std::byte* result = nullptr;

        // Dispatch to the correct scanner func using epic lambda chain
        if (a_begin < a_end) {
            auto dispatch_2 = [&]<bool FirstMask, second_byte_kind SecondByteKind> {
                switch (cmptype) {
                    case cmp_type::none:
                        result = avx2_main_scan<FirstMask, SecondByteKind, cmp_type::none>(a_begin, a_end, sig);
                        break;
                    case cmp_type::vector:
                        result = avx2_main_scan<FirstMask, SecondByteKind, cmp_type::vector>(a_begin, a_end, sig);
                        break;
                    case cmp_type::extended:
                        result = avx2_main_scan<FirstMask, SecondByteKind, cmp_type::extended>(a_begin, a_end, sig);
                        break;
                }
            };

            auto dispatch_1 = [&]<bool FirstMask> {
                switch (sbk) {
                    case second_byte_kind::none:
                        dispatch_2.template operator()<FirstMask, second_byte_kind::none>();
                        break;
                    case second_byte_kind::full:
                        dispatch_2.template operator()<FirstMask, second_byte_kind::full>();
                        break;
                    case second_byte_kind::masked:
                        dispatch_2.template operator()<FirstMask, second_byte_kind::masked>();
                        break;
                }
            };

            if (first_mask)
                dispatch_1.operator()<true>();
            else
                dispatch_1.operator()<false>();
        }

        if (a_end < end && !result) {
            auto ptr = std::search(a_end, end, sig.begin(), sig.end());
            if (ptr != end)
                return ptr;
        }

        return result;
    }
}
