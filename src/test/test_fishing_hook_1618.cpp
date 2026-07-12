#include "test_fishing_hook_1618.h"

#include "map/fishing_hook_capacity.h"

#include <iostream>

namespace
{
using namespace fishinghookhelpers;

auto Check() -> bool
{
    // Hook wait: base 13
    if (GetHookTimeWait(1, 12, false) != 13)
    {
        return false;
    }
    // new moon -4 → 9
    if (GetHookTimeWait(MoonPhaseNew, 12, false) != 9)
    {
        return false;
    }
    // full + hour 17 + rope → 13-4-1-1=7
    if (GetHookTimeWait(MoonPhaseFull, 17, true) != 7)
    {
        return false;
    }
    // floor at 7
    if (GetHookTimeWait(MoonPhaseNew, 5, true) != 7)
    {
        return false;
    }

    // Hook time: base fishTime only
    if (CalculateHookTime(60, 0, 1, 0, 0, SizeTypeSmall, false, 0, false, false, false) != 60)
    {
        return false;
    }
    // large penalty -10
    if (CalculateHookTime(60, 0, 1, RodFlagLargePenalty, 0, SizeTypeLarge, false, 0, false, false, false) != 50)
    {
        return false;
    }
    // mooching + live + albatross = +60
    if (CalculateHookTime(60, 0, 1, 0, 0, SizeTypeSmall, false, 0, true, true, true) != 120)
    {
        return false;
    }
    // legendary half-time: 60 - 30 = 30
    if (CalculateHookTime(60, 0, 1, 0, 0, SizeTypeSmall, true, LegendaryFlagHalfTime, false, false, false) != 30)
    {
        return false;
    }

    // Critical bite: fishing 50 fish 40 no ebisu moon new
    // fishSkillCheck=36, skillDiff=14, chance=5+28=33, moon +20 → 53
    // MoonPattern3(0)=1 → moonModifier=2, 10*(2-2)=0 → 33
    if (CalculateCriticalBite(50, 40, 0, MoonPhaseNew) != 33)
    {
        return false;
    }
    // overskill early 0: fish 100, fishing 50 → 100-4 > 50
    if (CalculateCriticalBite(50, 100, 0, MoonPhaseNew) != 0)
    {
        return false;
    }
    // Ebisu +40 skill, moon waning crescent (7): MoonPattern3=0 → +20
    // fishSkillCheck=36, skillDiff=50+40-36=54, chance=5+108=113 → clamp 70
    if (CalculateCriticalBite(50, 40, RodIDEbisu, 7) != 70)
    {
        return false;
    }

    // Weather
    if (GetWeatherModifier(WeatherRain) != 1.1f || GetWeatherModifier(WeatherSquall) != 1.2f || GetWeatherModifier(0) != 1.0f)
    {
        return false;
    }

    // Live bait
    if (!IsLiveBait(BaitIDDrillCalamary) || !IsLiveBait(BaitIDDwarfPugil) || IsLiveBait(1))
    {
        return false;
    }

    // Fish sense: lose only low skill → noskill feeling + roll
    {
        LsbRet lose{ FailTypeLostLowSkill, 10 };
        LsbRet none{};
        if (CalculateFishSense(lose, none, none, 1, 0) != SenseTypeNoSkillFeeling + 1)
        {
            return false;
        }
    }
    // snap+break mid band → BAD + roll
    {
        LsbRet none{};
        LsbRet snap{ 2, 35 };
        LsbRet brk{ 3, 40 };
        if (CalculateFishSense(none, snap, brk, 0, 1) != SenseTypeBad + 1)
        {
            return false;
        }
    }
    // terrible
    {
        LsbRet none{};
        LsbRet snap{ 2, 50 };
        LsbRet brk{ 3, 50 };
        if (CalculateFishSense(none, snap, brk, 0, 0) != SenseTypeTerrible)
        {
            return false;
        }
    }

    // Big fish: rolls 100,120 mean 110; weight floor(110*5.0)=550; mid (50+150)/2=100; epic
    {
        const auto s = CalculateBigFishStats(50, 150, 100, 120, 5.0f);
        if (s.length != 110 || s.weight != 550 || !s.epic)
        {
            return false;
        }
    }
    // maxLength <= 1 → empty
    {
        const auto s = CalculateBigFishStats(0, 1, 0, 0, 5.0f);
        if (s.length != 0 || s.weight != 0 || s.epic)
        {
            return false;
        }
    }

    // Mob modifiers: easy regen + difficult heal
    {
        MobModifierRolls rolls{};
        rolls.regenEasy     = 0;
        rolls.healDifficult = 20;
        const auto m        = CalculateMobModifiers(FishingNMRandomRegenEasy | FishingNMRandomHealDifficult, rolls);
        if (m.regenBonus != 0 || m.healBonus != 20 || m.attackPenalty != 0)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runFishingHook1618SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fishing_hook_1618 self-tests failed\n";
        return false;
    }
    return true;
}
