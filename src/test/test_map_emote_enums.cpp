/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_map_emote_enums.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "map/enums/emote.h"

static_assert(std::is_same_v<std::underlying_type_t<Emote>, uint8>);
static_assert(std::is_same_v<std::underlying_type_t<EmoteMode>, uint8>);

namespace
{

struct EnumCase
{
    std::uint64_t actual;
    std::uint64_t expected;
    std::string   label;
};

template <typename T>
auto enumValue(T value) -> std::uint64_t
{
    return static_cast<std::uint64_t>(value);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map emote enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto emoteCases() -> std::vector<EnumCase>
{
    return {
        { enumValue(Emote::Point), 0, "Emote::Point" },
        { enumValue(Emote::Bow), 1, "Emote::Bow" },
        { enumValue(Emote::Salute), 2, "Emote::Salute" },
        { enumValue(Emote::Kneel), 3, "Emote::Kneel" },
        { enumValue(Emote::Laugh), 4, "Emote::Laugh" },
        { enumValue(Emote::Cry), 5, "Emote::Cry" },
        { enumValue(Emote::No), 6, "Emote::No" },
        { enumValue(Emote::Yes), 7, "Emote::Yes" },
        { enumValue(Emote::Wave), 8, "Emote::Wave" },
        { enumValue(Emote::Goodbye), 9, "Emote::Goodbye" },
        { enumValue(Emote::Welcome), 10, "Emote::Welcome" },
        { enumValue(Emote::Joy), 11, "Emote::Joy" },
        { enumValue(Emote::Cheer), 12, "Emote::Cheer" },
        { enumValue(Emote::Clap), 13, "Emote::Clap" },
        { enumValue(Emote::Praise), 14, "Emote::Praise" },
        { enumValue(Emote::Smile), 15, "Emote::Smile" },
        { enumValue(Emote::Poke), 16, "Emote::Poke" },
        { enumValue(Emote::Slap), 17, "Emote::Slap" },
        { enumValue(Emote::Stagger), 18, "Emote::Stagger" },
        { enumValue(Emote::Sigh), 19, "Emote::Sigh" },
        { enumValue(Emote::Comfort), 20, "Emote::Comfort" },
        { enumValue(Emote::Surprised), 21, "Emote::Surprised" },
        { enumValue(Emote::Amazed), 22, "Emote::Amazed" },
        { enumValue(Emote::Stare), 23, "Emote::Stare" },
        { enumValue(Emote::Blush), 24, "Emote::Blush" },
        { enumValue(Emote::Angry), 25, "Emote::Angry" },
        { enumValue(Emote::Disgusted), 26, "Emote::Disgusted" },
        { enumValue(Emote::Muted), 27, "Emote::Muted" },
        { enumValue(Emote::Doze), 28, "Emote::Doze" },
        { enumValue(Emote::Panic), 29, "Emote::Panic" },
        { enumValue(Emote::Grin), 30, "Emote::Grin" },
        { enumValue(Emote::Dance), 31, "Emote::Dance" },
        { enumValue(Emote::Think), 32, "Emote::Think" },
        { enumValue(Emote::Fume), 33, "Emote::Fume" },
        { enumValue(Emote::Doubt), 34, "Emote::Doubt" },
        { enumValue(Emote::Sulk), 35, "Emote::Sulk" },
        { enumValue(Emote::Psych), 36, "Emote::Psych" },
        { enumValue(Emote::Huh), 37, "Emote::Huh" },
        { enumValue(Emote::Shocked), 38, "Emote::Shocked" },
        { enumValue(Emote::Logging), 40, "Emote::Logging" },
        { enumValue(Emote::Excavation), 41, "Emote::Excavation" },
        { enumValue(Emote::Harvesting), 42, "Emote::Harvesting" },
        { enumValue(Emote::Hurray), 43, "Emote::Hurray" },
        { enumValue(Emote::Toss), 44, "Emote::Toss" },
        { enumValue(Emote::Dance1), 65, "Emote::Dance1" },
        { enumValue(Emote::Dance2), 66, "Emote::Dance2" },
        { enumValue(Emote::Dance3), 67, "Emote::Dance3" },
        { enumValue(Emote::Dance4), 68, "Emote::Dance4" },
        { enumValue(Emote::Bell), 73, "Emote::Bell" },
        { enumValue(Emote::Job), 74, "Emote::Job" },
        { enumValue(Emote::Aim), 96, "Emote::Aim" },
    };
}

auto hasEmoteValue(const std::vector<EnumCase>& tests, std::uint64_t value) -> bool
{
    for (const auto& test : tests)
    {
        if (test.actual == value)
        {
            return true;
        }
    }
    return false;
}

auto expectNoEmoteValue(const std::vector<EnumCase>& tests, std::uint64_t value) -> bool
{
    if (hasEmoteValue(tests, value))
    {
        std::cerr << "map emote enum self-test failed: unexpected Emote value " << value << '\n';
        return false;
    }
    return true;
}

auto expectNoEmoteRange(const std::vector<EnumCase>& tests, std::uint64_t first, std::uint64_t last) -> bool
{
    bool ok = true;
    for (auto value = first; value <= last; ++value)
    {
        ok = expectNoEmoteValue(tests, value) && ok;
    }
    return ok;
}

auto testEmoteValuesAndHoles() -> bool
{
    const auto tests = emoteCases();
    bool       ok    = expectEqualInt(tests.size(), 51, "Emote catalog count");

    ok = expectAll(tests) && ok;
    ok = expectNoEmoteValue(tests, 39) && ok;
    ok = expectNoEmoteRange(tests, 45, 64) && ok;
    ok = expectNoEmoteRange(tests, 69, 72) && ok;
    ok = expectNoEmoteRange(tests, 75, 95) && ok;

    return ok;
}

auto testEmoteModes() -> bool
{
    const auto tests = std::vector<EnumCase>{
        { enumValue(EmoteMode::All), 0, "EmoteMode::All" },
        { enumValue(EmoteMode::Text), 1, "EmoteMode::Text" },
        { enumValue(EmoteMode::Motion), 2, "EmoteMode::Motion" },
    };

    bool ok = expectEqualInt(tests.size(), 3, "EmoteMode catalog count");
    ok      = expectAll(tests) && ok;
    return ok;
}

} // namespace

auto runMapEmoteEnumSelfTests() -> bool
{
    return testEmoteValuesAndHoles() && testEmoteModes();
}
