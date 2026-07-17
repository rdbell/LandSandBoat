#pragma once

#include "common/cbasetypes.h"

#include <cctype>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

// Pure decimal path parsing used by GET /api/zones/{id}.  This deliberately
// preserves HTTPServer's established strtol-to-uint16 conversion: numeric
// values are narrowed after parsing, including unsigned-16 wrap behavior.
namespace worldhttp
{

inline auto ParseZonePathID(const std::string_view zoneText) -> std::optional<uint16>
{
    if (zoneText.empty())
    {
        return std::nullopt;
    }

    for (const unsigned char character : zoneText)
    {
        if (!std::isdigit(character))
        {
            return std::nullopt;
        }
    }

    return static_cast<uint16>(std::strtol(std::string(zoneText).c_str(), nullptr, 10));
}

} // namespace worldhttp
