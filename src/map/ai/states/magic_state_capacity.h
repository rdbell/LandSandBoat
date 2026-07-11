#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CMagicState policy halves (CanCastSpell gates, HasCost, HasMoved,
// SpendCost arithmetic, GetRecast zero gate, ApplyEnmity CE/VE adjustments).
// Entity/spell lookups, zone misc, status containers, ninja tools inventory,
// and MP mutation remain host-injected.

namespace magicstatehelpers
{

// MAGICFLAGS pins (magic_state.h).
constexpr uint8 MagicFlagsNone        = 0;
constexpr uint8 MagicFlagsIgnoreMP    = 1;
constexpr uint8 MagicFlagsIgnoreTools = 2;

// SPELLGROUP pins used by cost/enmity paths.
constexpr uint8 SpellGroupNinjutsu  = 4;
constexpr uint8 SpellGroupSummoning = 5;
constexpr uint8 SpellGroupWhite     = 6;
constexpr uint8 SpellGroupBlack     = 2;

// SPELLFAMILY pins for Entrust Indi range and Yonin Utsusemi.
constexpr uint16 SpellFamilyUtsusemi   = 96;
constexpr uint16 SpellFamilyIndiBuff   = 144;
constexpr uint16 SpellFamilyIndiDebuff = 145;

// SKILLTYPE pin for Divine Emblem.
constexpr uint8 SkillDivineMagic = 32;

// Distance thresholds from CanCastSpell.
constexpr float MaxSpellTargetDistance = 40.0f;
constexpr float MobSpellMaxDistance    = 28.5f;
constexpr float EntrustIndiRange       = 25.0f;
constexpr float HasMovedDistanceThreshold = 0.3f;

// MsgBasic pins for pure CanCast/HasCost outcomes.
enum class MagicUseFail : uint16
{
    None                 = 0,
    NotEnoughMP          = 34,
    NoNinjaTools         = 35,
    CannotCastSpell      = 47,
    UnableToCastSpells   = 49,
    CannotPerformAction  = 71,
    TooFarAway           = 78,
    CannotOnThatTarget   = 155,
    OutOfRangeUnableCast = 313,
    CannotUseInArea      = 316,
    // Silent reject (no battle message): name hidden or mob out of 28.5.
    SilentReject         = 0xFFFF,
};

// HasFlag mirrors m_flags & mask.
inline auto HasMagicFlag(const uint8 flags, const uint8 mask) -> bool
{
    return (flags & mask) == mask;
}

// MagicHasMoved mirrors TYPE_PC && horizontalDistance > 0.3.
inline auto MagicHasMoved(const bool isPC, const float horizontalDistance) -> bool
{
    if (!isPC)
    {
        return false;
    }
    return horizontalDistance > HasMovedDistanceThreshold;
}

// OmertaBlocksSpell mirrors (1 << (spellGroup - 1)) & omertaPower.
// spellGroup is SPELLGROUP (1-based for Song..); group 0 never blocks.
inline auto OmertaBlocksSpell(const uint8 spellGroup, const int16 omertaPower) -> bool
{
    if (spellGroup == 0)
    {
        return false;
    }
    return ((1 << (spellGroup - 1)) & omertaPower) != 0;
}

// IsIndiSpellFamily mirrors SPELLFAMILY_INDI_BUFF || INDI_DEBUFF.
inline auto IsIndiSpellFamily(const uint16 spellFamily) -> bool
{
    return spellFamily == SpellFamilyIndiBuff || spellFamily == SpellFamilyIndiDebuff;
}

// ResolvePCSpellRange mirrors PC range calc:
//   base = spellRange + targetHitbox + casterHitbox
//   Entrust + Indi → 25.0 (no hitboxes).
inline auto ResolvePCSpellRange(
    const float spellRange,
    const float targetHitbox,
    const float casterHitbox,
    const bool hasEntrust,
    const uint16 spellFamily) -> float
{
    if (hasEntrust && IsIndiSpellFamily(spellFamily))
    {
        return EntrustIndiRange;
    }
    return spellRange + targetHitbox + casterHitbox;
}

// TooFarAbsolute mirrors distance > 40.
inline auto TooFarAbsolute(const float distance) -> bool
{
    return distance > MaxSpellTargetDistance;
}

// OutOfPCSpellRange mirrors distance > resolved PC spell range.
inline auto OutOfPCSpellRange(const float distance, const float resolvedRange) -> bool
{
    return distance > resolvedRange;
}

// OutOfMobSpellRange mirrors !isWithinDistance(..., 28.5) i.e. distanceSq > 28.5^2
// Host may inject either precomputed distance or distanceSq; we use distance
// with the same threshold (isWithinDistance uses <= so > 28.5 is out).
inline auto OutOfMobSpellRange(const float distance) -> bool
{
    return distance > MobSpellMaxDistance;
}

// ShouldCheckPlayerLOS mirrors !isEndOfCast && isPC && losPlayerBlock.
inline auto ShouldCheckPlayerLOS(const bool isEndOfCast, const bool isPC, const bool losPlayerBlock) -> bool
{
    return !isEndOfCast && isPC && losPlayerBlock;
}

// EvaluateHasCost mirrors HasCost pure decision with host-injected afford/tools.
// Returns MagicUseFail::None when cost is available.
inline auto EvaluateHasCost(
    const uint8 spellGroup,
    const bool isPC,
    const uint8 flags,
    const bool hasNinjaTool,
    const bool canAffordSpell) -> MagicUseFail
{
    if (spellGroup == SpellGroupNinjutsu)
    {
        if (isPC && !HasMagicFlag(flags, MagicFlagsIgnoreTools) && !hasNinjaTool)
        {
            return MagicUseFail::NoNinjaTools;
        }
        return MagicUseFail::None;
    }
    if (!canAffordSpell)
    {
        return MagicUseFail::NotEnoughMP;
    }
    return MagicUseFail::None;
}

// EvaluateCanCastSpell is the pure decision ladder of CanCastSpell.
// hasCostFail is the HasCost outcome (None means cost OK).
// nameHidden silent-rejects without a message (original returns false bare).
// isSelf skips distance/LOS after target presence.
inline auto EvaluateCanCastSpell(
    const bool canUseSpell,
    const bool zoneAllowsMisc,
    const bool silencedOrMuted,
    const bool hasOmerta,
    const uint8 spellGroup,
    const int16 omertaPower,
    const MagicUseFail hasCostFail,
    const bool hasTarget,
    const bool nameHidden,
    const bool isSelf,
    const float distance,
    const bool isPC,
    const float pcResolvedRange,
    const bool isMob,
    const bool isEndOfCast,
    const bool losPlayerBlock,
    const bool canSeeTarget) -> MagicUseFail
{
    if (!canUseSpell)
    {
        return MagicUseFail::CannotCastSpell;
    }
    if (!zoneAllowsMisc)
    {
        return MagicUseFail::CannotUseInArea;
    }
    if (silencedOrMuted)
    {
        return MagicUseFail::UnableToCastSpells;
    }
    if (hasOmerta && OmertaBlocksSpell(spellGroup, omertaPower))
    {
        return MagicUseFail::UnableToCastSpells;
    }
    if (hasCostFail != MagicUseFail::None)
    {
        return hasCostFail;
    }
    if (!hasTarget)
    {
        return MagicUseFail::CannotOnThatTarget;
    }
    if (nameHidden)
    {
        return MagicUseFail::SilentReject;
    }
    if (isSelf)
    {
        return MagicUseFail::None;
    }
    if (TooFarAbsolute(distance))
    {
        return MagicUseFail::TooFarAway;
    }
    if (isPC && OutOfPCSpellRange(distance, pcResolvedRange))
    {
        return MagicUseFail::OutOfRangeUnableCast;
    }
    if (isMob && OutOfMobSpellRange(distance))
    {
        return MagicUseFail::SilentReject;
    }
    if (ShouldCheckPlayerLOS(isEndOfCast, isPC, losPlayerBlock) && !canSeeTarget)
    {
        return MagicUseFail::CannotPerformAction;
    }
    return MagicUseFail::None;
}

// ShouldSpendNinjaTools mirrors NINJUTSU && !(flags & IGNORE_TOOLS).
inline auto ShouldSpendNinjaTools(const uint8 spellGroup, const uint8 flags) -> bool
{
    return spellGroup == SpellGroupNinjutsu && !HasMagicFlag(flags, MagicFlagsIgnoreTools);
}

// ShouldSpendMP mirrors hasMPCost && !manafont && !(flags & IGNORE_MP).
inline auto ShouldSpendMP(const bool hasMPCost, const bool hasManafont, const uint8 flags) -> bool
{
    return hasMPCost && !hasManafont && !HasMagicFlag(flags, MagicFlagsIgnoreMP);
}

// ApplyQuickMagicCost mirrors:
//   cost * (1.0f - (float)((jp * 2) / 100))  with integer (jp*2)/100.
inline auto ApplyQuickMagicCost(const int16 cost, const uint8 quickMagicJP) -> int16
{
    const auto reduction = static_cast<float>((static_cast<int>(quickMagicJP) * 2) / 100);
    return static_cast<int16>(cost * (1.0f - reduction));
}

// ShouldApplyConserveMP mirrors roll0to99 < rate.
inline auto ShouldApplyConserveMP(const int16 rate, const int roll0to99) -> bool
{
    return roll0to99 < rate;
}

// ApplyConserveMP mirrors cost * (factor / 16.0f) with factor in [8, 16).
// Host injects the float draw; we clamp into the live band for safety.
inline auto ApplyConserveMP(const int16 cost, const float factor8to16) -> int16
{
    float f = factor8to16;
    if (f < 8.0f)
    {
        f = 8.0f;
    }
    if (f >= 16.0f)
    {
        f = 15.999f; // stay below exclusive upper of GetRandomNumber(8,16)
    }
    return static_cast<int16>(cost * (f / 16.0f));
}

// RecastIsZero mirrors Chainspell || Spontaneity || instantCast.
inline auto RecastIsZero(const bool hasChainspell, const bool hasSpontaneity, const bool instantCast) -> bool
{
    return hasChainspell || hasSpontaneity || instantCast;
}

// ApplySubtleSorceryCE forces CE to 0 when Subtle Sorcery is active.
inline auto ApplySubtleSorceryCE(const int ce, const bool hasSubtleSorcery) -> int
{
    return hasSubtleSorcery ? 0 : ce;
}

// ApplyYoninUtsusemiEnmity overrides CE/VE when Yonin + Utsusemi + mod > 0.
// Returns true when override applied; host writes ce=160, ve=480.
inline auto ShouldApplyYoninUtsusemiEnmity(
    const bool hasYonin,
    const uint16 spellFamily,
    const int16 yoninUtsusemiMod) -> bool
{
    return hasYonin && spellFamily == SpellFamilyUtsusemi && yoninUtsusemiMod > 0;
}

// Yonin Utsusemi CE/VE pins.
constexpr int YoninUtsusemiCE = 160;
constexpr int YoninUtsusemiVE = 480;

// ApplyDivineEmblemEnmity multiplies CE/VE by (1 + power/100).
inline auto ApplyDivineEmblemScale(const int value, const int16 power) -> int
{
    return static_cast<int>(value * (1.0f + (power / 100.0f)));
}

// ShouldApplyDivineEmblem mirrors hasDivineEmblem && skill == DIVINE_MAGIC.
inline auto ShouldApplyDivineEmblem(const bool hasDivineEmblem, const uint8 skillType) -> bool
{
    return hasDivineEmblem && skillType == SkillDivineMagic;
}

// DivineBenisonEnmityDelta mirrors -(DIVINE_BENISON >> 1) for Na spells.
inline auto DivineBenisonEnmityDelta(const int16 divineBenisonMod) -> int16
{
    return static_cast<int16>(-(divineBenisonMod >> 1));
}

// ShouldSkipSummoningEnmity mirrors SPELLGROUP_SUMMONING early return.
inline auto ShouldSkipSummoningEnmity(const uint8 spellGroup) -> bool
{
    return spellGroup == SpellGroupSummoning;
}

// ShouldGenerateMobEnmity mirrors !isHeal || tookEffect for hostile mob target.
inline auto ShouldGenerateMobEnmity(const bool isHeal, const bool tookEffect) -> bool
{
    return !isHeal || tookEffect;
}

// ShouldSkipUncharmedMobCaster mirrors isMob && !isCharmed (no claim/enmity).
inline auto ShouldSkipUncharmedMobCaster(const bool casterIsMob, const bool casterIsCharmed) -> bool
{
    return casterIsMob && !casterIsCharmed;
}

// ShouldClaimOnDeath mirrors target dead && (!isMob || (isMob && charmed)).
inline auto ShouldClaimOnDeath(const bool targetDead, const bool casterIsMob, const bool casterIsCharmed) -> bool
{
    return targetDead && (!casterIsMob || (casterIsMob && casterIsCharmed));
}

// ShouldApplyTranquility mirrors hasTranquility && WHITE group.
inline auto ShouldApplyTranquility(const bool hasTranquility, const uint8 spellGroup) -> bool
{
    return hasTranquility && spellGroup == SpellGroupWhite;
}

// ShouldApplyEquanimity mirrors hasEquanimity && BLACK group.
inline auto ShouldApplyEquanimity(const bool hasEquanimity, const uint8 spellGroup) -> bool
{
    return hasEquanimity && spellGroup == SpellGroupBlack;
}

} // namespace magicstatehelpers
