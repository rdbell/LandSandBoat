#pragma once

namespace weaponskillunlockmodifierhelpers
{
struct Facts
{
    bool hasWeapon{};
    bool isUnlockable{};
    bool isUnlocked{};
};

struct Plan
{
    bool useMainModifier{};
    bool useRangedModifier{};
};

constexpr Plan PlanFor(Facts main, Facts ranged)
{
    return {
        .useMainModifier   = main.hasWeapon && main.isUnlockable && main.isUnlocked,
        .useRangedModifier = ranged.hasWeapon && ranged.isUnlockable && ranged.isUnlocked,
    };
}
} // namespace weaponskillunlockmodifierhelpers
