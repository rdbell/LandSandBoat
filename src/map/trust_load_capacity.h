#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure LoadTrust level / weapon / spawn policy.
// Parity: internal/trustutils TrustLevels / TrustWeaponDamage / WeaponDPS /
// SpawnRadius / DualWieldFromCmbSkill (slice 1614).

namespace trustloadhelpers
{

// SKILL_HAND_TO_HAND pin (enables dual-wield modeling for trusts).
constexpr std::uint8_t SkillHandToHand = 1;

// CTrustController::SpawnDistance base (matches Go SpawnDistance = 3.0).
constexpr float SpawnDistance = 3.0f;

// Mob-style damage scale for trusts (0.5).
constexpr float WeaponDamageScale = 0.5f;

// mLvl = master; sLvl = floor(master / 2).
constexpr auto MainLevel(const std::uint8_t masterMLevel) -> std::uint8_t
{
    return masterMLevel;
}

constexpr auto SubLevel(const std::uint8_t masterMLevel) -> std::uint8_t
{
    return masterMLevel / 2;
}

// Spawn nearPosition distance: SpawnDistance * (1 + existingTrustCount).
constexpr auto SpawnRadius(const int existingTrustCount) -> float
{
    const auto count = existingTrustCount < 0 ? 0 : existingTrustCount;
    return SpawnDistance + static_cast<float>(count) * SpawnDistance;
}

// final = max(mobStyleDamage * 0.5 * cmbDmgMult/100, 1)
inline auto WeaponDamage(const std::uint16_t mobStyleDamage, const std::uint16_t cmbDmgMult) -> std::uint16_t
{
    const auto baseDamage     = static_cast<float>(mobStyleDamage) * WeaponDamageScale;
    const auto damageMultiplier = static_cast<float>(cmbDmgMult) / 100.0f;
    const auto adjustedDamage = baseDamage * damageMultiplier;
    return static_cast<std::uint16_t>(std::max(adjustedDamage, 1.0f));
}

// DPS = damage / (delay_ms / 1000); 0 when delay is 0.
inline auto WeaponDPS(const std::uint16_t damage, const std::uint16_t delayMs) -> double
{
    if (delayMs == 0)
    {
        return 0.0;
    }
    return static_cast<double>(damage) / (static_cast<double>(delayMs) / 1000.0);
}

constexpr auto DualWieldFromCmbSkill(const std::uint8_t cmbSkill) -> bool
{
    return cmbSkill == SkillHandToHand;
}

// SC_GRAVITATION pin (skillchain.h); elements >= this form Lv3 skillchains.
constexpr std::uint8_t SkillchainGravitation = 9;

// Min main level for unrestricted Lv3 SC TP skills.
constexpr std::uint8_t MinLv3SkillchainLevel = 60;

// LoadTrustStatsAndSkills TP skill filter:
// canFormLv3 = any SC element >= Gravitation
// onlyHasLv3 = canFormLv3 && existingTPSkills == 0
// allow when !canFormLv3 || mLvl >= 60 || onlyHasLv3
constexpr auto CanUseTPSkill(const std::uint8_t mLvl, const std::uint8_t primary, const std::uint8_t secondary, const std::uint8_t tertiary,
                             const int existingTPSkills) -> bool
{
    const bool canFormLv3 = primary >= SkillchainGravitation || secondary >= SkillchainGravitation || tertiary >= SkillchainGravitation;
    const bool onlyHasLv3 = canFormLv3 && existingTPSkills == 0;
    return !canFormLv3 || mLvl >= MinLv3SkillchainLevel || onlyHasLv3;
}

} // namespace trustloadhelpers
