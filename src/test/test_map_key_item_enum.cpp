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

#include "test_map_key_item_enum.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "map/enums/key_items.h"

static_assert(std::is_same_v<std::underlying_type_t<KeyItem>, std::uint16_t>);

namespace
{

struct EnumCase
{
    KeyItem       actual;
    std::uint16_t expected;
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
        std::cerr << "map key item enum self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto keyItemCases() -> std::vector<EnumCase>
{
    return {
        { KeyItem::NONE, 0, "KeyItem::NONE" },
        { KeyItem::LIMIT_BREAKER, 606, "KeyItem::LIMIT_BREAKER" },
        { KeyItem::AZURE_ABYSSITE_OF_CELERITY, 1385, "KeyItem::AZURE_ABYSSITE_OF_CELERITY" },
        { KeyItem::CRIMSON_ABYSSITE_OF_CELERITY, 1386, "KeyItem::CRIMSON_ABYSSITE_OF_CELERITY" },
        { KeyItem::IVORY_ABYSSITE_OF_CELERITY, 1387, "KeyItem::IVORY_ABYSSITE_OF_CELERITY" },
        { KeyItem::JOB_GESTURE_WARRIOR, 1738, "KeyItem::JOB_GESTURE_WARRIOR" },
        { KeyItem::JOB_GESTURE_MONK, 1739, "KeyItem::JOB_GESTURE_MONK" },
        { KeyItem::JOB_GESTURE_WHITE_MAGE, 1740, "KeyItem::JOB_GESTURE_WHITE_MAGE" },
        { KeyItem::JOB_GESTURE_BLACK_MAGE, 1741, "KeyItem::JOB_GESTURE_BLACK_MAGE" },
        { KeyItem::JOB_GESTURE_RED_MAGE, 1742, "KeyItem::JOB_GESTURE_RED_MAGE" },
        { KeyItem::JOB_GESTURE_THIEF, 1743, "KeyItem::JOB_GESTURE_THIEF" },
        { KeyItem::JOB_GESTURE_PALADIN, 1744, "KeyItem::JOB_GESTURE_PALADIN" },
        { KeyItem::JOB_GESTURE_DARK_KNIGHT, 1745, "KeyItem::JOB_GESTURE_DARK_KNIGHT" },
        { KeyItem::JOB_GESTURE_BEASTMASTER, 1746, "KeyItem::JOB_GESTURE_BEASTMASTER" },
        { KeyItem::JOB_GESTURE_BARD, 1747, "KeyItem::JOB_GESTURE_BARD" },
        { KeyItem::JOB_GESTURE_RANGER, 1748, "KeyItem::JOB_GESTURE_RANGER" },
        { KeyItem::JOB_GESTURE_SAMURAI, 1749, "KeyItem::JOB_GESTURE_SAMURAI" },
        { KeyItem::JOB_GESTURE_NINJA, 1750, "KeyItem::JOB_GESTURE_NINJA" },
        { KeyItem::JOB_GESTURE_DRAGOON, 1751, "KeyItem::JOB_GESTURE_DRAGOON" },
        { KeyItem::JOB_GESTURE_SUMMONER, 1752, "KeyItem::JOB_GESTURE_SUMMONER" },
        { KeyItem::JOB_GESTURE_BLUE_MAGE, 1753, "KeyItem::JOB_GESTURE_BLUE_MAGE" },
        { KeyItem::JOB_GESTURE_CORSAIR, 1754, "KeyItem::JOB_GESTURE_CORSAIR" },
        { KeyItem::JOB_GESTURE_PUPPETMASTER, 1755, "KeyItem::JOB_GESTURE_PUPPETMASTER" },
        { KeyItem::JOB_GESTURE_DANCER, 1756, "KeyItem::JOB_GESTURE_DANCER" },
        { KeyItem::JOB_GESTURE_SCHOLAR, 1757, "KeyItem::JOB_GESTURE_SCHOLAR" },
        { KeyItem::FROG_FISHING, 1976, "KeyItem::FROG_FISHING" },
        { KeyItem::SERPENT_RUMORS, 1977, "KeyItem::SERPENT_RUMORS" },
        { KeyItem::MOOCHING, 1978, "KeyItem::MOOCHING" },
        { KeyItem::WINDURST_TRUST_PERMIT, 2497, "KeyItem::WINDURST_TRUST_PERMIT" },
        { KeyItem::BASTOK_TRUST_PERMIT, 2499, "KeyItem::BASTOK_TRUST_PERMIT" },
        { KeyItem::SAN_DORIA_TRUST_PERMIT, 2501, "KeyItem::SAN_DORIA_TRUST_PERMIT" },
        { KeyItem::JOB_BREAKER, 2544, "KeyItem::JOB_BREAKER" },
        { KeyItem::IMPERIAL_CHAIR, 2826, "KeyItem::IMPERIAL_CHAIR" },
        { KeyItem::DECORATIVE_CHAIR, 2827, "KeyItem::DECORATIVE_CHAIR" },
        { KeyItem::ORNATE_STOOL, 2828, "KeyItem::ORNATE_STOOL" },
        { KeyItem::REFINED_CHAIR, 2829, "KeyItem::REFINED_CHAIR" },
        { KeyItem::PORTABLE_CONTAINER, 2830, "KeyItem::PORTABLE_CONTAINER" },
        { KeyItem::CHOCOBO_CHAIR, 2831, "KeyItem::CHOCOBO_CHAIR" },
        { KeyItem::EPHRAMADIAN_THRONE, 2832, "KeyItem::EPHRAMADIAN_THRONE" },
        { KeyItem::SHADOW_THRONE, 2833, "KeyItem::SHADOW_THRONE" },
        { KeyItem::LEAF_BENCH, 2834, "KeyItem::LEAF_BENCH" },
        { KeyItem::ASTRAL_CUBE, 2835, "KeyItem::ASTRAL_CUBE" },
        { KeyItem::CHOCOBO_CHAIR_II, 2836, "KeyItem::CHOCOBO_CHAIR_II" },
        { KeyItem::RHAPSODY_IN_WHITE, 2884, "KeyItem::RHAPSODY_IN_WHITE" },
        { KeyItem::RHAPSODY_IN_UMBER, 2885, "KeyItem::RHAPSODY_IN_UMBER" },
        { KeyItem::RHAPSODY_IN_AZURE, 2886, "KeyItem::RHAPSODY_IN_AZURE" },
        { KeyItem::RHAPSODY_IN_CRIMSON, 2887, "KeyItem::RHAPSODY_IN_CRIMSON" },
        { KeyItem::RHAPSODY_IN_EMERALD, 2888, "KeyItem::RHAPSODY_IN_EMERALD" },
        { KeyItem::RHAPSODY_IN_MAUVE, 2889, "KeyItem::RHAPSODY_IN_MAUVE" },
        { KeyItem::RHAPSODY_IN_FUCHSIA, 2890, "KeyItem::RHAPSODY_IN_FUCHSIA" },
        { KeyItem::RHAPSODY_IN_PUCE, 2891, "KeyItem::RHAPSODY_IN_PUCE" },
        { KeyItem::RHAPSODY_IN_OCHRE, 2892, "KeyItem::RHAPSODY_IN_OCHRE" },
        { KeyItem::SCINTILLATING_RHAPSODY, 2893, "KeyItem::SCINTILLATING_RHAPSODY" },
        { KeyItem::JOB_GESTURE_GEOMANCER, 2963, "KeyItem::JOB_GESTURE_GEOMANCER" },
        { KeyItem::JOB_GESTURE_RUNE_FENCER, 2964, "KeyItem::JOB_GESTURE_RUNE_FENCER" },
        { KeyItem::MOG_PATIO_DESIGN_DOCUMENT, 3051, "KeyItem::MOG_PATIO_DESIGN_DOCUMENT" },
        { KeyItem::CHOCOBO_COMPANION, 3072, "KeyItem::CHOCOBO_COMPANION" },
        { KeyItem::CIPHER_BRACELET, 3361, "KeyItem::CIPHER_BRACELET" },
    };
}

auto hasSeenValue(const std::vector<std::uint64_t>& seen, std::uint64_t value) -> bool
{
    for (const auto seenValue : seen)
    {
        if (seenValue == value)
        {
            return true;
        }
    }
    return false;
}

auto testKeyItemValuesAndCatalogShape() -> bool
{
    const auto tests = keyItemCases();
    bool       ok    = expectEqualInt(tests.size(), 58, "KeyItem catalog count");

    std::vector<std::uint64_t> seen;
    std::uint64_t              previous = 0;
    std::size_t                gaps     = 0;
    bool                       first    = true;

    for (const auto& test : tests)
    {
        const auto value = enumValue(test.actual);
        ok               = expectEqualInt(value, test.expected, test.label) && ok;

        if (hasSeenValue(seen, value))
        {
            std::cerr << "map key item enum self-test failed: duplicate value " << value << " at " << test.label << '\n';
            ok = false;
        }
        seen.emplace_back(value);

        if (!first)
        {
            if (value <= previous)
            {
                std::cerr << "map key item enum self-test failed: " << test.label << " is not ordered after previous value " << previous << '\n';
                ok = false;
            }
            if (value > previous + 1)
            {
                ++gaps;
            }
        }
        previous = value;
        first    = false;
    }

    ok = expectEqualInt(gaps, 14, "KeyItem sparse gap count") && ok;

    return ok;
}

} // namespace

auto runMapKeyItemEnumSelfTests() -> bool
{
    return testKeyItemValuesAndCatalogShape();
}
