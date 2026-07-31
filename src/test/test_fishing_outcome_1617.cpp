#include "test_fishing_outcome_1617.h"

#include "map/fishing_outcome_capacity.h"

#include <iostream>

namespace
{
using namespace fishingoutcomehelpers;

auto Check() -> bool
{
    // Regen: base 128 generic small fish balanced skill
    if (CalculateRegen(50, 17391, CatchTypeSmallFish, SizeTypeSmall, 40, false, false) != 128)
    {
        return false;
    }
    // large non-Ebisu: +1 → 129
    if (CalculateRegen(50, 17391, CatchTypeBigFish, SizeTypeLarge, 40, false, false) != 129)
    {
        return false;
    }
    // Ebisu skill drain: floor((50+11-12-40)/1.3)=6 → 1+6=7 → 128-7=121
    if (CalculateRegen(50, RodIDEbisu, CatchTypeBigFish, SizeTypeLarge, 40, false, false) != 121)
    {
        return false;
    }
    // skill drain generic: floor((100-12-50)/1.5)=25 → 26 → 102
    if (CalculateRegen(100, 17391, CatchTypeSmallFish, SizeTypeSmall, 50, false, false) != 102)
    {
        return false;
    }
    // NM skips skill drain
    if (CalculateRegen(100, 17391, CatchTypeSmallFish, SizeTypeSmall, 50, false, true) != 128)
    {
        return false;
    }
    // skill penalty: floor((60-0-44)*0.5)=8 → +9 → 137
    if (CalculateRegen(20, 17391, CatchTypeSmallFish, SizeTypeSmall, 60, false, false) != 137)
    {
        return false;
    }
    // Lu Shang mob: -3 + size large +1 → 126
    if (CalculateRegen(50, RodIDLuShang, CatchTypeMob, SizeTypeLarge, 50, false, false) != 126)
    {
        return false;
    }

    // Rod view projection preserves every field used by the pure outcome helpers.
    {
        const auto rod = MakeRodView(1234, SizeTypeLarge, 5, 6, 1, 2, 3, 4, true, true);
        if (rod.rodID != 1234 || rod.sizeType != SizeTypeLarge || rod.minRank != 5 || rod.maxRank != 6 ||
            rod.smDelayBonus != 1 || rod.smMoveBonus != 2 || rod.lgDelayBonus != 3 || rod.lgMoveBonus != 4 ||
            !rod.legendary || !rod.breakable)
        {
            return false;
        }
    }

    // Delay: base 10 count 1 no bonus
    {
        const auto rod = MakeRodView(0, SizeTypeSmall, 0, 0, 0, 0, 0, 0, false, false);
        if (CalculateDelay(10, SizeTypeSmall, rod, 1, false) != 10)
        {
            return false;
        }
        // count 2 → floor(10*1.1)=11
        if (CalculateDelay(10, SizeTypeSmall, rod, 2, false) != 11)
        {
            return false;
        }
        // penguin +2
        if (CalculateDelay(10, SizeTypeSmall, rod, 1, true) != 12)
        {
            return false;
        }
        // cap 15
        if (CalculateDelay(20, SizeTypeSmall, rod, 1, false) != 15)
        {
            return false;
        }
    }
    // small sm bonus
    {
        const auto rod = MakeRodView(0, SizeTypeSmall, 0, 0, 2, 0, 9, 0, false, false);
        if (CalculateDelay(10, SizeTypeSmall, rod, 1, false) != 12)
        {
            return false;
        }
    }

    // Movement mirrors delay structure
    {
        const auto rod = MakeRodView(0, SizeTypeLarge, 0, 0, 0, 0, 0, 3, false, false);
        if (CalculateMovement(10, SizeTypeLarge, rod, 1, false) != 13)
        {
            return false;
        }
        if (CalculateMovement(14, SizeTypeLarge, rod, 1, true) != 15) // 14+3+2=19 → 15
        {
            return false;
        }
    }

    // Lose: none when ranks OK
    {
        const auto rod  = MakeRodView(0, SizeTypeSmall, 0, 20, 0, 0, 0, 0, false, false);
        const auto lose = CalculateLoseChance(CatchTypeSmallFish, 50, 50, SizeTypeSmall, false, 10, rod);
        if (lose.failReason != FailTypeNone || lose.chance != 0)
        {
            return false;
        }
    }
    // too big: ranking 20 > maxRank 10, size large vs rod small → clamp 50
    {
        const auto rod  = MakeRodView(0, SizeTypeSmall, 0, 10, 0, 0, 0, 0, false, false);
        const auto lose = CalculateLoseChance(CatchTypeSmallFish, 40, 50, SizeTypeLarge, false, 20, rod);
        if (lose.failReason != FailTypeLostTooBig || lose.chance != 50)
        {
            return false;
        }
    }
    // low skill only: floor((50-(20+7))*0.8)=floor(23*0.8)=18
    {
        const auto rod  = MakeRodView(0, SizeTypeSmall, 0, 50, 0, 0, 0, 0, false, false);
        const auto lose = CalculateLoseChance(CatchTypeSmallFish, 20, 50, SizeTypeSmall, false, 10, rod);
        if (lose.failReason != FailTypeLostLowSkill || lose.chance != 18)
        {
            return false;
        }
    }

    // Snap: ranking 20, maxRank 10, no bonuses → floor(10*8.5)=85 → clamp 55
    {
        const auto rod  = MakeRodView(0, SizeTypeLarge, 0, 10, 0, 0, 0, 0, false, false);
        const auto snap = CalculateSnapChance(CatchTypeSmallFish, 50, 50, SizeTypeSmall, false, 20, rod);
        if (snap.failReason != FailTypeLineSnap || snap.chance != 55)
        {
            return false;
        }
    }
    // Snap none when ranking <= durability
    {
        const auto rod  = MakeRodView(0, SizeTypeSmall, 0, 20, 0, 0, 0, 0, false, false);
        const auto snap = CalculateSnapChance(CatchTypeSmallFish, 50, 50, SizeTypeSmall, false, 10, rod);
        if (snap.failReason != FailTypeNone || snap.chance != 0)
        {
            return false;
        }
    }

    // Break: not breakable → none
    {
        const auto rod    = MakeRodView(0, SizeTypeSmall, 0, 10, 0, 0, 0, 0, false, false);
        const auto rbreak = CalculateBreakChance(CatchTypeSmallFish, 50, 50, SizeTypeSmall, false, 20, rod);
        if (rbreak.failReason != FailTypeNone || rbreak.chance != 0)
        {
            return false;
        }
    }
    // Break: ranking 20, maxRank 10, skill equal → levelDiffBonus 2, threshold 12
    // strDuraDiff=8, floor(8*1.3)=10
    {
        const auto rod    = MakeRodView(0, SizeTypeSmall, 0, 10, 0, 0, 0, 0, false, true);
        const auto rbreak = CalculateBreakChance(CatchTypeSmallFish, 50, 50, SizeTypeSmall, false, 20, rod);
        if (rbreak.failReason != FailTypeRodBreak || rbreak.chance != 10)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runFishingOutcome1617SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_outcome_1617 self-tests failed\n";
        return false;
    }
    return true;
}
