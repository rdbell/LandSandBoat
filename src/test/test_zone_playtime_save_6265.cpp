#include "test_zone_playtime_save_6265.h"

#include "map/zone_playtime_save.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone playtime save 6265 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SavePlayTime's production-wired unconditional character dispatch.
auto runZonePlayTimeSave6265SelfTests() -> bool
{
    return expect(zoneplaytimesave::ShouldSaveCharacter(), "each listed character is saved");
}
