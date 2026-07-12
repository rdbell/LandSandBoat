#include "test_level_correction_1574.h"

#include "map/level_correction_capacity.h"

#include <iostream>

namespace
{
using namespace levelcorrectionhelpers;

auto Check() -> bool
{
    // Adoulin WS changes off → always level-correct.
    if (!IsLevelCorrectedZone(false, 0) || !IsLevelCorrectedZone(false, 9999))
    {
        return false;
    }
    // Phanauet Channel = 1 in list
    if (!IsLevelCorrectedZone(true, 1) || !IsInLevelCorrectionZoneList(1))
    {
        return false;
    }
    // Not in list
    if (IsLevelCorrectedZone(true, 9999) || IsInLevelCorrectionZoneList(9999))
    {
        return false;
    }
    // Sorted binary search edges: min and max of table
    if (!IsInLevelCorrectionZoneList(kLevelCorrectionZones.front()) ||
        !IsInLevelCorrectionZoneList(kLevelCorrectionZones.back()))
    {
        return false;
    }
    if (kLevelCorrectionZones.size() != 217)
    {
        return false;
    }
    return true;
}
} // namespace

auto runLevelCorrection1574SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "level_correction_1574 self-tests failed\n";
        return false;
    }
    return true;
}
