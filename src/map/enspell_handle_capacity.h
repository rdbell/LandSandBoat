#pragma once

#include <cmath>
#include <cstdint>

// Pure HandleEnspell samba/daze/enspell dispatch after TH early return.

namespace enspellhandlehelpers
{

constexpr std::uint8_t EnspellIFire       = 1;
constexpr std::uint8_t EnspellILight      = 7;
constexpr std::uint8_t EnspellIDark       = 8;
constexpr std::uint8_t EnspellIIFire      = 9;
constexpr std::uint8_t EnspellIIDark      = 16;
constexpr std::uint8_t EnspellBloodWeapon = 17;
constexpr std::uint8_t EnspellAuspice     = 18;

constexpr std::uint8_t ProcFireDamage = 1;
constexpr std::uint8_t ProcIceDamage  = 2;
constexpr std::uint8_t ProcHPDrain    = 21;

constexpr std::uint16_t MsgAddEffectDamage           = 163;
constexpr std::uint16_t MsgAddEffectAdditionalDamage = 229;
constexpr std::uint16_t MsgAddEffectRecoversHP       = 384;

enum class SambaDaze : std::uint8_t
{
    None = 0,
    Drain,
    Aspir,
    Haste,
};

constexpr auto SelectPreviousDaze(const bool hasDrainSamba,
                                  const bool hasAspirSamba,
                                  const bool hasHasteSamba,
                                  const bool defenderIsUndead) -> SambaDaze
{
    if (hasDrainSamba && !defenderIsUndead)
    {
        return SambaDaze::Drain;
    }
    if (hasAspirSamba)
    {
        return SambaDaze::Aspir;
    }
    if (hasHasteSamba)
    {
        return SambaDaze::Haste;
    }
    return SambaDaze::None;
}

enum class DazeClearPath : std::uint8_t
{
    Party = 0,
    TrustMasterParty,
    AttackerOnly,
};

constexpr auto ClassifyDazeClearPath(const bool attackerIsPC,
                                     const bool hasParty,
                                     const bool attackerIsTrust,
                                     const bool hasMaster) -> DazeClearPath
{
    if (attackerIsPC && hasParty)
    {
        return DazeClearPath::Party;
    }
    if (attackerIsTrust && hasMaster)
    {
        return DazeClearPath::TrustMasterParty;
    }
    return DazeClearPath::AttackerOnly;
}

constexpr auto EnspellShouldProc(const std::uint8_t enspell,
                                 const std::uint8_t enspellChance,
                                 const int          roll,
                                 const int          activeRuneCount) -> bool
{
    if (activeRuneCount > 0)
    {
        return true;
    }
    if (enspell == 0)
    {
        return false;
    }
    if (enspellChance == 0)
    {
        return true;
    }
    return static_cast<int>(enspellChance) > roll;
}

enum class EnspellPath : std::uint8_t
{
    None = 0,
    BloodWeapon,
    Rune,
    Auspice,
    ElementalII,
    ElementalI,
    ElementalIISkip,
};

constexpr auto ClassifyEnspellPath(const std::uint8_t enspell,
                                   const bool         defenderIsUndead,
                                   const bool         hasActiveRunes,
                                   const bool         isFirstSwing) -> EnspellPath
{
    if (enspell == EnspellBloodWeapon && !defenderIsUndead)
    {
        return EnspellPath::BloodWeapon;
    }
    if (hasActiveRunes)
    {
        return EnspellPath::Rune;
    }
    if (enspell == EnspellAuspice && isFirstSwing)
    {
        return EnspellPath::Auspice;
    }
    if (enspell <= EnspellIIDark)
    {
        if (enspell > EnspellIDark)
        {
            return isFirstSwing ? EnspellPath::ElementalII : EnspellPath::ElementalIISkip;
        }
        return EnspellPath::ElementalI;
    }
    return EnspellPath::None;
}

constexpr auto ElementalEnspellSubEffect(const std::uint8_t enspell, const bool tierII) -> std::uint8_t
{
    if (tierII)
    {
        if (enspell >= EnspellIIFire && enspell <= EnspellIIDark)
        {
            return static_cast<std::uint8_t>((enspell - 9) + ProcFireDamage);
        }
        return 0;
    }
    if (enspell >= EnspellIFire && enspell <= EnspellIDark)
    {
        return enspell;
    }
    return 0;
}

constexpr auto ElementalEnspellTierAndElement(const std::uint8_t enspell, const EnspellPath path, std::uint8_t& tier, std::uint8_t& element) -> void
{
    tier    = 0;
    element = 0;
    switch (path)
    {
        case EnspellPath::Auspice:
            tier    = 2;
            element = 7;
            break;
        case EnspellPath::ElementalII:
            tier    = 2;
            element = static_cast<std::uint8_t>(enspell - 8);
            break;
        case EnspellPath::ElementalI:
            if (enspell >= EnspellILight)
            {
                tier    = 3;
                element = enspell;
            }
            else
            {
                tier    = 1;
                element = enspell;
            }
            break;
        default:
            break;
    }
}

constexpr auto RuneUsesNewest(const int highestRuneCount) -> bool
{
    return highestRuneCount == 1;
}

inline auto BloodWeaponAbsorbed(const std::int32_t actionParam, const bool isPC, const std::uint8_t bloodWeaponJP) -> std::int32_t
{
    auto absorbed = actionParam;
    if (isPC)
    {
        absorbed += static_cast<std::int32_t>(std::floor(static_cast<float>(absorbed) * 0.02f * static_cast<float>(bloodWeaponJP)));
    }
    return absorbed;
}

constexpr auto ShouldRewriteAddEffectToRecoverHP(const std::uint16_t message, const std::int32_t param) -> bool
{
    return message == MsgAddEffectDamage && param < 0;
}

constexpr auto NormalizeEnspellDamageParam(const std::int32_t param, std::int32_t& outParam, std::uint16_t& outMessage) -> void
{
    if (param < 0)
    {
        outParam   = -param;
        outMessage = MsgAddEffectRecoversHP;
    }
    else
    {
        outParam   = param;
        outMessage = MsgAddEffectAdditionalDamage;
    }
}

inline auto DrainSambaRollUpper(const std::uint16_t power, const std::int32_t delay) -> int
{
    if (power == 0)
    {
        return 1;
    }
    const double multiplier = 3.0 + 5.5 * static_cast<double>(power - 1);
    return static_cast<int>(static_cast<double>(delay) * multiplier) / 100 + 1;
}

inline auto DrainSambaAmount(const std::uint16_t power,
                             const std::int32_t  delay,
                             const std::int8_t   roll,
                             const std::int8_t   lvlDiff,
                             const std::int32_t  finalDamage) -> std::int8_t
{
    if (power == 0)
    {
        return 0;
    }
    int samba = roll;
    if (lvlDiff > 0)
    {
        auto ld = lvlDiff;
        if (ld > 10)
        {
            ld = 10;
        }
        const int penalty = static_cast<int>(std::ceil(static_cast<double>(samba) * static_cast<double>(ld) * 0.04));
        samba -= penalty;
    }
    if (finalDamage <= 2)
    {
        return 0;
    }
    if (samba > static_cast<int>(finalDamage / 2))
    {
        return static_cast<std::int8_t>(finalDamage / 2);
    }
    if (samba < 0)
    {
        return 0;
    }
    return static_cast<std::int8_t>(samba);
}

constexpr auto AspirSambaRollUpper(const std::uint16_t power, const std::int32_t delay) -> int
{
    if (power == 0)
    {
        return 1;
    }
    const int multiplier = 1 + 2 * static_cast<int>(power - 1);
    return static_cast<int>(delay) * multiplier / 100 + 1;
}

constexpr auto AspirSambaAmount(const std::uint16_t power,
                                const std::int32_t  delay,
                                const std::int8_t   roll,
                                const std::int32_t  finalDamage) -> std::int8_t
{
    if (power == 0)
    {
        return 0;
    }
    int samba = roll;
    if (finalDamage <= 2)
    {
        return 0;
    }
    if (samba >= static_cast<int>(finalDamage / 4))
    {
        return static_cast<std::int8_t>(finalDamage / 4);
    }
    if (samba < 0)
    {
        return 0;
    }
    return static_cast<std::int8_t>(samba);
}

constexpr auto ShouldApplyDrainSamba(const SambaDaze daze, const std::uint16_t power) -> bool
{
    return daze == SambaDaze::Drain && power > 0;
}

constexpr auto ShouldApplyAspirSamba(const SambaDaze daze, const std::uint16_t power, const std::int32_t defenderMaxMP) -> bool
{
    return daze == SambaDaze::Aspir && power > 0 && defenderMaxMP > 0;
}

constexpr auto ShouldApplyHasteSamba(const SambaDaze daze, const std::uint16_t power) -> bool
{
    return daze == SambaDaze::Haste && power > 0;
}

constexpr auto DazePowerFromMembership(const bool attackerIDMatchesSubID, const std::uint16_t dazePower) -> std::uint16_t
{
    return attackerIDMatchesSubID ? dazePower : static_cast<std::uint16_t>(0);
}

constexpr auto WeaponAddEffectPriority(const bool isPC, const std::int16_t priorityMod) -> bool
{
    return isPC && priorityMod > 0;
}

constexpr auto GripAddEffectGate(const bool isPC, const bool hasSubEquip, const bool subIsGrip, const bool hasAddEffectType) -> bool
{
    return isPC && hasSubEquip && subIsGrip && hasAddEffectType;
}

constexpr auto MobPetAddEffectGate(const bool isMobOrPet, const std::int16_t addEffectMobMod) -> bool
{
    return isMobOrPet && addEffectMobMod > 0;
}

} // namespace enspellhandlehelpers
