#include "pch.h"
#include "HttpClient.h"

#include <limits>
#include <winhttp.h>

namespace {
struct InternetHandle {
    HINTERNET value = nullptr;

    explicit InternetHandle(HINTERNET handle) : value(handle) {}

    ~InternetHandle() {
        if (value) WinHttpCloseHandle(value);
    }

    InternetHandle(InternetHandle const&) = delete;
    InternetHandle& operator=(InternetHandle const&) = delete;
};
}

std::string http::errorMessage(char const* operation, unsigned long error) {
    return std::format("{} failed (Win32 error {})", operation, error);
}

std::expected<http::Response, std::string> http::request(
    std::wstring const& method, std::wstring const& url,
    std::vector<std::uint8_t> const& body, Headers const& headers) {
    URL_COMPONENTS parts {};
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = static_cast<DWORD>(-1);
    parts.dwHostNameLength = static_cast<DWORD>(-1);
    parts.dwUrlPathLength = static_cast<DWORD>(-1);
    parts.dwExtraInfoLength = static_cast<DWORD>(-1);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
        return std::unexpected(errorMessage("WinHttpCrackUrl", GetLastError()));
    }

    if (parts.nScheme != INTERNET_SCHEME_HTTP && parts.nScheme != INTERNET_SCHEME_HTTPS) {
        return std::unexpected("Unsupported URL scheme");
    }

    if (body.size() > std::numeric_limits<DWORD>::max()) {
        return std::unexpected("HTTP request body is too large");
    }

    std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring path;
    if (parts.dwUrlPathLength) {
        path.assign(parts.lpszUrlPath, parts.dwUrlPathLength);
    }
    if (parts.dwExtraInfoLength) {
        path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    }
    if (path.empty()) path = L"/";

    InternetHandle session(WinHttpOpen(L"Latite Client/2.9", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                       WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session.value) {
        return std::unexpected(errorMessage("WinHttpOpen", GetLastError()));
    }

    if (!WinHttpSetTimeouts(session.value, 10000, 10000, 15000, 15000)) {
        return std::unexpected(errorMessage("WinHttpSetTimeouts", GetLastError()));
    }

    InternetHandle connection(WinHttpConnect(session.value, host.c_str(), parts.nPort, 0));
    if (!connection.value) {
        return std::unexpected(errorMessage("WinHttpConnect", GetLastError()));
    }

    InternetHandle handle(WinHttpOpenRequest(connection.value, method.c_str(), path.c_str(), nullptr,
                                             WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                             parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0));
    if (!handle.value) {
        return std::unexpected(errorMessage("WinHttpOpenRequest", GetLastError()));
    }

    for (auto const& [name, value] : headers) {
        if (name.find_first_of(L"\r\n:") != std::wstring::npos ||
            value.find_first_of(L"\r\n") != std::wstring::npos) {
            return std::unexpected("Invalid HTTP header");
        }

        auto line = name + L": " + value;
        if (!WinHttpAddRequestHeaders(handle.value, line.c_str(), static_cast<DWORD>(line.size()),
                                      WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
            return std::unexpected(errorMessage("WinHttpAddRequestHeaders", GetLastError()));
        }
    }

    auto length = static_cast<DWORD>(body.size());
    if (!WinHttpSendRequest(handle.value, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            length ? const_cast<std::uint8_t*>(body.data()) : WINHTTP_NO_REQUEST_DATA,
                            length, length, 0)) {
        return std::unexpected(errorMessage("WinHttpSendRequest", GetLastError()));
    }

    if (!WinHttpReceiveResponse(handle.value, nullptr)) {
        return std::unexpected(errorMessage("WinHttpReceiveResponse", GetLastError()));
    }

    Response response;
    DWORD statusSize = sizeof(response.status);
    if (!WinHttpQueryHeaders(handle.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &response.status, &statusSize,
                             WINHTTP_NO_HEADER_INDEX)) {
        return std::unexpected(errorMessage("WinHttpQueryHeaders", GetLastError()));
    }

    std::array<std::uint8_t, 16384> buffer;
    DWORD received = 0;
    do {
        if (!WinHttpReadData(handle.value, buffer.data(), static_cast<DWORD>(buffer.size()), &received)) {
            return std::unexpected(errorMessage("WinHttpReadData", GetLastError()));
        }

        response.body.insert(response.body.end(), buffer.data(), buffer.data() + received);
    } while (received > 0);

    return response;
}
