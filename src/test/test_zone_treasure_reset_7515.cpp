#include "test_zone_treasure_reset_7515.h"

#include "map/zone_treasure_reset.h"

#include <iostream>

auto runZoneTreasureReset7515SelfTests() -> bool
{
    const auto expect = [](bool value, const char* label) {
        if (!value)
        {
            std::cerr << "zone treasure reset 7515 self-test failed: " << label << '\n';
        }
        return value;
    };
    using zonehelpers::ShouldResetZoneTreasurePool;
    return expect(!ShouldResetZoneTreasurePool(false, true, true), "missing pool") &&
           expect(!ShouldResetZoneTreasurePool(true, false, true), "non-zone pool") &&
           expect(!ShouldResetZoneTreasurePool(true, true, false), "characters remain") &&
           expect(ShouldResetZoneTreasurePool(true, true, true), "final character resets zone pool");
}
