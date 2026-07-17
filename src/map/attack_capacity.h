#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CAttack check-policy halves for parry/guard/deflect/counter/cover.

namespace attackhelpers
{

// PHYSICAL_ATTACK_TYPE::DAKEN numeric.
constexpr uint8 AttackTypeDaken = 9;

// Counter rate hard cap used with COUNTER mod + merit.
constexpr uint16 CounterRateHardCap = 80;

// Seigan counter is Zanshin rate / 4 after clamp.
constexpr uint16 SeiganCounterDivisor = 4;

// Facing arc used for counter (64 units of 256).
constexpr uint8 CounterFacingArc = 64;

// IsDakenAttack mirrors attack type == DAKEN exclusions.
inline auto IsDakenAttack(const uint8 attackType) -> bool
{
    return attackType == AttackTypeDaken;
}

// ShouldSkipParryForDaken mirrors CheckParried Daken early-out.
inline auto ShouldSkipParryForDaken(const uint8 attackType) -> bool
{
    return IsDakenAttack(attackType);
}

// ShouldSkipCounterForDaken mirrors CheckCounter Daken early-out.
inline auto ShouldSkipCounterForDaken(const uint8 attackType) -> bool
{
    return IsDakenAttack(attackType);
}

// ShouldSkipAnticipateForDaken mirrors CheckAnticipated Daken early-out.
inline auto ShouldSkipAnticipateForDaken(const uint8 attackType) -> bool
{
    return IsDakenAttack(attackType);
}

// ApplyGuardDamageRatio mirrors damageRatio -= 1.0f clamped to >= 0.
inline auto ApplyGuardDamageRatio(const float damageRatio) -> float
{
    return std::max(damageRatio - 1.0f, 0.0f);
}

// GuardResolution is the state mutation performed by CAttack::CheckGuarded
// after its entity-dependent guard check. A failed guard intentionally leaves
// the stored ratio untouched; a successful guard lowers it by one and floors
// it at zero.
struct GuardResolution
{
    bool  guarded{};
    float damageRatio{};
};

// ResolveGuardCheck mirrors the pure mutation half of CAttack::CheckGuarded.
inline auto ResolveGuardCheck(const bool guarded, const float damageRatio) -> GuardResolution
{
    if (!guarded)
    {
        return { false, damageRatio };
    }
    return { true, ApplyGuardDamageRatio(damageRatio) };
}

// IsDeflected mirrors DefenseBoost present + subpower > 0 + infront(host).
// hasDefenseBoost and subpower injected; facing result injected as inFront.
inline auto IsDeflected(const bool hasDefenseBoost, const uint16 subPower, const bool inFront) -> bool
{
    if (!hasDefenseBoost || subPower == 0)
    {
        return false;
    }
    return inFront;
}

// ShouldBlockCounterForState mirrors !engaged || HasPreventActionEffect(true).
inline auto ShouldBlockCounterForState(const bool isEngaged, const bool hasPreventActionIgnoringCharm) -> bool
{
    return !isEngaged || hasPreventActionIgnoringCharm;
}

// ShouldAddMNKCounterMerit mirrors TYPE_PC && MNK main.
inline auto ShouldAddMNKCounterMerit(const bool isPC, const bool isMNKMain) -> bool
{
    return isPC && isMNKMain;
}

// ClampCounterRate mirrors clamp(COUNTER + merit, 0, 80).
inline auto ClampCounterRate(const int16 counterMod, const uint8 meritCounter) -> uint16
{
    int32 total = static_cast<int32>(counterMod) + static_cast<int32>(meritCounter);
    if (total < 0)
    {
        return 0;
    }
    if (total > static_cast<int32>(CounterRateHardCap))
    {
        return CounterRateHardCap;
    }
    return static_cast<uint16>(total);
}

// ComputeSeiganCounterChance mirrors (clamp Zanshin+merit, 0..100) / 4 when valid seigan.
inline auto ComputeSeiganCounterChance(
    const bool hasValidSeigan,
    const uint16 zanshinMod,
    const uint16 zanshinMerit) -> uint16
{
    if (!hasValidSeigan)
    {
        return 0;
    }
    uint16 chance = zanshinMod + zanshinMerit;
    if (chance > 100)
    {
        chance = 100;
    }
    return static_cast<uint16>(chance / SeiganCounterDivisor);
}

// IsValidSeiganForCounter mirrors 2H weapon && Seigan status.
inline auto IsValidSeiganForCounter(const bool isTwoHandedWeapon, const bool hasSeigan) -> bool
{
    return isTwoHandedWeapon && hasSeigan;
}

// ShouldSkipCounterForPerfectDodge mirrors attacker has Perfect Dodge.
inline auto ShouldSkipCounterForPerfectDodge(const bool attackerHasPerfectDodge) -> bool
{
    return attackerHasPerfectDodge;
}

// ShouldAttemptCounterRate mirrors rate roll OR seigan roll (host injects both procs).
inline auto ShouldAttemptCounterRate(const bool counterRateProcs, const bool seiganRateProcs) -> bool
{
    return counterRateProcs || seiganRateProcs;
}

// ShouldLandCounter mirrors rate attempt && facing && hit-rate proc.
inline auto ShouldLandCounter(const bool rateAttempt, const bool facing, const bool hitRateProcs) -> bool
{
    return rateAttempt && facing && hitRateProcs;
}

// ShouldPerfectCounter mirrors Perfect Counter status after normal counter miss path.
// Host only evaluates this when rate/facing path did not land.
inline auto ShouldPerfectCounter(const bool hasPerfectCounter) -> bool
{
    return hasPerfectCounter;
}

// IsCoverActive mirrors cover ability user non-null && alive.
inline auto IsCoverActive(const bool coverUserPresent, const bool coverUserAlive) -> bool
{
    return coverUserPresent && coverUserAlive;
}

// HasThirdEyeForAnticipate mirrors third eye effect present.
inline auto HasThirdEyeForAnticipate(const bool hasThirdEye) -> bool
{
    return hasThirdEye;
}

// ClampZanshinRate mirrors clamp for seigan path input before /4.
inline auto ClampZanshinRate(const uint16 rate) -> uint16
{
    return rate > 100 ? static_cast<uint16>(100) : rate;
}

// --- Slice 1377: ProcessDamage SA/TA/Samba pure gates ---

// PHYSICAL_ATTACK_TYPE::NORMAL and SAMBA.
constexpr uint8 AttackTypeNormal = 0;
constexpr uint8 AttackTypeSamba  = 7;

// ShouldApplySneakAttack mirrors THF main + first swing + SA effect + position/hide/doubt.
inline auto ShouldApplySneakAttack(
    const bool isTHFMain,
    const bool isFirstSwing,
    const bool hasSneakAttack,
    const bool behindTarget,
    const bool hasHide,
    const bool victimHasDoubt) -> bool
{
    return isTHFMain && isFirstSwing && hasSneakAttack && (behindTarget || hasHide || victimHasDoubt);
}

// ShouldApplyTrickAttack mirrors THF main + first swing + TA entity present.
inline auto ShouldApplyTrickAttack(
    const bool isTHFMain,
    const bool isFirstSwing,
    const bool hasTAEntity) -> bool
{
    return isTHFMain && isFirstSwing && hasTAEntity;
}

// SneakAttackDexBonus mirrors DEX * (1 + max(SNEAK_ATK_DEX/100, 0)).
inline auto SneakAttackDexBonus(const int16 dex, const int16 sneakAtkDexMod) -> float
{
    float bonusPct = static_cast<float>(sneakAtkDexMod) / 100.0f;
    if (bonusPct < 0.0f)
    {
        bonusPct = 0.0f;
    }
    return static_cast<float>(dex) * (1.0f + bonusPct);
}

// TrickAttackAgiBonus mirrors AGI * (1 + max(TRICK_ATK_AGI/100, 0)).
inline auto TrickAttackAgiBonus(const int16 agi, const int16 trickAtkAgiMod) -> float
{
    float bonusPct = static_cast<float>(trickAtkAgiMod) / 100.0f;
    if (bonusPct < 0.0f)
    {
        bonusPct = 0.0f;
    }
    return static_cast<float>(agi) * (1.0f + bonusPct);
}

// ShouldPromoteNormalToSamba mirrors NORMAL attack type + any Samba effect.
inline auto ShouldPromoteNormalToSamba(
    const uint8 attackType,
    const bool hasDrainSamba,
    const bool hasAspirSamba,
    const bool hasHasteSamba) -> bool
{
    return attackType == AttackTypeNormal && (hasDrainSamba || hasAspirSamba || hasHasteSamba);
}

// ShouldTrySkillUp mirrors damage > 0.
inline auto ShouldTrySkillUp(const int32 damage) -> bool
{
    return damage > 0;
}

// ShouldSkillUpThrowing mirrors Daken attack type for PC skillup.
inline auto ShouldSkillUpThrowing(const uint8 attackType) -> bool
{
    return attackType == AttackTypeDaken;
}

// ShouldSkillUpAutomaton mirrors pet + master PC + automaton type.
inline auto ShouldSkillUpAutomaton(
    const bool isPet,
    const bool masterIsPC,
    const bool isAutomaton) -> bool
{
    return isPet && masterIsPC && isAutomaton;
}

// NaturalH2HDamage mirrors floor(skill * 0.11) + 3.
inline auto NaturalH2HDamage(const uint16 h2hSkill) -> int32
{
    return static_cast<int32>(h2hSkill * 0.11f) + 3;
}

// MobH2HPenaltyPreToAU mirrors 0.425 for region <= LIMBUS.
constexpr float MobH2HPenaltyPreToAU = 0.425f;
// MobH2HPenaltyToAUOnward mirrors 0.650 for later regions.
constexpr float MobH2HPenaltyToAUOnward = 0.650f;

// SelectMobH2HPenalty mirrors pre-TOAU vs later region penalty when no NO_H2H_PENALTY.
// isPreToAURegion: regionID <= LIMBUS.
inline auto SelectMobH2HPenalty(const bool noH2HPenaltyMod, const bool isPreToAURegion) -> float
{
    if (noH2HPenaltyMod)
    {
        return 1.0f;
    }
    return isPreToAURegion ? MobH2HPenaltyPreToAU : MobH2HPenaltyToAUOnward;
}

// ShouldApplyConsumeMana mirrors TYPE_PC.
inline auto ShouldApplyConsumeMana(const bool isPC) -> bool
{
    return isPC;
}


// --- Slice 1378: ProcessDamage base assembly and post-multipliers ---

// PHYSICAL_ATTACK_TYPE numeric pins used by damage assembly.
constexpr uint8 AttackTypeDouble = 1;
constexpr uint8 AttackTypeTriple = 2;
constexpr uint8 AttackTypeKick   = 4;

// Mob kick penalty is 2/3 per Jimmy.
constexpr float MobKickPenalty = 2.0f / 3.0f;

// FloorAtZero mirrors std::max(damage, 0).
inline auto FloorAtZero(const int32 damage) -> int32
{
    return damage < 0 ? 0 : damage;
}

// ApplyDamageRatio mirrors floor(damage * damageRatio) as uint32-style floor.
// Returns non-negative int32; negative inputs clamp to 0 first.
inline auto ApplyDamageRatio(const int32 damage, const float damageRatio) -> int32
{
    if (damage <= 0)
    {
        return 0;
    }
    return static_cast<int32>(std::floor(static_cast<float>(damage) * damageRatio));
}

// AssembleMobH2HDamage mirrors mob H2H kick vs non-kick path before ratio.
// baseDamage is weapon dmg + bonusBasePhysicalDamage (not yet including fSTR).
// kickDamageMod is Mod::KICK_DMG; fSTR is injected.
inline auto AssembleMobH2HDamage(
    const int32 baseDamagePlusBonus,
    const bool isKick,
    const int32 kickDamageMod,
    const int32 fSTR,
    const float mobH2HPenalty) -> int32
{
    if (isKick)
    {
        // (base + kick) * mobPenalty * kickPenalty + fSTR
        float dmg = static_cast<float>(baseDamagePlusBonus + kickDamageMod) * mobH2HPenalty * MobKickPenalty;
        dmg += static_cast<float>(fSTR);
        return FloorAtZero(static_cast<int32>(dmg)); // intermediate before ratio uses float then cast elsewhere
    }
    // Non-kick: (base + fSTR) * mobPenalty
    float dmg = static_cast<float>(baseDamagePlusBonus + fSTR) * mobH2HPenalty;
    return FloorAtZero(static_cast<int32>(dmg));
}

// Note: production uses float intermediate then floor after ratio. For non-kick
// path: m_damage = (m_damage + fSTR) * mobH2HPenalty then max(0) then floor*ratio.
// Returning int32 mid-cast can diverge; provide float intermediate helpers.

// AssembleMobH2HDamagePreRatio returns the pre-ratio value as float for fidelity.
inline auto AssembleMobH2HDamagePreRatio(
    const int32 baseDamagePlusBonus,
    const bool isKick,
    const int32 kickDamageMod,
    const int32 fSTR,
    const float mobH2HPenalty) -> float
{
    if (isKick)
    {
        return static_cast<float>(baseDamagePlusBonus + kickDamageMod) * mobH2HPenalty * MobKickPenalty + static_cast<float>(fSTR);
    }
    return static_cast<float>(baseDamagePlusBonus + fSTR) * mobH2HPenalty;
}

// AssemblePlayerH2HKickPreRatio mirrors kickDamage + bonus + fSTR (kickDamage already includes natural+kick mod).
inline auto AssemblePlayerH2HKickPreRatio(
    const int32 naturalH2hDamage,
    const int32 kickDamageMod,
    const int32 bonusBasePhysicalDamage,
    const int32 fSTR) -> int32
{
    return FloorAtZero(naturalH2hDamage + kickDamageMod + bonusBasePhysicalDamage + fSTR);
}

// AssemblePlayerH2HPunchPreRatio mirrors base + natural + bonus + fSTR.
inline auto AssemblePlayerH2HPunchPreRatio(
    const int32 baseDamage,
    const int32 naturalH2hDamage,
    const int32 bonusBasePhysicalDamage,
    const int32 fSTR) -> int32
{
    return FloorAtZero(baseDamage + naturalH2hDamage + bonusBasePhysicalDamage + fSTR);
}

// AssembleMainHandPreRatio mirrors main weapon dmg + bonus + fSTR.
inline auto AssembleMainHandPreRatio(
    const int32 mainWeaponDmg,
    const int32 bonusBasePhysicalDamage,
    const int32 fSTR) -> int32
{
    return FloorAtZero(mainWeaponDmg + bonusBasePhysicalDamage + fSTR);
}

// AssembleSubHandPreRatio mirrors sub weapon dmg + bonus + fSTR.
inline auto AssembleSubHandPreRatio(
    const int32 subWeaponDmg,
    const int32 bonusBasePhysicalDamage,
    const int32 fSTR) -> int32
{
    return FloorAtZero(subWeaponDmg + bonusBasePhysicalDamage + fSTR);
}

// AssembleRangedAmmoPreRatio mirrors ranged weapon dmg + fSTR (no bonus base SA/TA for ammo path).
inline auto AssembleRangedAmmoPreRatio(const int32 rangedWeaponDmg, const int32 fSTR) -> int32
{
    return FloorAtZero(rangedWeaponDmg + fSTR);
}

// FloorProduct mirrors floor(damage * mult) as uint32 floor then cast.
inline auto FloorProduct(const int32 damage, const float mult) -> int32
{
    if (damage <= 0)
    {
        return 0;
    }
    return static_cast<int32>(std::floor(static_cast<float>(damage) * mult));
}

// ScarletDeliriumMultiplier mirrors 1.0 + power/1000.
inline auto ScarletDeliriumMultiplier(const uint16 effectPower) -> float
{
    float mult = 1.0f + static_cast<float>(effectPower) / 1000.0f;
    return mult < 0.0f ? 0.0f : mult;
}

// ShouldApplyScarletDelirium mirrors has ScarletDelirium1.
inline auto ShouldApplyScarletDelirium(const bool hasScarletDelirium1) -> bool
{
    return hasScarletDelirium1;
}

// MultiAttackDmgModFraction mirrors max(mod/100, 0) for DA/TA dmg mods.
inline auto MultiAttackDmgModFraction(const int16 dmgMod) -> float
{
    float bonus = static_cast<float>(dmgMod) / 100.0f;
    return bonus < 0.0f ? 0.0f : bonus;
}

// ApplyDoubleTripleAttackDamage mirrors LSB:
// floor(damage * 1.0f + max(mod/100, 0)) — additive, not multiplicative.
// Preserves the production quirk where the mod adds a fractional amount after *1.0.
inline auto ApplyDoubleTripleAttackDamage(const int32 damage, const int16 dmgMod) -> int32
{
    float result = static_cast<float>(damage) * 1.0f + MultiAttackDmgModFraction(dmgMod);
    return static_cast<int32>(std::floor(result));
}

// ShouldApplyDoubleAttackDamage mirrors DOUBLE type && PC.
inline auto ShouldApplyDoubleAttackDamage(const uint8 attackType, const bool isPC) -> bool
{
    return attackType == AttackTypeDouble && isPC;
}

// ShouldApplyTripleAttackDamage mirrors TRIPLE type && PC.
inline auto ShouldApplyTripleAttackDamage(const uint8 attackType, const bool isPC) -> bool
{
    return attackType == AttackTypeTriple && isPC;
}

// ShouldApplySoulEater mirrors TYPE_PC.
inline auto ShouldApplySoulEater(const bool isPC) -> bool
{
    return isPC;
}

// ShouldApplySAAugment mirrors AUGMENTS_SA > 0 && isSA && has SA effect.
inline auto ShouldApplySAAugment(
    const int16 augmentsSAMod,
    const bool isSneakAttack,
    const bool hasSneakAttackEffect) -> bool
{
    return augmentsSAMod > 0 && isSneakAttack && hasSneakAttackEffect;
}

// ShouldApplyTAAugment mirrors AUGMENTS_TA > 0 && isTA && has TA effect.
inline auto ShouldApplyTAAugment(
    const int16 augmentsTAMod,
    const bool isTrickAttack,
    const bool hasTrickAttackEffect) -> bool
{
    return augmentsTAMod > 0 && isTrickAttack && hasTrickAttackEffect;
}

// AugmentDamageMultiplier mirrors 1.0 + max(mod/100, 0) for SA/TA augments.
inline auto AugmentDamageMultiplier(const int16 augmentMod) -> float
{
    return 1.0f + MultiAttackDmgModFraction(augmentMod);
}

// ClampNonNegativeDamage mirrors final m_damage < 0 → 0.
inline auto ClampNonNegativeDamage(const int32 damage) -> int32
{
    return FloorAtZero(damage);
}

// IsKickAttackType mirrors KICK.
inline auto IsKickAttackType(const uint8 attackType) -> bool
{
    return attackType == AttackTypeKick;
}

// --- Slice 1577: CalculateAttackDamage pure product ---

constexpr uint8 SlotMain = 0;
constexpr uint8 SlotSub  = 1;
constexpr uint8 SlotAmmo = 3;

struct AttackDamageParams
{
    bool  isSneakAttack{};
    bool  isTrickAttack{};
    int16 dex{};
    int16 sneakAtkDex{};
    int16 agi{};
    int16 trickAtkAgi{};
    int32 consumeMana{};

    bool  isH2H{};
    bool  isMob{};
    bool  isKick{};
    uint8 slot{};

    int32 weaponDmg{};
    int32 naturalH2H{};
    int32 kickDamageMod{};
    int32 fSTR{};
    float mobH2HPenalty{ 1.0f };
    float damageRatio{ 1.0f };

    float scarletMult{ 1.0f };

    uint8 attackType{};
    bool  isPC{};
    int16 doubleAttackDmg{};
    int16 tripleAttackDmg{};

    int32 soulEater{};

    bool  useDamageMultipliers{};
    int32 damageAfterMultipliers{};

    int16 augmentsSA{};
    bool  hasSneakAttackEffect{};
    int16 augmentsTA{};
    bool  hasTrickAttackEffect{};
};

// CalculateAttackDamage pure product (parity: internal/attack CalculateAttackDamage).
inline auto CalculateAttackDamage(const AttackDamageParams& p) -> int32
{
    int32 bonus = 0;
    if (p.isSneakAttack)
    {
        bonus += static_cast<int32>(std::floor(SneakAttackDexBonus(p.dex, p.sneakAtkDex)));
    }
    if (p.isTrickAttack)
    {
        bonus += static_cast<int32>(std::floor(TrickAttackAgiBonus(p.agi, p.trickAtkAgi)));
    }
    bonus += p.consumeMana;

    int32 damage = 0;
    if (p.isH2H && p.isMob)
    {
        const float pre = AssembleMobH2HDamagePreRatio(
            p.weaponDmg + bonus, p.isKick, p.kickDamageMod, p.fSTR, p.mobH2HPenalty);
        damage = ApplyDamageRatio(FloorAtZero(static_cast<int32>(pre)), p.damageRatio);
    }
    else if (p.isH2H && p.isKick)
    {
        damage = AssemblePlayerH2HKickPreRatio(p.naturalH2H, p.kickDamageMod, bonus, p.fSTR);
        damage = ApplyDamageRatio(damage, p.damageRatio);
    }
    else if (p.isH2H)
    {
        damage = AssemblePlayerH2HPunchPreRatio(p.weaponDmg, p.naturalH2H, bonus, p.fSTR);
        damage = ApplyDamageRatio(damage, p.damageRatio);
    }
    else if (p.slot == SlotMain)
    {
        damage = AssembleMainHandPreRatio(p.weaponDmg, bonus, p.fSTR);
        damage = ApplyDamageRatio(damage, p.damageRatio);
    }
    else if (p.slot == SlotSub)
    {
        damage = AssembleSubHandPreRatio(p.weaponDmg, bonus, p.fSTR);
        damage = ApplyDamageRatio(damage, p.damageRatio);
    }
    else if (p.slot == SlotAmmo)
    {
        damage = AssembleRangedAmmoPreRatio(p.weaponDmg, p.fSTR);
        damage = ApplyDamageRatio(damage, p.damageRatio);
    }

    if (p.scarletMult > 0.0f && p.scarletMult != 1.0f)
    {
        damage = FloorProduct(damage, p.scarletMult);
    }

    if (ShouldApplyDoubleAttackDamage(p.attackType, p.isPC))
    {
        damage = ApplyDoubleTripleAttackDamage(damage, p.doubleAttackDmg);
    }
    else if (ShouldApplyTripleAttackDamage(p.attackType, p.isPC))
    {
        damage = ApplyDoubleTripleAttackDamage(damage, p.tripleAttackDmg);
    }

    damage += p.soulEater;

    if (p.useDamageMultipliers)
    {
        damage = p.damageAfterMultipliers;
    }

    if (ShouldApplySAAugment(p.augmentsSA, p.isSneakAttack, p.hasSneakAttackEffect))
    {
        damage = FloorProduct(damage, AugmentDamageMultiplier(p.augmentsSA));
    }
    if (ShouldApplyTAAugment(p.augmentsTA, p.isTrickAttack, p.hasTrickAttackEffect))
    {
        damage = FloorProduct(damage, AugmentDamageMultiplier(p.augmentsTA));
    }

    return ClampNonNegativeDamage(damage);
}

// --- Slice 1379: GetHitRate path selection pure policy ---

// Hit-rate path / hand indices passed to battleutils::GetHitRate.
constexpr uint8 HitRateHandRight = 0;
constexpr uint8 HitRateHandLeft  = 1;
constexpr uint8 HitRateHandKick  = 2;

// Zanshin accuracy bonus inject for GetHitRate optional acc parameter.
constexpr uint8 ZanshinHitRateAccBonus = 35;

// Sange base accuracy bonus and per-merit past-first increment.
constexpr int16 SangeBaseAccBonus      = 100;
constexpr int16 SangeAccBonusPerMerit  = 25;

// PHYSICAL_ATTACK_DIRECTION pins.
constexpr uint8 AttackDirectionLeft  = 0;
constexpr uint8 AttackDirectionRight = 1;

// PHYSICAL_ATTACK_TYPE::ZANSHIN pin.
constexpr uint8 AttackTypeZanshin = 3;

// HitRatePath identifies which battleutils hit-rate helper path to take.
enum class HitRatePath : uint8
{
    KickMelee   = 0, // GetHitRate(..., hand=2)
    DakenRanged = 1, // GetRangedHitRate with sange acc bonus
    RightMelee  = 2, // GetHitRate hand=0, optional zanshin +35
    LeftMelee   = 3, // GetHitRate hand=1, optional zanshin +35
};

// ResolveHitRatePath mirrors GetHitRate branching order.
inline auto ResolveHitRatePath(const uint8 attackType, const uint8 attackDirection) -> HitRatePath
{
    if (attackType == AttackTypeKick)
    {
        return HitRatePath::KickMelee;
    }
    if (attackType == AttackTypeDaken)
    {
        return HitRatePath::DakenRanged;
    }
    if (attackDirection == AttackDirectionRight)
    {
        return HitRatePath::RightMelee;
    }
    return HitRatePath::LeftMelee;
}

// HitRateHandForPath returns battleutils hand index (0/1/2). Daken returns 0 unused.
inline auto HitRateHandForPath(const HitRatePath path) -> uint8
{
    switch (path)
    {
        case HitRatePath::KickMelee:
            return HitRateHandKick;
        case HitRatePath::RightMelee:
            return HitRateHandRight;
        case HitRatePath::LeftMelee:
            return HitRateHandLeft;
        default:
            return HitRateHandRight;
    }
}

// ShouldApplyZanshinAccBonus mirrors attack type == ZANSHIN on melee paths.
inline auto ShouldApplyZanshinAccBonus(const uint8 attackType) -> bool
{
    return attackType == AttackTypeZanshin;
}

// ZanshinAccBonusOrZero returns 35 when zanshin, else 0.
inline auto ZanshinAccBonusOrZero(const uint8 attackType) -> uint8
{
    return ShouldApplyZanshinAccBonus(attackType) ? ZanshinHitRateAccBonus : static_cast<uint8>(0);
}

// ComputeSangeAccBonus mirrors 100 + (meritValue-1)*25 when sange active with merits.
// When sange is inactive, base 100 is still used for Daken ranged hit rate.
inline auto ComputeSangeAccBonus(const bool hasSange, const bool hasMeritPoints, const int32 meritValue) -> int16
{
    int16 accBonus = SangeBaseAccBonus;
    if (hasSange && hasMeritPoints)
    {
        accBonus = static_cast<int16>(accBonus + (meritValue - 1) * SangeAccBonusPerMerit);
    }
    return accBonus;
}

// ShouldStampSATAOnPerfectHit mirrors right-hand path when hitRate == 100.
inline auto ShouldStampSATAOnPerfectHit(const HitRatePath path, const uint8 hitRate) -> bool
{
    return path == HitRatePath::RightMelee && hitRate == 100;
}

// IsRightAttackDirection / IsLeftAttackDirection helpers.
inline auto IsRightAttackDirection(const uint8 direction) -> bool
{
    return direction == AttackDirectionRight;
}

inline auto IsLeftAttackDirection(const uint8 direction) -> bool
{
    return direction == AttackDirectionLeft;
}


} // namespace attackhelpers
