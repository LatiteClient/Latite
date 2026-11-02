#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <utility>
#include <vector>

namespace http {
struct Response {
    unsigned status = 0;
    std::vector<std::uint8_t> body;
};

using Headers = std::vector<std::pair<std::wstring, std::wstring>>;

// Returns transport errors separately from HTTP status codes.
std::expected<Response, std::string> request(std::wstring const& method, std::wstring const& url,
                                              std::vector<std::uint8_t> const& body = {},
                                              Headers const& headers = {});

std::string errorMessage(char const* operation, unsigned long error);
}
