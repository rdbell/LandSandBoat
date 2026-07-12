#pragma once

#include <cstdint>

// Pure TakePhysicalDamage formless/SDT/block/claim/interrupt gates.

namespace takephysicalhelpers
{

constexpr std::uint8_t FormlessStrikesBase  = 55;
constexpr std::uint8_t PhysicalAttackDaken  = 9;

constexpr auto GiveTPToAttacker(const bool giveTP, const bool hasMeikyoShisui) -> bool
{
    return giveTP && !hasMeikyoShisui;
}

constexpr auto GiveTPToVictim(const bool giveTP, const std::uint8_t physicalAttackType) -> bool
{
    return giveTP && physicalAttackType != PhysicalAttackDaken;
}

constexpr auto ShouldUseFormlessStrikesPath(const bool hasFormlessStrikes, const bool isCounter) -> bool
{
    return hasFormlessStrikes && !isCounter;
}

constexpr auto FormlessStrikesMod(const bool isPC, const std::uint8_t formlessMerit) -> std::uint8_t
{
    auto mod = FormlessStrikesBase;
    if (isPC)
    {
        mod = static_cast<std::uint8_t>(mod + formlessMerit);
    }
    return mod;
}

constexpr auto ApplyFormlessPower(const std::int32_t damage, const std::uint8_t formlessMod) -> std::int32_t
{
    return damage * static_cast<std::int32_t>(formlessMod) / 100;
}

inline auto ApplyUDMGBreath(const std::int32_t damage, const std::int16_t udmgBreath) -> std::int32_t
{
    float resist = 1.0f + static_cast<float>(udmgBreath) / 10000.0f;
    if (resist < 0.0f)
    {
        resist = 0.0f;
    }
    return static_cast<std::int32_t>(static_cast<float>(damage) * resist);
}

inline auto ApplyDMGBreath(const std::int32_t damage, const std::int16_t dmgBreath, const std::int16_t dmg) -> std::int32_t
{
    float resist = 1.0f + static_cast<float>(dmgBreath) / 10000.0f + static_cast<float>(dmg) / 10000.0f;
    if (resist < 0.5f)
    {
        resist = 0.5f;
    }
    if (resist > 1.5f)
    {
        resist = 1.5f;
    }
    return static_cast<std::int32_t>(static_cast<float>(damage) * resist);
}

enum class FormlessAbsorbNullResult : std::uint8_t
{
    Absorbed = 0,
    Nulled,
    PassThrough,
};

inline auto FormlessAbsorbNull(const std::int32_t damage,
                               const bool         absorbProc,
                               const bool         nullAllProc,
                               const bool         nullBreathProc,
                               std::int32_t&      outDamage) -> FormlessAbsorbNullResult
{
    if (absorbProc)
    {
        outDamage = -damage;
        return FormlessAbsorbNullResult::Absorbed;
    }
    if (nullAllProc || nullBreathProc)
    {
        outDamage = 0;
        return FormlessAbsorbNullResult::Nulled;
    }
    outDamage = damage;
    return FormlessAbsorbNullResult::PassThrough;
}

constexpr auto FlipCounterAbsorb(const std::int32_t damage, const bool isCounter) -> std::int32_t
{
    if (damage < 0 && isCounter)
    {
        return -damage;
    }
    return damage;
}

inline auto ApplySDT(const std::int32_t damage, const std::int16_t sdtMod) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(damage) * (1.0f + static_cast<float>(sdtMod) / 10000.0f));
}

constexpr auto UseTypeSpecificSDT(const bool isCounter, const bool giveTPtoAttacker) -> bool
{
    return !isCounter || giveTPtoAttacker;
}

constexpr auto ApplyShieldDefBonus(const std::int32_t damage, const std::int16_t shieldDefBonus) -> std::int32_t
{
    if (damage > 0)
    {
        auto out = damage - static_cast<std::int32_t>(shieldDefBonus);
        return out < 0 ? 0 : out;
    }
    return damage;
}

constexpr auto ShouldAddShieldMasteryTP(const std::int32_t damage, const std::int16_t stoneskinMod, const std::int16_t shieldMasteryTP) -> bool
{
    auto unblocked = damage - static_cast<std::int32_t>(stoneskinMod);
    if (unblocked < 0)
    {
        unblocked = 0;
    }
    return unblocked > 0 && shieldMasteryTP != 0;
}

constexpr auto ShieldBlockAbsorb(const bool defenderIsPC, const bool hasShield, const std::uint8_t shieldAbsorption) -> std::uint8_t
{
    if (defenderIsPC && hasShield)
    {
        int a = 100 - static_cast<int>(shieldAbsorption);
        if (a < 0)
        {
            return 0;
        }
        if (a > 100)
        {
            return 100;
        }
        return static_cast<std::uint8_t>(a);
    }
    return 50;
}

constexpr auto ApplyBlockAbsorb(const std::int32_t damage, const std::uint8_t absorb) -> std::int32_t
{
    return (damage * static_cast<std::int32_t>(absorb)) / 100;
}

constexpr auto ShouldApplyReprisalSpikes(const std::int32_t damage, const bool hasReprisal) -> bool
{
    return damage > 0 && hasReprisal;
}

inline auto ReprisalEffectPower(const std::int16_t power, const std::int16_t spikesBonusMod) -> std::int16_t
{
    return static_cast<std::int16_t>(static_cast<float>(power) * (1.0f + static_cast<float>(spikesBonusMod) / 100.0f));
}

inline auto ReprisalBlockedDamage(const std::int32_t damage,
                                  const std::int32_t baseDamage,
                                  const std::uint8_t absorb,
                                  const bool         invincibleOrSentinel) -> std::int32_t
{
    if (invincibleOrSentinel)
    {
        return static_cast<std::int32_t>(static_cast<float>(baseDamage) * (100.0f - static_cast<float>(absorb)) / 100.0f);
    }
    return (damage * static_cast<std::int32_t>(100 - absorb)) / 100;
}

inline auto ReprisalSpikesDamage(const std::int32_t blockedDamage, const std::int16_t effectPower) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(blockedDamage) * (static_cast<float>(effectPower) / 100.0f));
}

constexpr auto ShouldClaimOnPhysicalDamage(const bool defenderIsMob, const bool sameAllegiance) -> bool
{
    return defenderIsMob && !sameAllegiance;
}

constexpr auto ShouldTryHitInterruptPhysical(const bool isRanged,
                                             const bool isBlocked,
                                             const bool defenderIsPC,
                                             const bool hasShieldMasteryTrait) -> bool
{
    if (isRanged)
    {
        return false;
    }
    if (isBlocked && defenderIsPC && hasShieldMasteryTrait)
    {
        return false;
    }
    return true;
}

} // namespace takephysicalhelpers
