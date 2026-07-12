#include "test_fishing_bait_rod_1629.h"

#include "map/fishing_bait_rod_capacity.h"

#include <iostream>

namespace
{
using namespace fishingbaitrodhelpers;

auto Check() -> bool
{
    if (ClassifyBaitLoss(false, false, 0, false, 0) != BaitLossReject::OK)
    {
        return false;
    }
    if (ClassifyBaitLoss(true, false, SkillFishing, true, 99) != BaitLossReject::NotWeapon)
    {
        return false;
    }
    if (ClassifyBaitLoss(true, true, 1, true, 99) != BaitLossReject::NotFishingSkill)
    {
        return false;
    }
    if (ClassifyBaitLoss(true, true, SkillFishing, false, 1) != BaitLossReject::KeepFly)
    {
        return false;
    }
    if (ClassifyBaitLoss(true, true, SkillFishing, true, 1) != BaitLossReject::OK)
    {
        return false;
    }
    if (ClassifyBaitLoss(true, true, SkillFishing, false, 12) != BaitLossReject::OK)
    {
        return false;
    }

    if (BaitLossShouldConsume(false, 2) || BaitLossShouldConsume(true, FishingSuccessCatchItem))
    {
        return false;
    }
    if (!BaitLossShouldConsume(true, 2) || !BaitLossShouldUnequip(1) || BaitLossShouldUnequip(2))
    {
        return false;
    }

    if (!ShouldBreakRod(true, 17383) || ShouldBreakRod(false, 17383) || ShouldBreakRod(true, 0))
    {
        return false;
    }

    if (CanFishMob(false, false, StatusDisappear, 1) || CanFishMob(true, true, StatusDisappear, 1))
    {
        return false;
    }
    if (CanFishMob(true, false, 0, 1) || CanFishMob(true, false, StatusDisappear, 0))
    {
        return false;
    }
    if (!CanFishMob(true, false, StatusDisappear, 1))
    {
        return false;
    }
    return true;
}
} // namespace

auto runFishingBaitRod1629SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_bait_rod_1629 self-tests failed\n";
        return false;
    }
    return true;
}
