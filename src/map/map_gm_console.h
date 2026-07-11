#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstddef>
#include <fmt/format.h>
#include <string>

// Pure MapEngine::onGM console argument policy and diagnostics extracted so
// native tests can pin behavior without zoneutils/charutils hosts.

namespace mapapp
{

// onGM expects exactly 3 argv tokens: command name, char name, level.
constexpr std::size_t GMCommandArgCount = 3;

// GM level is clamped to [GMLevelMin, GMLevelMax] inclusive.
constexpr uint8 GMLevelMin = 0;
constexpr uint8 GMLevelMax = 5;

// ClassifyGMCommandArgs mirrors inputs.size() != 3 usage rejection.
inline auto ClassifyGMCommandArgsValid(const std::size_t argc) -> bool
{
    return argc == GMCommandArgCount;
}

// ClampGMLevel mirrors std::clamp<uint8>(stoi(level), 0, 5).
// parsedLevel is the host-parsed integer (may be out of range before clamp).
inline auto ClampGMLevel(const int parsedLevel) -> uint8
{
    if (parsedLevel < static_cast<int>(GMLevelMin))
    {
        return GMLevelMin;
    }
    if (parsedLevel > static_cast<int>(GMLevelMax))
    {
        return GMLevelMax;
    }
    return static_cast<uint8>(parsedLevel);
}

// FormatGMUsage mirrors the usage line printed on bad argc.
// Preserved trailing space after "example:" for parity.
inline auto FormatGMUsage() -> std::string
{
    return "Usage: gm <char_name> <level>. example: gm Testo 1\n";
}

// FormatGMCharNotFound mirrors the missing-character line (LSB typo "Couldnt").
inline auto FormatGMCharNotFound(const std::string& name) -> std::string
{
    return fmt::format("Couldnt find character: {}\n", name);
}

// FormatGMPromote mirrors the success console line.
inline auto FormatGMPromote(const std::string& name, const uint8 level) -> std::string
{
    return fmt::format("> Promoting {} to GM level {}\n", name, level);
}

// FormatGMPlayerNotice mirrors the MESSAGE_SYSTEM_3 packet text.
inline auto FormatGMPlayerNotice(const uint8 level) -> std::string
{
    return fmt::format("You have been set to GM level {}.", level);
}

// FormatReloadRecipesNotice mirrors onReloadRecipes console output.
inline auto FormatReloadRecipesNotice() -> std::string
{
    return "> Reloading crafting recipes\n";
}

} // namespace mapapp
