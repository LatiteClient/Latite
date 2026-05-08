#pragma once
#include <cstddef>

#define LOAD_RESOURCE(x) extern "C" unsigned char _binary_assets_##x##_start[]; extern "C" const unsigned int _binary_assets_##x##_size;
#define GET_RESOURCE(x) ::Resource{reinterpret_cast<const char*>(&_binary_assets_##x##_start[0]), reinterpret_cast<const char*>(&_binary_assets_##x##_start[0]) + _binary_assets_##x##_size}

struct Resource {
    Resource(const char* begin, const char* end) : _begin(begin), _end(end) {}
    [[nodiscard]] const char* data() const { return _begin; }
    [[nodiscard]] const char* begin() const { return _begin; }
    [[nodiscard]] const char* end() const { return _end; }
    [[nodiscard]] size_t size() const { return size_t(_end - _begin); }
    [[nodiscard]] std::string_view str() const { return std::string_view{this->data(), this->size()}; }
    [[nodiscard]] std::span<const std::byte> bytes() const {
        return {
            reinterpret_cast<const std::byte*>(this->begin()),
            reinterpret_cast<const std::byte*>(this->end())
        };
    }
private:
    const char* _begin;
    const char* _end;
};