#pragma once

#include <cstdint>

// Pure LoadAvatarStats policy tails (race pin, astral HP, jobStat×1.5, EVA).
// Shared HP/MP/stat band composition lives in calculate_stats_capacity.h.
// Parity: internal/petutils avatar_stats.go (slice 1603).

namespace avatarstatshelpers
{

// Fixed grade race row index used by LoadAvatarStats (LSB race = 3).
// Comment in petutils says "Tarutaru" but the value is used as GetRaceGrades index as-is.
constexpr std::uint8_t RaceGradeGroup = 3;

// PETID pins (petutils.h).
constexpr std::uint32_t PetIDAlexander = 17;
constexpr std::uint32_t PetIDOdin      = 18;

// Flat maxhp bonus for Odin / Alexander.
constexpr std::int32_t AstralHPBonus = 6800;

// Main-job base-stat multiplier (comment: assuming BLM/BLM for avatars).
constexpr float JobStatMultiplier = 1.5f;

constexpr auto IsAstralPet(const std::uint32_t petID) -> bool
{
    return petID == PetIDOdin || petID == PetIDAlexander;
}

// Adds AstralHPBonus when pet is Odin or Alexander.
constexpr auto ApplyAstralHPBonus(const std::int32_t bonusStat, const std::uint32_t petID) -> std::int32_t
{
    if (IsAstralPet(petID))
    {
        return bonusStat + AstralHPBonus;
    }
    return bonusStat;
}

// jobStat * 1.5f for avatar base STR–CHR.
inline auto ScaleJobStat(const float jobStat) -> float
{
    return jobStat * JobStatMultiplier;
}

// Evasion skill → Mod::EVA: identity ≤200, else 200 + (skill-200)*0.9.
inline auto EvasionFromSkill(const std::int16_t evaskill) -> std::int16_t
{
    if (evaskill > 200)
    {
        return static_cast<std::int16_t>(200 + (evaskill - 200) * 0.9f);
    }
    return evaskill;
}

// --- CalculateAvatarStats combat tails ---

// Fixed Mod::DMGPHYS (−50% PDT) and Crit Att Bonus II.
constexpr std::int16_t PhysicalDamageTaken = -5000;
constexpr std::int16_t CritDamageIncrease  = 8;

// SLOT_RANGED base delay used for Titan TP returns.
constexpr std::uint16_t RangedBaseDelay = 360;

// 2014 avatar weapon damage: main level + 2.
constexpr auto WeaponDamage(const std::uint8_t mLvl) -> std::uint16_t
{
    return static_cast<std::uint16_t>(mLvl) + 2;
}

// GetMaxSkill level arg: mLvl > 99 ? 99 : mLvl.
constexpr auto SkillCapLevel(const std::uint8_t mLvl) -> std::uint8_t
{
    return mLvl > 99 ? 99 : mLvl;
}

// Avatar MATT ladder by main level bands.
constexpr auto MagicAttack(const std::uint8_t mLvl) -> std::int16_t
{
    if (mLvl >= 70)
    {
        return 32;
    }
    if (mLvl >= 50)
    {
        return 28;
    }
    if (mLvl >= 30)
    {
        return 24;
    }
    if (mLvl >= 10)
    {
        return 20;
    }
    return 0;
}

// ATT is roughly 2× capped club skill for WHM.
constexpr auto AttackFromSkill(const std::uint16_t maxSkill) -> std::int16_t
{
    return static_cast<std::int16_t>(2 * maxSkill);
}

// Job-point scaling for SMN summon bonuses.
constexpr auto SummonPhysAtkBonus(const std::uint8_t jp) -> std::int16_t
{
    return static_cast<std::int16_t>(jp * 2);
}

constexpr auto SummonMagicDmgBonus(const std::uint8_t jp) -> std::int16_t
{
    return static_cast<std::int16_t>(jp * 5);
}

constexpr auto BloodPactDmgBonus(const std::uint8_t jp) -> std::int16_t
{
    return static_cast<std::int16_t>(jp * 3);
}

// --- CalculateAvatarStats level resolution ---

constexpr std::uint32_t PetIDCarbuncle = 8;
constexpr std::uint32_t PetIDCaitSith = 20;
constexpr std::uint8_t  FallbackLevel = 1;

// Carbuncle / Cait Sith specific level-bonus inject (0 for other pets).
constexpr auto PetSpecificLevelBonus(const std::uint32_t petID, const std::int16_t carbuncleBonus, const std::int16_t caitSithBonus) -> std::int16_t
{
    if (petID == PetIDCarbuncle)
    {
        return carbuncleBonus;
    }
    if (petID == PetIDCaitSith)
    {
        return caitSithBonus;
    }
    return 0;
}

// SMN main: masterMLvl + AVATAR_LVL_BONUS + pet-specific bonus (0..255 clamp).
constexpr auto MainJobLevel(const std::uint8_t masterMLvl, const std::int16_t avatarLvlBonus, const std::int16_t petSpecificBonus) -> std::uint8_t
{
    const auto sum = static_cast<int>(masterMLvl) + static_cast<int>(avatarLvlBonus) + static_cast<int>(petSpecificBonus);
    if (sum < 0)
    {
        return 0;
    }
    if (sum > 255)
    {
        return 255;
    }
    return static_cast<std::uint8_t>(sum);
}

// Resolve avatar main level from master SMN main/sub and bonuses.
constexpr auto ResolveLevel(const bool smnMain, const bool smnSub, const std::uint8_t masterMLvl, const std::uint8_t masterSLvl,
                            const std::int16_t avatarLvlBonus, const std::int16_t petSpecificBonus) -> std::uint8_t
{
    if (smnMain)
    {
        return MainJobLevel(masterMLvl, avatarLvlBonus, petSpecificBonus);
    }
    if (smnSub)
    {
        return masterSLvl;
    }
    return FallbackLevel;
}

} // namespace avatarstatshelpers
