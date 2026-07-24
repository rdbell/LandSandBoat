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
} // namespace gambitshelpers
