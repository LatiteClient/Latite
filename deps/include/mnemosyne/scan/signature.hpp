#pragma once
#ifndef MNEMOSYNE_SCAN_SIGNATURE_HPP
#define MNEMOSYNE_SCAN_SIGNATURE_HPP

#include "../internal/string_literal.hpp"

#include <cstdint>
#include <vector>
#include <span>
#include <array>

namespace mnem {
    struct sig_element {
        constexpr sig_element() = default;
        constexpr explicit sig_element(std::byte byte, std::byte mask = std::byte{ 0xFF }) noexcept : byte_(byte & mask), mask_(mask) {}

        [[nodiscard]] constexpr std::byte byte() const noexcept { return byte_; }
        [[nodiscard]] constexpr std::byte mask() const noexcept { return mask_; }

        friend constexpr bool operator==(const sig_element& lhs, std::byte rhs) noexcept {
            return lhs.byte_ == (rhs & lhs.mask_);
        }

    private:
        std::byte byte_{0}, mask_{0};
    };

    namespace internal {
        template <class Container>
        class sig_base {
        public:
            using container_t = Container;

        protected:
            container_t c_{};

        public:
            constexpr sig_base() = default;
            constexpr explicit sig_base(container_t c) : c_(std::move(c)) {}

            /// Returns the underlying container.
            [[nodiscard]] constexpr auto& container() noexcept { return c_; }
            /// Returns the underlying container.
            [[nodiscard]] constexpr auto& container() const noexcept { return c_; }

            [[nodiscard]] constexpr auto begin() noexcept { return c_.begin(); }
            [[nodiscard]] constexpr auto end() noexcept { return c_.end(); }
            [[nodiscard]] constexpr auto begin() const noexcept { return c_.begin(); }
            [[nodiscard]] constexpr auto end() const noexcept { return c_.end(); }

            [[nodiscard]] constexpr size_t size() const { return c_.size(); }
            [[nodiscard]] constexpr bool empty() const { return c_.empty(); }

            [[nodiscard]] constexpr auto& operator[](size_t idx) const { return c_[idx]; }
            [[nodiscard]] constexpr auto& operator[](size_t idx) { return c_[idx]; }

            [[nodiscard]] constexpr auto& front() const { return c_.front(); }
            [[nodiscard]] constexpr auto& back() const { return c_.back(); }
        };

        // TODO: These are severely dumb

        constexpr uint8_t parse_nibble(char c) {
            if (c >= '0' && c <= '9')
                return c - '0';
            else if (c >= 'A' && c <= 'F')
                return c - 'A' + 0xA;
            else if (c >= 'a' && c <= 'f')
                return c - 'a' + 0xA;
            else
                return 0;
        }

        constexpr auto parse_byte(std::string_view view) {
            uint8_t byte = 0, mask = 0;

            if (view.size() == 2) {
                if (view[0] != '?') {
                    byte |= parse_nibble(view[0]) << 4;
                    mask |= 0xF0;
                }

                if (view[1] != '?') {
                    byte |= parse_nibble(view[1]);
                    mask |= 0x0F;
                }
            } else {
                char c = view.front();

                if (c != '?') {
                    byte |= parse_nibble(c);
                    mask = 0xFF;
                }
            }

            return sig_element{ std::byte{byte}, std::byte{mask} };
        }
    }

    class sig_storage : public internal::sig_base<std::vector<sig_element>> {};

    template <size_t N>
    class static_sig_storage : public internal::sig_base<std::array<sig_element, N>> {
    public:
        constexpr explicit static_sig_storage(const sig_storage& sig) {
            // TODO: Should we throw an exception or something if the size isn't right?
            std::copy_n(sig.container().begin(), sig.container().size(), this->c_.begin());
        }
    };

    class signature : public internal::sig_base<std::span<const sig_element>> {
        using base_t = internal::sig_base<std::span<const sig_element>>;

    public:
        constexpr signature() = delete;
        constexpr signature(const signature&) noexcept = default;

        constexpr signature(const sig_storage& sig) : // NOLINT(google-explicit-constructor)
            base_t(container_t{ sig.container() }) {}

        template <size_t N>
        constexpr signature(const static_sig_storage<N>& sig) noexcept : // NOLINT(google-explicit-constructor)
                base_t(container_t{ sig.container() }) {}

        constexpr explicit signature(std::span<const sig_element> span) noexcept : base_t(span) {}

        [[nodiscard]] constexpr auto& span() const noexcept { return this->c_; }
        [[nodiscard]] constexpr auto& span() noexcept { return this->c_; }

        [[nodiscard]] constexpr auto subsig(size_t offset, size_t count = std::dynamic_extent) {
            return signature{ this->c_.subspan(offset, count) };
        }
    };



    constexpr sig_storage parse_signature(std::string_view str) {
        sig_storage result;

        size_t iter = 0;
        while (iter < str.size()) {
            while (iter < str.size() && str[iter] == ' ')
                ++iter;

            if (iter == str.size())
                break;

            size_t size = 1;
            if (iter + 1 != str.size() && str[iter + 1] != ' ')
                size = 2;

            result.container().push_back(internal::parse_byte(str.substr(iter, size)));

            iter += size;
        }

        return result;
    }

    template <internal::string_literal Str>
    consteval auto parse_static_signature() {
        constexpr size_t size = parse_signature(Str.stringview()).container().size();
        return static_sig_storage<size>{ parse_signature(Str.stringview()) };
    }

    namespace internal {
        template <internal::string_literal Str>
        struct make_sig_helper {
            static constexpr auto storage = parse_static_signature<Str>();
        };
    }

    template <internal::string_literal Str>
    constexpr signature make_signature() {
        return internal::make_sig_helper<Str>::storage;
    }

    namespace sig_literals {
        template <internal::string_literal Str>
        constexpr signature operator ""_sig() {
            return make_signature<Str>();
        }
    }
}

#endif
