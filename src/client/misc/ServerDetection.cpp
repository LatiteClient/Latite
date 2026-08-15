#include "pch.h"

#include "ServerDetection.h"

#include "mc/common/network/GameConnectionInfo.h"

#include <algorithm>
#include <array>
#include <cctype>

namespace {
    using ServerDetection::ServerDefinition;
    using ServerDetection::ServerId;

    // Hostnames are kept alongside their A records resolved on 2026-08-15 so both unresolvedUrl and
    // hostIpAddress work. Hostname matching remains useful when providers rotate those numeric addresses.
    constexpr std::array<std::string_view, 30> hiveAddresses = {
        "geo.hivebedrock.cloud",
        "geo.hivebedrock.network",
        "ca.hivebedrock.network",
        "fr.hivebedrock.network",
        "sg.hivebedrock.network",
        "au.hivebedrock.network",
        "in.hivebedrock.network",
        "hivebedrock.cloud",
        "hivebedrock.network",
        // Global/North America
        "15.235.56.76",
        "15.235.102.99",
        "15.235.32.205",
        "15.235.73.149",
        "15.235.56.107",
        // Europe
        "51.178.216.177",
        "5.196.205.182",
        "46.105.88.71",
        "51.77.4.103",
        "51.77.4.86",
        // Asia
        "141.11.39.2",
        "141.11.39.110",
        "141.11.39.100",
        "141.11.39.104",
        "141.11.39.112",
        // Australia
        "51.161.142.92",
        "51.161.142.93",
        "51.161.140.50",
        // India
        "148.113.15.56",
        "148.113.15.183",
        "148.113.15.63",
    };
    constexpr std::array<std::string_view, 1> hiveFeaturedNames = { "The Hive" };

    constexpr std::array<std::string_view, 10> cubeCraftAddresses = {
        "play.cubecraft.net",
        "mco.cubecraft.net",
        "play.cubecraftgames.net",
        "cubecraft.net",
        // play.cubecraft.net
        "217.145.237.233",
        "217.145.237.231",
        "217.145.237.225",
        // mco.cubecraft.net
        "217.145.237.227",
        "217.145.237.229",
        "217.145.237.239",
    };
    constexpr std::array<std::string_view, 2> cubeCraftFeaturedNames = { "CubeCraft", "CubeCraft Games" };

    constexpr std::array<std::string_view, 3> galaxiteAddresses = {
        "play.galaxite.net",
        "galaxite.net",
        "51.79.82.89",
    };
    constexpr std::array<std::string_view, 1> galaxiteFeaturedNames = { "Galaxite" };

    constexpr std::array<std::string_view, 16> zeqaAddresses = {
        "zeqa.net",       // Global
        "geo.zeqa.net",   // Geo-routed
        "na.zeqa.net",    // North America
        "eu.zeqa.net",    // Europe
        "as.zeqa.net",    // Asia
        "au.zeqa.net",    // Australia
        "sa.zeqa.net",    // South America
        "za.zeqa.net",    // South Africa
        "40.223.14.30",   // Global
        "172.65.200.198", // Geo-routed
        "51.222.254.180", // North America
        "151.80.34.76",   // Europe
        "15.235.229.253", // Asia
        "203.28.238.35",  // Australia
        "80.75.221.63",   // South America
        "38.54.64.216",   // South Africa
    };
    constexpr std::array<std::string_view, 3> zeqaFeaturedNames = { "Zeqa", "Mineville", "Mineville Zeqa" };

    constexpr std::array<ServerDefinition, 4> definitions = {
        ServerDefinition { ServerId::Hive, "The Hive", hiveAddresses, hiveFeaturedNames },
        ServerDefinition { ServerId::CubeCraft, "CubeCraft", cubeCraftAddresses, cubeCraftFeaturedNames },
        ServerDefinition { ServerId::Galaxite, "Galaxite", galaxiteAddresses, galaxiteFeaturedNames },
        ServerDefinition { ServerId::Zeqa, "Zeqa", zeqaAddresses, zeqaFeaturedNames },
    };

    std::string_view normalizeAddress(std::string_view rawAddress) noexcept {
        const auto first = rawAddress.find_first_not_of(" \t\r\n");
        if (first == std::string_view::npos) {
            return {};
        }

        rawAddress.remove_prefix(first);
        const auto last = rawAddress.find_last_not_of(" \t\r\n");
        rawAddress = rawAddress.substr(0, last + 1);

        const auto scheme = rawAddress.find("://");
        if (scheme != std::string_view::npos) {
            rawAddress.remove_prefix(scheme + 3);
        }

        const auto path = rawAddress.find_first_of("/?#");
        if (path != std::string_view::npos) {
            rawAddress = rawAddress.substr(0, path);
        }

        if (rawAddress.starts_with('[')) {
            const auto closingBracket = rawAddress.find(']');
            if (closingBracket != std::string_view::npos) {
                rawAddress = rawAddress.substr(1, closingBracket - 1);
            }
        } else {
            const auto colon = rawAddress.find(':');
            if (colon != std::string_view::npos && colon == rawAddress.rfind(':')) {
                rawAddress = rawAddress.substr(0, colon);
            }
        }

        while (rawAddress.ends_with('.')) {
            rawAddress.remove_suffix(1);
        }

        return rawAddress;
    }

    bool equalIgnoringCase(std::string_view lhs, std::string_view rhs) noexcept {
        return lhs.size() == rhs.size() && std::ranges::equal(lhs, rhs, [](char left, char right) {
                   return std::tolower(static_cast<unsigned char>(left)) ==
                          std::tolower(static_cast<unsigned char>(right));
               });
    }

    bool endsWithIgnoringCase(std::string_view value, std::string_view suffix) noexcept {
        return value.size() >= suffix.size() && equalIgnoringCase(value.substr(value.size() - suffix.size()), suffix);
    }

    bool matchesAddress(std::string_view rawAddress, std::span<const std::string_view> knownAddresses) noexcept {
        const std::string_view address = normalizeAddress(rawAddress);
        if (address.empty()) {
            return false;
        }

        for (const std::string_view knownAddress : knownAddresses) {
            if (equalIgnoringCase(address, knownAddress)) {
                return true;
            }

            const bool isHostname = knownAddress.find_first_not_of("0123456789.") != std::string_view::npos;
            if (isHostname && address.size() > knownAddress.size() && endsWithIgnoringCase(address, knownAddress) &&
                address[address.size() - knownAddress.size() - 1] == '.') {
                return true;
            }
        }
        return false;
    }

    bool matchesFeaturedName(std::string_view featuredName, std::span<const std::string_view> knownNames) noexcept {
        return std::ranges::any_of(knownNames, [featuredName](std::string_view knownName) {
            return equalIgnoringCase(featuredName, knownName);
        });
    }
}

std::span<const ServerDefinition> ServerDetection::getDefinitions() noexcept {
    return definitions;
}

const ServerDefinition& ServerDetection::getDefinition(ServerId id) noexcept {
    return definitions[static_cast<std::size_t>(id)];
}

const ServerDefinition* ServerDetection::identify(SDK::Social::GameConnectionInfo* connectionInfo) noexcept {
    if (!connectionInfo) {
        return nullptr;
    }

    // Addresses are authoritative. Only fall back to Minecraft's featured-server label when no address matches.
    for (const auto& definition : definitions) {
        if (matchesAddress(connectionInfo->unresolvedUrl, definition.addresses) ||
            matchesAddress(connectionInfo->hostIpAddress, definition.addresses)) {
            return &definition;
        }
    }

    for (const auto& definition : definitions) {
        if (matchesFeaturedName(connectionInfo->thirdPartyServerInfo.creatorName, definition.featuredServerNames)) {
            return &definition;
        }
    }
    return nullptr;
}

bool ServerDetection::matches(SDK::Social::GameConnectionInfo* connectionInfo, ServerId id) noexcept {
    const auto* definition = identify(connectionInfo);
    return definition && definition->id == id;
}
