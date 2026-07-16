/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_scenarioitem_transition.h"

#include <iostream>
#include <string_view>
#include <vector>

#include "map/packets/c2s/0x064_scenarioitem.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s SCENARIOITEM transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto markedKeyItemIds(const uint16_t tableIndex, const uint32_t (&flags)[scenarioitemhelpers::LookItemFlagWordCount]) -> std::vector<uint16_t>
{
    std::vector<uint16_t> ids;
    scenarioitemhelpers::ForEachMarkedKeyItem(tableIndex, flags, [&ids](const uint16_t id)
                                              {
                                                  ids.emplace_back(id);
                                              });
    return ids;
}

auto testSparseFlagsPreserveTableWordBitOrder() -> bool
{
    uint32_t flags[scenarioitemhelpers::LookItemFlagWordCount]{};
    flags[0] = 0x80000005;
    flags[1] = 0x00000002;
    return expect(markedKeyItemIds(3, flags) == std::vector<uint16_t>{ 1536, 1538, 1567, 1569 }, "sparse key item IDs") &&
           expect(scenarioitemhelpers::ShouldSaveKeyItems(), "sparse flags save key items");
}

auto testHighWordBitAndTableOffset() -> bool
{
    uint32_t highFlags[scenarioitemhelpers::LookItemFlagWordCount]{};
    highFlags[15] = 0x80000000;

    uint32_t offsetFlags[scenarioitemhelpers::LookItemFlagWordCount]{};
    offsetFlags[1] = 1;
    return expect(markedKeyItemIds(0, highFlags) == std::vector<uint16_t>{ 511 }, "high word and bit ID") &&
           expect(markedKeyItemIds(2, offsetFlags) == std::vector<uint16_t>{ 1056 }, "table offset ID");
}

auto testNoFlagsStillSaves() -> bool
{
    uint32_t flags[scenarioitemhelpers::LookItemFlagWordCount]{};
    return expect(markedKeyItemIds(7, flags).empty(), "no flags has no IDs") &&
           expect(scenarioitemhelpers::ShouldSaveKeyItems(), "no flags still saves");
}

} // namespace

auto runC2SScenarioItemTransitionSelfTests() -> bool
{
    return testSparseFlagsPreserveTableWordBitOrder() &&
           testHighWordBitAndTableOffset() &&
           testNoFlagsStillSaves();
}
