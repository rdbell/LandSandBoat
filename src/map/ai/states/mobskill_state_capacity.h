#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstdint>

// Pure CMobSkillState policy halves (SpendCost TP accounting, Update finish /
// enmity / Avatars Favor gates). Entity mutation, packets, and skill catalog
// remain host-injected.

namespace mobskillstatehelpers
{

// TP pins.
constexpr int16 SekkanokiTPDrainRequest = 1000;
constexpr int16 MeikyoMobSkillSpentTP   = 3000;
constexpr int16 TPCap                   = 3000;

// Avatars Favor power pins for Blood Pact exit.
constexpr int16 AvatarsFavorRageGain = 3;
constexpr int16 AvatarsFavorWardGain = 2;
constexpr int16 AvatarsFavorPowerMin = 11;

// MobSkillSpendPath selects SpendCost branch.
enum class MobSkillSpendPath : uint8
{
    TpFree       = 0,
    Sekkanoki    = 1,
    MeikyoShisui = 2,
    Normal       = 3,
};

// ResolveMobSkillSpendPath mirrors:
//   isTpFree → free; else Sekkanoki; else Meikyo&&count>0; else Normal.
inline auto ResolveMobSkillSpendPath(
    const bool isTpFreeSkill,
    const bool hasSekkanoki,
    const bool hasMeikyoShisui,
    const uint32 meikyoMobSkillCount) -> MobSkillSpendPath
{
    if (isTpFreeSkill)
    {
        return MobSkillSpendPath::TpFree;
    }
    if (hasSekkanoki)
    {
        return MobSkillSpendPath::Sekkanoki;
    }
    if (hasMeikyoShisui && meikyoMobSkillCount > 0)
    {
        return MobSkillSpendPath::MeikyoShisui;
    }
    return MobSkillSpendPath::Normal;
}

// SekkanokiActualDrain mirrors addTP(-1000) return abs(change) under clamp 0..3000.
// Pure: min(1000, currentTP) when draining 1000 (no positive-TP multipliers).
inline auto SekkanokiActualDrain(const int16 currentTP) -> int16
{
    if (currentTP <= 0)
    {
        return 0;
    }
    return std::min(currentTP, SekkanokiTPDrainRequest);
}

// MobSkillSpendCostResult is the pure SpendCost outcome.
struct MobSkillSpendCostResult
{
    int16  spentTP{};
    int16  newTP{};              // health.tp after spend
    bool   deleteSekkanoki{};
    bool   updateMeikyoCount{};
    uint32 meikyoCountAfter{};   // only valid when updateMeikyoCount
    bool   zeroTPOnMeikyoEnd{};  // currentCount became 0
};

// EvaluateMobSkillSpendCost assembles pure SpendCost arithmetic.
// Meikyo does not drain TP until count hits 0 (then forces TP=0); spent is always 3000.
// Normal zeros TP and spends currentTP. Sekkanoki drains min(1000, current).
inline auto EvaluateMobSkillSpendCost(
    const bool isTpFreeSkill,
    const bool hasSekkanoki,
    const bool hasMeikyoShisui,
    const uint32 meikyoMobSkillCount,
    const int16 currentTP) -> MobSkillSpendCostResult
{
    MobSkillSpendCostResult r{};
    r.newTP = currentTP;

    const auto path = ResolveMobSkillSpendPath(isTpFreeSkill, hasSekkanoki, hasMeikyoShisui, meikyoMobSkillCount);
    switch (path)
    {
        case MobSkillSpendPath::TpFree:
            break;
        case MobSkillSpendPath::Sekkanoki:
        {
            const auto drain = SekkanokiActualDrain(currentTP);
            r.spentTP        = drain;
            r.newTP          = static_cast<int16>(currentTP - drain);
            r.deleteSekkanoki = true;
            break;
        }
        case MobSkillSpendPath::MeikyoShisui:
        {
            const auto after = meikyoMobSkillCount - 1;
            r.spentTP            = MeikyoMobSkillSpentTP;
            r.updateMeikyoCount  = true;
            r.meikyoCountAfter   = after;
            r.zeroTPOnMeikyoEnd  = after == 0;
            if (r.zeroTPOnMeikyoEnd)
            {
                r.newTP = 0;
            }
            break;
        }
        case MobSkillSpendPath::Normal:
        default:
            r.spentTP = currentTP;
            r.newTP   = 0;
            break;
    }
    return r;
}

// ShouldProcessInstantMobSkill mirrors castTime == 0s → DoUpdate immediately.
inline auto ShouldProcessInstantMobSkill(const bool castTimeIsZero) -> bool
{
    return castTimeIsZero;
}

// ShouldTurnDuringMobSkillCast mirrors castTime > 0 && tick < entry + cast.
inline auto ShouldTurnDuringMobSkillCast(const bool castTimePositive, const bool tickBeforeCastEnd) -> bool
{
    return castTimePositive && tickBeforeCastEnd;
}

// ShouldFinishMobSkill mirrors tick >= entry+cast && !completed (note: >=).
inline auto ShouldFinishMobSkill(const bool tickAtOrAfterCastEnd, const bool completed) -> bool
{
    return tickAtOrAfterCastEnd && !completed;
}

// ShouldInterruptMobSkillFinish mirrors HasPreventActionEffect || Hysteria.
inline auto ShouldInterruptMobSkillFinish(const bool hasPreventAction, const bool hasHysteria) -> bool
{
    return hasPreventAction || hasHysteria;
}

// SkillSuccessFromAction mirrors !action.targets.empty().
inline auto SkillSuccessFromAction(const bool actionTargetsEmpty) -> bool
{
    return !actionTargetsEmpty;
}

// ShouldClearFinishMessage mirrors getFlag() & SKILLFLAG_NO_FINISH_MSG.
inline auto ShouldClearFinishMessage(const bool hasNoFinishMsgFlag) -> bool
{
    return hasNoFinishMsgFlag;
}

// ShouldUpdateExitEnmity mirrors:
//   skillSuccess && PTarget && TYPE_MOB && PTarget != self && allegiance == PLAYER
inline auto ShouldUpdateExitEnmity(
    const bool skillSuccess,
    const bool hasTarget,
    const bool targetIsMob,
    const bool targetIsSelf,
    const bool casterAllegiancePlayer) -> bool
{
    return skillSuccess && hasTarget && targetIsMob && !targetIsSelf && casterAllegiancePlayer;
}

// EnmityWithMaster mirrors PET || (MOB && charmed).
inline auto EnmityWithMaster(const bool isPet, const bool isMob, const bool isCharmed) -> bool
{
    return isPet || (isMob && isCharmed);
}

// ShouldApplyAvatarsFavor mirrors:
//   TYPE_PET && master PC && (BP rage || BP ward) && has AvatarsFavor
inline auto ShouldApplyAvatarsFavor(
    const bool isPet,
    const bool masterIsPC,
    const bool isBloodPactRage,
    const bool isBloodPactWard,
    const bool hasAvatarsFavor) -> bool
{
    return isPet && masterIsPC && (isBloodPactRage || isBloodPactWard) && hasAvatarsFavor;
}

// AvatarsFavorLevelGained mirrors rage ? 3 : 2.
inline auto AvatarsFavorLevelGained(const bool isBloodPactRage) -> int16
{
    return isBloodPactRage ? AvatarsFavorRageGain : AvatarsFavorWardGain;
}

// ApplyAvatarsFavorPower mirrors power += gain; then power = max(power, 11).
// Note LSB: SetPower(power > 11 ? power : 11) after += gain.
inline auto ApplyAvatarsFavorPower(const int16 currentPower, const int16 levelGained) -> int16
{
    const auto power = static_cast<int16>(currentPower + levelGained);
    return power > AvatarsFavorPowerMin ? power : AvatarsFavorPowerMin;
}

// ShouldExitMobSkill mirrors IsCompleted() && tick > m_finishTime.
inline auto ShouldExitMobSkill(const bool completed, const bool tickAfterFinish) -> bool
{
    return completed && tickAfterFinish;
}

} // namespace mobskillstatehelpers
