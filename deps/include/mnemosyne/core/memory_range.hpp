#pragma once
#ifndef MNEMOSYNE_CORE_MEMORY_RANGE_HPP
#define MNEMOSYNE_CORE_MEMORY_RANGE_HPP

#include <cstddef>
#include <span>
#include <concepts>
#include <ranges>
#include <array>

namespace mnem {
    namespace internal {
        template <class T>
        concept contiguous_byte_range = std::ranges::contiguous_range<T> && std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<T>>, std::byte>;
    }

    template <class T>
    concept memory_range = std::copyable<T>
            && std::ranges::range<T>
            && internal::contiguous_byte_range<std::remove_reference_t<std::ranges::range_reference_t<T>>>;

    template <memory_range T>
    using memory_range_element_t = std::remove_reference_t<std::ranges::range_reference_t<std::ranges::range_reference_t<T>>>;

    /// Represents a range in memory.
    class memory_span {
    public:
        constexpr memory_span(std::byte* begin, std::size_t size) noexcept : dumb_array_({{ begin, size }}) {}

        [[nodiscard]] constexpr auto span() const noexcept { return dumb_array_[0]; }

        [[nodiscard]] constexpr auto begin() const noexcept { return dumb_array_.begin(); }
        [[nodiscard]] constexpr auto end() const noexcept { return dumb_array_.end(); }

    private:
        // TODO: this is a severely stupid hack so i don't need to make a custom iterator, but i will do it properly eventually
        std::array<std::span<std::byte>, 1> dumb_array_;
    };

    class const_memory_span {
    public:
        constexpr const_memory_span(const std::byte* begin, std::size_t size) noexcept : dumb_array_({{ begin, size }}) {}
        constexpr const_memory_span(const memory_span& range) noexcept : dumb_array_({ range.span() }) {} // NOLINT(google-explicit-constructor)

        [[nodiscard]] constexpr auto span() const noexcept { return dumb_array_[0]; }

        [[nodiscard]] constexpr auto begin() noexcept { return dumb_array_.begin(); }
        [[nodiscard]] constexpr auto end() noexcept { return dumb_array_.end(); }

    private:
        std::array<std::span<const std::byte>, 1> dumb_array_;
    };
}

#endif
