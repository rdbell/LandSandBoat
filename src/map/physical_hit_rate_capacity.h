#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure physical hit-rate helpers from physical_hit_rate.lua.
// Parity: internal/physhitrate + internal/hitratemod
// Host injects ACC/EVA/RACC, status/merit/JP/geometry, distance, level-correction zone.

namespace physicalhitratehelpers
{

constexpr double CapPet          = 0.99;
constexpr double CapPCH2H        = 0.99;
constexpr double CapPCMainhand1H = 0.99;
constexpr double CapPCOffhandOr2H = 0.95;
constexpr double CapNonPC        = 0.95;

constexpr double MeleeHitRateFloor  = 0.2;
constexpr double RangedHitRateFloor = 0.05;
constexpr double RangedHitRateCap   = 0.95;

constexpr double MaxRangedDistance = 25.0;

constexpr double FlashReductionPerMillisecond = 0.03;
constexpr int    LevelCorrectionAccPerLevel   = 4;
constexpr int    AvatarDlvlMin                = 0;
constexpr int    AvatarDlvlMax                = 38;
constexpr double BaseHitPercent               = 75.0;

// Geometry half-angles (host inject documentation).
constexpr std::uint8_t BehindAngle     = 23;
constexpr std::uint8_t FacingConeYonin = 64;
constexpr std::uint8_t FacingDefault   = 64; // isFacing without angle

constexpr int BuildingFlourishBase           = 40;
constexpr int BuildingFlourishSubPowerScale  = 2;
constexpr int YoninJPScale                   = 2;

// attackNumber / attack animation: 0 main, 1 left/offhand, 2 kick.
constexpr std::uint8_t AttackSlotMain = 0;
constexpr std::uint8_t AttackSlotLeft = 1;

inline auto ClampFloat(const double v, const double lo, const double hi) -> double
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

inline auto ClampInt(const int v, const int lo, const int hi) -> int
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

// getPhysicalHitRateCap
inline auto HitRateCap(const bool isPet, const bool isPC, const bool usingH2H, const bool isTwoHanded, const bool slotLeftOrHigher) -> double
{
    if (isPet)
    {
        return CapPet;
    }
    if (isPC)
    {
        if (usingH2H)
        {
            return CapPCH2H;
        }
        if (isTwoHanded || slotLeftOrHigher)
        {
            return CapPCOffhandOr2H;
        }
        return CapPCMainhand1H;
    }
    return CapNonPC;
}

// getFlashPenalty once time remaining (ms) is known; 0 when no Flash / non-positive.
inline auto FlashPenalty(const std::int64_t timeRemainingMs) -> int
{
    if (timeRemainingMs <= 0)
    {
        return 0;
    }
    return static_cast<int>(std::floor(static_cast<double>(timeRemainingMs) * FlashReductionPerMillisecond));
}

// Level-correction branch of accuracyAndEvasionToHitRate.
inline auto LevelCorrectedAccuracy(int acc, const std::uint8_t atkLvl, const std::uint8_t defLvl, const bool applyLevelCorrection, const bool isPC, const bool isAvatar) -> int
{
    if (!applyLevelCorrection)
    {
        return acc;
    }
    int dlvl = static_cast<int>(atkLvl) - static_cast<int>(defLvl);
    if (isAvatar)
    {
        dlvl = ClampInt(dlvl, AvatarDlvlMin, AvatarDlvlMax);
    }
    if (!isPC && atkLvl > defLvl)
    {
        acc += dlvl * LevelCorrectionAccPerLevel;
    }
    else if (isPC && atkLvl < defLvl)
    {
        acc += dlvl * LevelCorrectionAccPerLevel;
    }
    return acc;
}

inline auto AccuracyEvasionToHitRate(const int acc, const int eva) -> double
{
    const double hitdiff = static_cast<double>(acc - eva) / 2.0;
    return (BaseHitPercent + hitdiff) / 100.0;
}

inline auto ClampMeleeHitRate(const double hitrate, const double cap) -> double
{
    return ClampFloat(hitrate, MeleeHitRateFloor, cap);
}

inline auto ClampRangedHitRate(const double hitrate) -> double
{
    return ClampFloat(hitrate, RangedHitRateFloor, RangedHitRateCap);
}

// getHitRateModifiers inject form (hitratemod.HitRateModifiers).
struct HitRateModParams
{
    bool isRanged{};
    bool isWeaponskill{};

    bool hasBuildingFlourish{};
    int  buildingFlourishPower{};
    int  buildingFlourishSubPower{};

    bool hasInnin{};
    int  inninPower{};
    int  inninJP{}; // target JP.INNIN_EFFECT

    bool attackerIsPC{};
    bool attackerIsFacing{};
    int  attackerClosedPositionMerit{};

    bool hasAmbushTrait{};
    int  ambushMerit{};
    bool isBehind23{};

    bool hasYonin{};
    int  yoninPower{};
    int  yoninJP{}; // target JP.YONIN_EFFECT
    bool isFacing64{};

    bool targetIsPC{};
    bool targetIsFacing{};
    int  targetClosedPositionMerit{};

    int flashPenalty{};
};

struct HitRateModResult
{
    int accBonus{};
    int evaBonus{};
};

inline auto HitRateModifiers(const HitRateModParams& p) -> HitRateModResult
{
    int accBonus = 0;
    int evaBonus = 0;

    if (!p.isRanged)
    {
        if (p.isWeaponskill && p.hasBuildingFlourish && p.buildingFlourishPower >= 1)
        {
            accBonus = BuildingFlourishBase + p.buildingFlourishSubPower * BuildingFlourishSubPowerScale;
        }
        if (p.hasInnin && p.isBehind23)
        {
            accBonus += p.inninPower + p.inninJP;
        }
        if (p.attackerIsPC && p.attackerIsFacing)
        {
            accBonus += p.attackerClosedPositionMerit;
        }
    }

    if (p.hasAmbushTrait && p.isBehind23)
    {
        accBonus += p.ambushMerit;
    }

    if (p.hasYonin && p.isFacing64)
    {
        evaBonus += p.yoninPower + YoninJPScale * p.yoninJP;
    }

    if (p.targetIsPC && p.targetIsFacing)
    {
        evaBonus += p.targetClosedPositionMerit;
    }

    accBonus -= p.flashPenalty;
    return HitRateModResult{ accBonus, evaBonus };
}

struct MeleeHitRateParams
{
    int  acc{};
    int  eva{};
    int  bonus{};
    int  accBonus{};
    int  evaBonus{};
    double cap{ CapNonPC };
    bool applyLevelCorrection{};
    std::uint8_t attackerLevel{};
    std::uint8_t defenderLevel{};
    bool attackerIsPC{};
    bool attackerIsAvatar{};
};

inline auto MeleeHitRate(const MeleeHitRateParams& p) -> double
{
    int acc = p.acc + p.bonus + p.accBonus;
    int eva = p.eva + p.evaBonus;
    acc     = LevelCorrectedAccuracy(acc, p.attackerLevel, p.defenderLevel, p.applyLevelCorrection, p.attackerIsPC, p.attackerIsAvatar);
    return ClampMeleeHitRate(AccuracyEvasionToHitRate(acc, eva), p.cap);
}

struct RangedHitRateParams
{
    int  acc{};
    int  eva{};
    int  bonus{};
    int  accBonus{};
    int  evaBonus{};
    int  distancePenalty{};
    double distance{};
    bool applyLevelCorrection{};
    std::uint8_t attackerLevel{};
    std::uint8_t defenderLevel{};
    bool attackerIsPC{};
    bool attackerIsAvatar{};
};

inline auto RangedHitRate(const RangedHitRateParams& p) -> double
{
    if (p.distance > MaxRangedDistance)
    {
        return 0.0;
    }
    int acc = p.acc + p.bonus + p.accBonus - p.distancePenalty;
    int eva = p.eva + p.evaBonus;
    acc     = LevelCorrectedAccuracy(acc, p.attackerLevel, p.defenderLevel, p.applyLevelCorrection, p.attackerIsPC, p.attackerIsAvatar);
    return ClampRangedHitRate(AccuracyEvasionToHitRate(acc, eva));
}

// Floor product to percent uint8 (GetHitRateEx / GetRangedHitRate conversion).
inline auto HitRateToPercent(const double hitRate) -> std::uint8_t
{
    const auto hitrate = static_cast<std::int32_t>(std::floor(hitRate * 100.0));
    if (hitrate < 0)
    {
        return 0;
    }
    if (hitrate > 255)
    {
        return 255;
    }
    return static_cast<std::uint8_t>(hitrate);
}

// Ranged accuracy distance penalty pure (rangeddist.AccuracyPenalty).
// sweetSpotEnd is pre-resolved band end before hitbox expansion.
inline auto AccuracyDistancePenalty(const bool isPC, const double distance, const double sweetSpotEnd, const double defenderHitbox, const double attackerHitbox, const std::uint8_t mainLvl) -> int
{
    if (!isPC)
    {
        return 0;
    }
    const double centroidEnd = sweetSpotEnd + defenderHitbox + attackerHitbox;
    if (distance <= centroidEnd)
    {
        return 0;
    }
    const double denom = MaxRangedDistance - centroidEnd;
    if (denom <= 0.0)
    {
        return 0;
    }
    const double penaltyPercentage = (distance - centroidEnd) / denom;
    return static_cast<int>(std::fabs(std::floor(penaltyPercentage * (static_cast<double>(mainLvl) / 2.0))));
}

// Attack distance penalty pure (rangeddist.AttackPenalty).
// sweetSpotStart/End before hitbox pad; cSkillMax is getMaxSkillLevel(mainLvl, WAR, EVASION).
constexpr double MaxInnerAttackPenalty = 25.0;
constexpr double MaxOuterAttackPenalty = 20.0;

inline auto AttackDistancePenalty(const bool isPC, const double distance, const double sweetSpotStart, const double sweetSpotEnd, const double defenderHitbox, const double attackerHitbox, const int cSkillMax) -> int
{
    if (!isPC)
    {
        return 0;
    }
    const double pad           = defenderHitbox + attackerHitbox;
    const double centroidStart = sweetSpotStart + pad;
    const double centroidEnd   = sweetSpotEnd + pad;
    double       penaltyPct    = 0.0;
    if (distance < centroidStart)
    {
        // Linear from -maxInner at 0 to 0 at centroidStart
        if (centroidStart <= 0.0)
        {
            penaltyPct = -MaxInnerAttackPenalty;
        }
        else
        {
            penaltyPct = -MaxInnerAttackPenalty + (MaxInnerAttackPenalty * (distance / centroidStart));
        }
    }
    else if (distance <= centroidEnd)
    {
        penaltyPct = 0.0;
    }
    else
    {
        const double denom = MaxRangedDistance - centroidEnd;
        if (denom <= 0.0)
        {
            penaltyPct = MaxOuterAttackPenalty;
        }
        else
        {
            penaltyPct = MaxOuterAttackPenalty * (distance - centroidEnd) / denom;
        }
    }
    return static_cast<int>(std::fabs(std::ceil((penaltyPct / 100.0) * static_cast<double>(cSkillMax))));
}

// Sweet-spot defaults (yalms) matching xi.combat.ranged.sweetSpotDefaults + Yoichi.
struct SweetSpot
{
    double start{};
    double end{};
};

constexpr SweetSpot SweetThrowing{ 0.0, 1.3 };
constexpr SweetSpot SweetCannon{ 3.0, 4.3 };
constexpr SweetSpot SweetGun{ 3.0, 4.3 };
constexpr SweetSpot SweetShortbow{ 4.0, 6.4 };
constexpr SweetSpot SweetCrossbow{ 5.0, 8.4 };
constexpr SweetSpot SweetLongbow{ 6.0, 9.5 };
constexpr SweetSpot SweetYoichi{ 5.5, 9.5 };

constexpr std::uint8_t SkillArchery      = 25;
constexpr std::uint8_t SkillMarksmanship = 26;
constexpr std::uint8_t SubSkillXBowShort = 0;
constexpr std::uint8_t SubSkillGun       = 1;
constexpr std::uint8_t SubSkillCannon    = 2;
constexpr std::uint8_t SubSkillLongbow   = 4;

inline auto IsYoichiWeapon(const std::uint16_t weaponID) -> bool
{
    switch (weaponID)
    {
        case 18348: // YOICHINOYUMI_75
        case 18349:
        case 18650:
        case 18664:
        case 18678:
        case 19759:
        case 19852:
        case 21210:
        case 21211:
        case 22115:
        case 22129:
            return true;
        default:
            return false;
    }
}

inline auto ResolveSweetSpot(const bool hasWeapon, const std::uint16_t weaponID, const std::uint8_t skillType, const std::uint8_t subSkillType) -> SweetSpot
{
    if (IsYoichiWeapon(weaponID))
    {
        return SweetYoichi;
    }
    if (!hasWeapon)
    {
        return SweetThrowing;
    }
    if (skillType == SkillArchery && subSkillType == SubSkillLongbow)
    {
        return SweetLongbow;
    }
    if (skillType == SkillArchery && subSkillType == SubSkillXBowShort)
    {
        return SweetShortbow;
    }
    if (skillType == SkillMarksmanship && subSkillType == SubSkillXBowShort)
    {
        return SweetCrossbow;
    }
    if (skillType == SkillMarksmanship && subSkillType == SubSkillGun)
    {
        return SweetGun;
    }
    if (skillType == SkillMarksmanship && subSkillType == SubSkillCannon)
    {
        return SweetCannon;
    }
    return SweetThrowing;
}


// Third Eye retention (checkAnticipated pure half). Parity: physhitrate.
constexpr double ThirdEyeRetentionLossPerMs   = 1.0 / 300.0;
constexpr int    ThirdEyeRetentionScale       = 100;
constexpr double ThirdEyeRetentionMaxPercent  = 100.0;

// canRetain: !isPC || isTwoHanded
constexpr auto CanRetainThirdEye(const bool isPC, const bool isWeaponTwoHanded) -> bool
{
    return !isPC || isWeaponTwoHanded;
}

// Scaled chance 0..10000 for math.random(1,10000); 0 when Seigan/retain gates fail (caller).
inline auto ThirdEyeRetentionScaledChance(const std::int64_t timeInEffectMs, const int retentionRateMod) -> int
{
    auto t = timeInEffectMs;
    if (t < 0)
    {
        t = 0;
    }
    const double retentionModifier = ClampFloat(1.0 - static_cast<double>(retentionRateMod) / 100.0, 0.0, 1.0);
    double percent = ThirdEyeRetentionMaxPercent - static_cast<double>(t) * ThirdEyeRetentionLossPerMs * retentionModifier;
    percent        = ClampFloat(percent, 0.0, ThirdEyeRetentionMaxPercent);
    return static_cast<int>(std::floor(percent * static_cast<double>(ThirdEyeRetentionScale)));
}

// Retains when scaledChance > 0 and roll1to10000 <= scaledChance.
constexpr auto RetainsThirdEye(const int scaledChance, const int roll1to10000) -> bool
{
    if (scaledChance == 0)
    {
        return false;
    }
    return roll1to10000 <= scaledChance;
}

// checkAnticipated after TE presence is known:
// if seigan && canRetain: scaled = ThirdEyeRetentionScaledChance(...)
// else scaled = 0
// if !RetainsThirdEye(scaled, roll): shouldDeleteTE = true
// always returns anticipated=true when TE was present (caller checked).
struct AnticipateResult
{
    bool anticipated{ true };
    bool shouldDeleteThirdEye{ false };
};

inline auto CheckAnticipatedRetention(const bool hasSeigan, const bool canRetain, const std::int64_t timeInEffectMs, const int retentionRateMod, const int roll1to10000) -> AnticipateResult
{
    int scaled = 0;
    if (hasSeigan && canRetain)
    {
        scaled = ThirdEyeRetentionScaledChance(timeInEffectMs, retentionRateMod);
    }
    AnticipateResult r{};
    r.anticipated           = true;
    r.shouldDeleteThirdEye  = !RetainsThirdEye(scaled, roll1to10000);
    return r;
}

} // namespace physicalhitratehelpers
