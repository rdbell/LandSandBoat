#pragma once

#include <cstdint>

// Pure TakeWeaponskillDamage / TakeSpellDamage / TakeSwipeLungeDamage gates.

namespace takedamagehelpers
{

constexpr std::uint8_t SlotRanged = 2;
constexpr std::uint8_t SlotAmmo   = 3;

constexpr std::uint8_t AttackTypePhysical = 1;
constexpr std::uint8_t AttackTypeRanged   = 3;

constexpr std::int32_t WSDamageClampMin = -99999;
constexpr std::int32_t WSDamageClampMax = 99999;

constexpr auto IsWSRangedSlot(const std::uint8_t slot) -> bool
{
    return slot == SlotAmmo || slot == SlotRanged;
}

constexpr auto WSDefenseBoostNullifies(const std::uint8_t attackType,
                                       const bool         hasDefenseBoost,
                                       const std::uint16_t subPower,
                                       const bool         inFront) -> bool
{
    return attackType == AttackTypePhysical && hasDefenseBoost && subPower != 0 && inFront;
}

constexpr auto WSNullDamage(const std::uint8_t attackType, const bool nullRangedProc, const bool nullPhysProc) -> bool
{
    if (attackType == AttackTypeRanged && nullRangedProc)
    {
        return true;
    }
    if (attackType == AttackTypePhysical && nullPhysProc)
    {
        return true;
    }
    return false;
}

constexpr auto ApplyWSDefenseAndNull(std::int32_t     damage,
                                     const bool       defenseBoostNullifies,
                                     const std::uint8_t attackType,
                                     const bool       nullRangedProc,
                                     const bool       nullPhysProc) -> std::int32_t
{
    if (defenseBoostNullifies)
    {
        damage = 0;
    }
    if (WSNullDamage(attackType, nullRangedProc, nullPhysProc))
    {
        damage = 0;
    }
    return damage;
}

constexpr auto ShouldApplyWSPhalanxStoneskin(const std::int32_t damage) -> bool
{
    return damage > 0;
}

constexpr auto ApplyPhalanx(const std::int32_t damage, const std::int16_t phalanxMod) -> std::int32_t
{
    const auto out = damage - static_cast<std::int32_t>(phalanxMod);
    return out < 0 ? 0 : out;
}

constexpr auto ShouldApplyOverwhelm(const bool isRanged) -> bool
{
    return !isRanged;
}

constexpr auto ClampWSDamage(const std::int32_t damage) -> std::int32_t
{
    if (damage < WSDamageClampMin)
    {
        return WSDamageClampMin;
    }
    if (damage > WSDamageClampMax)
    {
        return WSDamageClampMax;
    }
    return damage;
}

constexpr auto CorrectedDamageAfterTake(const std::int32_t damage, const std::int32_t corrected) -> std::int32_t
{
    if (damage < 0)
    {
        return -corrected;
    }
    return damage;
}

constexpr auto ShouldClaimOnWSDamage(const bool attackerIsPC) -> bool
{
    return attackerIsPC;
}

constexpr auto ShouldProcessWSHitEffects(const std::int32_t damage) -> bool
{
    return damage > 0;
}

constexpr auto ShouldUpdateEnmityFromZeroWS(const std::int32_t damage, const bool defenderIsMob) -> bool
{
    return damage <= 0 && defenderIsMob;
}

constexpr auto ShouldUpdateMobHiPCLvl(const std::uint8_t hiPCLvl, const std::uint8_t attackerMLevel) -> bool
{
    return hiPCLvl < attackerMLevel;
}

constexpr auto ShouldUpdateCharmedMobPacket(const bool defenderIsMob, const bool hasMaster, const bool masterIsPC) -> bool
{
    return defenderIsMob && hasMaster && masterIsPC;
}

constexpr auto ShouldUpdatePetCombatPacket(const bool defenderIsPet) -> bool
{
    return defenderIsPet;
}

inline auto WSStandbyTP(const bool primary, const std::uint16_t bonusTP, const float tpMultiplier, const std::int16_t baseTp) -> std::int16_t
{
    if (!primary)
    {
        return 0;
    }
    return static_cast<std::int16_t>(bonusTP) + static_cast<std::int16_t>(tpMultiplier * static_cast<float>(baseTp));
}

inline auto WSDefenderTP(const float tpMultiplier, const float targetTPMultiplier, const std::int16_t baseTp) -> std::int16_t
{
    return static_cast<std::int16_t>(tpMultiplier * targetTPMultiplier * static_cast<float>(baseTp));
}

constexpr auto WSAttackerAddTP(const std::int16_t saveTP, const std::int16_t standbyTp) -> std::int16_t
{
    return saveTP > standbyTp ? saveTP : standbyTp;
}

constexpr auto ShouldRemoveHagakure(const bool hasHagakure) -> bool
{
    return hasHagakure;
}

constexpr auto ShouldApplySpellDamageEffects(const bool canTargetEnemy, const std::int32_t damage) -> bool
{
    return canTargetEnemy && damage > 0;
}

constexpr auto ShouldApplySwipeLungeHitEffects(const std::int32_t damage) -> bool
{
    return damage > 0;
}

} // namespace takedamagehelpers
