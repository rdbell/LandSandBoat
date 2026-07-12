#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure TrySkillUP policy from charutils.

namespace skilluphelpers
{

// MAX_SKILLTYPE from battle_entity.h.
constexpr std::uint8_t MaxSkillType = 64;

// Blue-flag bit on WorkingSkills.skill entries.
constexpr std::uint16_t SkillCappedBlueFlag = 0x8000;

// Combat skill ranges that use COMBAT_SKILLUP_RATE: 1..12 and 25..31.
// Magic skill range that uses MAGIC_SKILLUP_RATE: 32..44.
constexpr std::uint8_t CombatSkillRangeAMin = 1;
constexpr std::uint8_t CombatSkillRangeAMax = 12;
constexpr std::uint8_t CombatSkillRangeBMin = 25;
constexpr std::uint8_t CombatSkillRangeBMax = 31;
constexpr std::uint8_t MagicSkillRangeMin   = 32;
constexpr std::uint8_t MagicSkillRangeMax   = 44;

// Max chance clamp and max tenths gained per skill-up.
constexpr double MaxSkillUpChance = 0.5;
constexpr std::uint8_t MaxSkillAmountTenths = 9;

// IsSkillIDValid mirrors rawSkillID < MAX_SKILLTYPE.
constexpr auto IsSkillIDValid(const std::uint8_t rawSkillID) -> bool
{
    return rawSkillID < MaxSkillType;
}

// ShouldConsiderSkillUp mirrors (rank != 0 && !(skill & 0x8000)) || useSubSkill.
constexpr auto ShouldConsiderSkillUp(const std::uint8_t rank, const std::uint16_t workingSkill, const bool useSubSkill) -> bool
{
    const bool uncappedRanked = rank != 0 && (workingSkill & SkillCappedBlueFlag) == 0;
    return uncappedRanked || useSubSkill;
}

// ResolveCapSkill chooses MainCap/SubCap when useSubSkill prefers the larger cap.
constexpr auto ResolveCapSkill(const std::uint16_t mainCap, const std::uint16_t subCap, const bool useSubSkill) -> std::uint16_t
{
    if (useSubSkill)
    {
        return mainCap > subCap ? mainCap : subCap;
    }
    return mainCap;
}

// ResolveMaxSkill pairs with ResolveCapSkill for victim-level max skill.
constexpr auto ResolveMaxSkill(const std::uint16_t mainCap,
                               const std::uint16_t subCap,
                               const std::uint16_t mainMax,
                               const std::uint16_t subMax,
                               const bool useSubSkill) -> std::uint16_t
{
    if (useSubSkill)
    {
        return mainCap > subCap ? mainMax : subMax;
    }
    return mainMax;
}

// SkillDiff mirrors MaxSkill - CurSkill / 10 (integer tenths→levels).
constexpr auto SkillDiff(const std::uint16_t maxSkill, const std::uint16_t curSkillTenths) -> std::int16_t
{
    return static_cast<std::int16_t>(maxSkill) - static_cast<std::int16_t>(curSkillTenths / 10);
}

// BaseSkillUpChance mirrors Diff/5.0 + mult*(2.0 - log10(1.0 + CurSkill/100)).
// CurSkill/100 is integer division before promotion.
inline auto BaseSkillUpChance(const std::int16_t diff, const std::uint16_t curSkillTenths, const double chanceMultiplier) -> double
{
    const auto curHundreds = curSkillTenths / 100; // integer
    return static_cast<double>(diff) / 5.0 + chanceMultiplier * (2.0 - std::log10(1.0 + static_cast<double>(curHundreds)));
}

// ClampSkillUpChance mirrors chance > 0.5 ? 0.5 : chance.
inline auto ClampSkillUpChance(const double chance) -> double
{
    return chance > MaxSkillUpChance ? MaxSkillUpChance : chance;
}

// IsCombatSkillUpSkill mirrors (1..12) || (25..31).
constexpr auto IsCombatSkillUpSkill(const std::uint8_t skillID) -> bool
{
    return (skillID >= CombatSkillRangeAMin && skillID <= CombatSkillRangeAMax) ||
           (skillID >= CombatSkillRangeBMin && skillID <= CombatSkillRangeBMax);
}

// IsMagicSkillUpSkill mirrors 32..44.
constexpr auto IsMagicSkillUpSkill(const std::uint8_t skillID) -> bool
{
    return skillID >= MagicSkillRangeMin && skillID <= MagicSkillRangeMax;
}

// ApplySkillUpRateMod mirrors chance * (100 + mod) / 100.
inline auto ApplySkillUpRateMod(const double chance, const std::int16_t rateMod) -> double
{
    return chance * ((100.0f + static_cast<float>(rateMod)) / 100.0f);
}

// ShouldGainSkillUp mirrors Diff > 0 && (random < chance || force).
inline auto ShouldGainSkillUp(const std::int16_t diff, const double random, const double chance, const bool forceSkillUp) -> bool
{
    return diff > 0 && (random < chance || forceSkillUp);
}

// SkillUpTier mirrors min(1 + Diff/5, 5) with integer Diff/5.
constexpr auto SkillUpTier(const std::int16_t diff) -> std::uint8_t
{
    const auto tier = 1 + (diff / 5);
    return static_cast<std::uint8_t>(tier > 5 ? 5 : tier);
}

// ExtraSkillUpTierChance is the per-extra-tenth chance for the current tier.
constexpr auto ExtraSkillUpTierChance(const std::uint8_t tier) -> double
{
    switch (tier)
    {
        case 5:
            return 0.900;
        case 4:
            return 0.700;
        case 3:
            return 0.500;
        case 2:
            return 0.300;
        case 1:
            return 0.200;
        default:
            return 0.000;
    }
}

// ShouldStopExtraSkillUp mirrors chance < random || skillAmount == 5.
inline auto ShouldStopExtraSkillUp(const double chance, const double random, const std::uint8_t skillAmount) -> bool
{
    return chance < random || skillAmount == 5;
}

// CapSkillTenths mirrors CapSkill * 10.
constexpr auto CapSkillTenths(const std::uint16_t capSkillLevels) -> std::uint16_t
{
    return static_cast<std::uint16_t>(capSkillLevels * 10);
}

// ApplyRovSkillAmount multiplies amount by (1 + keyItemCount) and clamps to 9.
constexpr auto ApplyRovSkillAmount(const std::uint8_t skillAmount, const std::uint8_t keyItemCount) -> std::uint8_t
{
    const auto rovBonus = static_cast<std::uint8_t>(1 + keyItemCount);
    auto       amount   = static_cast<std::uint8_t>(skillAmount * rovBonus);
    if (amount > MaxSkillAmountTenths)
    {
        amount = MaxSkillAmountTenths;
    }
    return amount;
}

// ApplySkillAmountMultiplier mirrors amount += amount * mult when mult > 1, clamp 9.
constexpr auto ApplySkillAmountMultiplier(const std::uint8_t skillAmount, const std::uint8_t amountMultiplier) -> std::uint8_t
{
    if (amountMultiplier <= 1)
    {
        return skillAmount;
    }
    auto amount = static_cast<std::uint8_t>(skillAmount + skillAmount * amountMultiplier);
    if (amount > MaxSkillAmountTenths)
    {
        amount = MaxSkillAmountTenths;
    }
    return amount;
}

// CapSkillAmountToCeiling clamps so cur + amount <= capTenths; returns amount.
constexpr auto CapSkillAmountToCeiling(const std::uint8_t skillAmount, const std::uint16_t curSkillTenths, const std::uint16_t capTenths) -> std::uint8_t
{
    if (static_cast<std::uint16_t>(skillAmount) + curSkillTenths >= capTenths)
    {
        return static_cast<std::uint8_t>(capTenths - curSkillTenths);
    }
    return skillAmount;
}

// HitsSkillCap reports amount + cur >= cap.
constexpr auto HitsSkillCap(const std::uint8_t skillAmount, const std::uint16_t curSkillTenths, const std::uint16_t capTenths) -> bool
{
    return static_cast<std::uint16_t>(skillAmount) + curSkillTenths >= capTenths;
}

// CrossedSkillLevel mirrors (CurSkill / 10) < (CurSkill + SkillAmount) / 10.
constexpr auto CrossedSkillLevel(const std::uint16_t curSkillTenths, const std::uint8_t skillAmount) -> bool
{
    return (curSkillTenths / 10) < ((curSkillTenths + skillAmount) / 10);
}

// ShouldIncrementWorkingSkill: when arts active, only if bonus unchanged; else always.
constexpr auto ShouldIncrementWorkingSkill(const bool artsActive, const std::int16_t skillBonusBefore, const std::int16_t skillBonusAfter) -> bool
{
    if (artsActive)
    {
        return skillBonusBefore == skillBonusAfter;
    }
    return true;
}

} // namespace skilluphelpers
