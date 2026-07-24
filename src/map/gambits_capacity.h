#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>

// Pure gambit trigger-condition gates extracted so native tests can pin policy
// without a live battle entity, status-effect container, or enmity controller.
//
// Dual-wire pure free functions (OmegaXI slice 6636):
//   - MaxRuneEffects / CanUseRunes            (G_CONDITION::NO_MAX_RUNE)
//   - HasTopEnmity / NotHasTopEnmity          (G_CONDITION::HAS_TOP_ENMITY,
//                                              NOT_HAS_TOP_ENMITY)
//   - SkillchainReady / SkillchainAvailable /
//     MagicBurstAvailable / NoSkillchain      (G_CONDITION::SC_AVAILABLE,
//                                              NOT_SC_AVAILABLE, MB_AVAILABLE)
//
// Production host: CGambitsContainer::CheckTrigger (ai/helpers/gambits_container.cpp)
// injects the rune count, owner main job/level, the top-enmity targid, and the
// skillchain effect's age and tier.
// Go dual-wire: internal/gambits EvaluatePredicate condition families.
namespace gambitshelpers
{
// Runemaster level thresholds for a third and second concurrent rune.
inline constexpr uint8 RuneJobRun          = 22; // JOB_RUN
inline constexpr uint8 RuneThirdRuneLevel  = 65;
inline constexpr uint8 RuneSecondRuneLevel = 35;

// A skillchain effect must have been up this long before it can be closed or
// burst (CheckTrigger compares GetStartTime() + 3s < now()).
//
// Expressed in milliseconds so the strict comparison is preserved exactly: a
// whole-second age would round 3.5s down to 3 and wrongly reject it.
inline constexpr uint32 SkillchainReadyDelayMs = 3000;

// MaxRuneEffects is the number of runes the owner may hold at once. Only a main
// Runemaster gains more than one, at level 35 and again at 65.
inline auto MaxRuneEffects(uint8 mainJob, uint8 mainLevel) -> std::size_t
{
    if (mainJob != RuneJobRun)
    {
        return 1;
    }

    if (mainLevel >= RuneThirdRuneLevel)
    {
        return 3;
    }

    if (mainLevel >= RuneSecondRuneLevel)
    {
        return 2;
    }

    return 1;
}

// CanUseRunes is true while the target holds fewer than its maximum.
inline auto CanUseRunes(std::size_t runeCount, std::size_t maxRuneEffects) -> bool
{
    return runeCount < maxRuneEffects;
}

// HasTopEnmity is true only when an enmity holder exists and it is the owner.
//
// NOTE: with no enmity holder at all, both HasTopEnmity and NotHasTopEnmity are
// false — NotHasTopEnmity is not the negation of HasTopEnmity. Upstream returns
// false from both when GetTopEnmity() is null.
inline auto HasTopEnmity(bool hasTopEnmity, uint16 topEnmityTargId, uint16 ownerTargId) -> bool
{
    return hasTopEnmity ? topEnmityTargId == ownerTargId : false;
}

inline auto NotHasTopEnmity(bool hasTopEnmity, uint16 topEnmityTargId, uint16 ownerTargId) -> bool
{
    return hasTopEnmity ? topEnmityTargId != ownerTargId : false;
}

// SkillchainReady is the shared age gate for closing or bursting.
inline auto SkillchainReady(bool hasSkillchain, uint32 ageMs) -> bool
{
    return hasSkillchain && ageMs > SkillchainReadyDelayMs;
}

// SkillchainAvailable admits a skillchain open (tier 0) once the effect is old
// enough.
inline auto SkillchainAvailable(bool hasSkillchain, uint32 ageMs, uint16 tier) -> bool
{
    return SkillchainReady(hasSkillchain, ageMs) && tier == 0;
}

// MagicBurstAvailable admits a burst on a closed skillchain (tier above 0).
inline auto MagicBurstAvailable(bool hasSkillchain, uint32 ageMs, uint16 tier) -> bool
{
    return SkillchainReady(hasSkillchain, ageMs) && tier > 0;
}

// NoSkillchain is true only when no skillchain effect is present at all.
//
// NOTE: this ignores the age and tier gates, so it is not the negation of
// SkillchainAvailable — a fresh or already-closed skillchain makes both false.
inline auto NoSkillchain(bool hasSkillchain) -> bool
{
    return !hasSkillchain;
}

// Party role main jobs (slice 6637). PartyHasHealer/PartyHasTank fold these
// over ForPartyWithTrusts.
inline constexpr uint8 JobWhm = 3;
inline constexpr uint8 JobRdm = 5;
inline constexpr uint8 JobPld = 7;
inline constexpr uint8 JobNin = 13;
inline constexpr uint8 JobSch = 20;
inline constexpr uint8 JobRun = 22;

// IsHealerJob classifies a member's main job for PartyHasHealer, used to pick
// Uka Totlihn's samba.
//
// NOTE: Paladin counts as both a healer and a tank.
inline auto IsHealerJob(uint8 mainJob) -> bool
{
    return mainJob == JobWhm || mainJob == JobRdm || mainJob == JobPld || mainJob == JobSch;
}

// IsTankJob classifies a member's main job for PartyHasTank (Volker, AA Hume).
inline auto IsTankJob(uint8 mainJob) -> bool
{
    return mainJob == JobNin || mainJob == JobPld || mainJob == JobRun;
}

// Casting condition gates (slice 6638). The CASTING_* family all first require
// the trigger target to be in CMagicState, then inspect the spell being cast.
inline constexpr uint8 ElementFire  = 1; // ELEMENT_FIRE
inline constexpr uint8 ElementWater = 6; // ELEMENT_WATER
inline constexpr uint8 SpellAoeRadial = 1; // SPELLAOE_RADIAL

// IsElementalMagic covers the six offensive elements Fire..Water. Light and
// Dark are deliberately outside the range, matching the upstream bounds check.
inline auto IsElementalMagic(uint8 spellElement) -> bool
{
    return spellElement >= ElementFire && spellElement <= ElementWater;
}

// CastingDebuff is G_CONDITION::CASTING_DEBUFF.
inline auto CastingDebuff(bool isCastingMagic, bool spellIsDebuff) -> bool
{
    return isCastingMagic && spellIsDebuff;
}

// CastingElementalMagic is G_CONDITION::CASTING_ELEMENT_MA.
inline auto CastingElementalMagic(bool isCastingMagic, uint8 spellElement) -> bool
{
    return isCastingMagic && IsElementalMagic(spellElement);
}

// CastingElementalAoe is G_CONDITION::CASTING_ELE_MA_AOE: an elemental spell
// with radial area of effect.
inline auto CastingElementalAoe(bool isCastingMagic, uint8 spellElement, uint8 spellAoeType) -> bool
{
    return CastingElementalMagic(isCastingMagic, spellElement) && spellAoeType == SpellAoeRadial;
}

// CastingElementalOnSelf is G_CONDITION::CAST_ELE_MA_SELF: an elemental spell
// aimed at the gambit owner.
inline auto CastingElementalOnSelf(bool isCastingMagic, uint8 spellElement, uint32 spellTargetId, uint32 ownerId) -> bool
{
    return CastingElementalMagic(isCastingMagic, spellElement) && spellTargetId == ownerId;
}

// Bar-effect and Lunge gates (slice 6639).

// Bar-spell status effects, indexed by the incoming spell's element.
inline constexpr uint16 StatusBarfire      = 100;
inline constexpr uint16 StatusBarblizzard  = 101;
inline constexpr uint16 StatusBaraero      = 102;
inline constexpr uint16 StatusBarstone     = 103;
inline constexpr uint16 StatusBarthunder   = 104;
inline constexpr uint16 StatusBarwater     = 105;

// BarEffectForElement maps an incoming elemental spell to the bar effect that
// resists it, or 0 for a non-elemental spell.
inline auto BarEffectForElement(uint8 spellElement) -> uint16
{
    switch (spellElement)
    {
        case 1: return StatusBarfire;     // ELEMENT_FIRE
        case 2: return StatusBarblizzard; // ELEMENT_ICE
        case 3: return StatusBaraero;     // ELEMENT_WIND
        case 4: return StatusBarstone;    // ELEMENT_EARTH
        case 5: return StatusBarthunder;  // ELEMENT_THUNDER
        case 6: return StatusBarwater;    // ELEMENT_WATER
        default: return 0;
    }
}

// NeedBarEffect is G_CONDITION::NEED_ELE_BAREFFECT: the owner lacks the bar
// effect resisting the spell being cast.
//
// NOTE: a non-elemental spell yields false, and upstream then overwrites the
// recorded cast element with the day's element rather than the spell's. That
// side effect stays with the host; only the predicate is pure here.
inline auto NeedBarEffect(bool isCastingMagic, uint8 spellElement, bool ownerHasBarEffect) -> bool
{
    if (!isCastingMagic || BarEffectForElement(spellElement) == 0)
    {
        return false;
    }

    return !ownerHasBarEffect;
}

// Skillchain elements Lunge can burst, and the runes matching each.
inline constexpr uint8 ScLight      = 13;
inline constexpr uint8 ScDarkness   = 14;
inline constexpr uint8 ScLightII    = 15;
inline constexpr uint8 ScDarknessII = 16;

// MinLungeSkillchainTier is the skillchain tier Lunge requires; only Light and
// Darkness reach it.
inline constexpr uint8 MinLungeSkillchainTier = 3;

inline auto IsLightSkillchain(uint8 skillchainElement) -> bool
{
    return skillchainElement == ScLight || skillchainElement == ScLightII;
}

inline auto IsDarkSkillchain(uint8 skillchainElement) -> bool
{
    return skillchainElement == ScDarkness || skillchainElement == ScDarknessII;
}

// LungeMagicBurstAvailable is G_CONDITION::LUNGE_MB_AVAILABLE: a burstable
// skillchain of tier 3 or better, in Light or Darkness, with a matching rune up.
//
// hasMatchingRune is the owner's hold on one of the light runes (Lux, Ignis,
// Flabra, Sulpor) for a Light chain, or the dark runes (Tenebrae, Tellus, Unda,
// Gelus) for a Darkness chain.
inline auto LungeMagicBurstAvailable(bool magicBurstAvailable, uint8 skillchainElement, uint8 skillchainTier, bool hasMatchingRune) -> bool
{
    if (!magicBurstAvailable || skillchainElement == 0 || skillchainTier < MinLungeSkillchainTier)
    {
        return false;
    }

    if (!IsLightSkillchain(skillchainElement) && !IsDarkSkillchain(skillchainElement))
    {
        return false;
    }

    return hasMatchingRune;
}

// IsEcosystem is G_CONDITION::IS_ECOSYSTEM.
inline auto IsEcosystem(uint8 targetEcosystem, uint8 wantedEcosystem) -> bool
{
    return targetEcosystem == wantedEcosystem;
}

// MatchesSubAnimation is G_CONDITION::SUB_ANIMATION.
inline auto MatchesSubAnimation(uint16 animationSub, uint16 wanted) -> bool
{
    return animationSub == wanted;
}

// AbilityOnCooldown is G_CONDITION::JA_ON_COOLDOWN.
inline auto AbilityOnCooldown(uint32 recastSeconds) -> bool
{
    return recastSeconds > 0;
}
} // namespace gambitshelpers
