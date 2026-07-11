#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CWeaponSkillState policy halves (SpendCost TP accounting, Update skill-
// finish / weaponskill-var extraction). Status-effect mutation, RNG draws,
// packets, and OnWeaponSkillFinished remain host-injected.

namespace weaponskillstatehelpers
{

// TP pins used by Meikyo / Sekkanoki paths.
constexpr int16 SekkanokiSpentTP   = 1000;
constexpr int16 MeikyoTPDrain      = 1000;
constexpr int16 SekkanokiTPDrain   = 1000;
constexpr int16 ConserveTPMin      = 10;  // GetRandomNumber(10, 200) inclusive-exclusive
constexpr int16 ConserveTPMaxExcl  = 200;

// ActionCategory::SkillFinish pin (for pure SkillFinish gate tests).
constexpr uint8 ActionCategorySkillFinish = 3; // verify against enums if needed

// Weaponskill damage local-var lower 24 bits mask.
constexpr uint32 WeaponskillDamageMask = 0x00FFFFFFu;

// SpendCostPath selects which TP-spend branch applies.
enum class SpendCostPath : uint8
{
    MeikyoShisui = 0,
    Sekkanoki    = 1,
    Normal       = 2,
};

// ResolveSpendCostPath mirrors Meikyo → Sekkanoki → else order.
inline auto ResolveSpendCostPath(const bool hasMeikyoShisui, const bool hasSekkanoki) -> SpendCostPath
{
    if (hasMeikyoShisui)
    {
        return SpendCostPath::MeikyoShisui;
    }
    if (hasSekkanoki)
    {
        return SpendCostPath::Sekkanoki;
    }
    return SpendCostPath::Normal;
}

// ResolveSpentTP is the m_spent value for each path.
// Meikyo: current entity TP; Sekkanoki: 1000; Normal: current entity TP.
inline auto ResolveSpentTP(const SpendCostPath path, const int16 currentTP) -> int16
{
    switch (path)
    {
        case SpendCostPath::MeikyoShisui:
            return currentTP;
        case SpendCostPath::Sekkanoki:
            return SekkanokiSpentTP;
        case SpendCostPath::Normal:
        default:
            return currentTP;
    }
}

// ResolveTPDrain is how much TP is removed (negative addTP amount as positive).
// Meikyo/Sekkanoki: 1000; Normal with deplete: full spent; Normal no-deplete: 0.
inline auto ResolveTPDrain(const SpendCostPath path, const int16 spentTP, const bool shouldDepleteNormal) -> int16
{
    switch (path)
    {
        case SpendCostPath::MeikyoShisui:
            return MeikyoTPDrain;
        case SpendCostPath::Sekkanoki:
            return SekkanokiTPDrain;
        case SpendCostPath::Normal:
        default:
            return shouldDepleteNormal ? spentTP : static_cast<int16>(0);
    }
}

// ShouldDepleteNormalTP mirrors:
//   getMod(WS_NO_DEPLETE) <= xirand::GetRandomNumber(100)
// Host injects roll in [0, 100). Higher WS_NO_DEPLETE → less often depletes.
inline auto ShouldDepleteNormalTP(const int16 wsNoDepleteMod, const int roll0to99) -> bool
{
    return wsNoDepleteMod <= roll0to99;
}

// ShouldDeleteSekkanoki mirrors Sekkanoki path side effect.
inline auto ShouldDeleteSekkanoki(const SpendCostPath path) -> bool
{
    return path == SpendCostPath::Sekkanoki;
}

// ConserveTPProcs mirrors xirand::GetRandomNumber(100) < CONSERVE_TP mod.
inline auto ConserveTPProcs(const int16 conserveTPMod, const int roll0to99) -> bool
{
    return roll0to99 < conserveTPMod;
}

// ClampConserveTPRestore pins restore into [10, 199] matching GetRandomNumber(10, 200).
inline auto ClampConserveTPRestore(const int16 restore) -> int16
{
    if (restore < ConserveTPMin)
    {
        return ConserveTPMin;
    }
    if (restore >= ConserveTPMaxExcl)
    {
        return static_cast<int16>(ConserveTPMaxExcl - 1);
    }
    return restore;
}

// ApplyConserveTPRestore returns the TP to add when Conserve TP procs.
inline auto ApplyConserveTPRestore(const int16 restoreDraw) -> int16
{
    return ClampConserveTPRestore(restoreDraw);
}

// SpendCostResult is the pure outcome of SpendCost arithmetic (no entity mutation).
struct SpendCostResult
{
    int16 spentTP{};
    int16 tpDrain{};     // amount to addTP(-tpDrain)
    int16 tpRestore{};   // amount to addTP(+tpRestore) from Conserve TP (0 if none)
    bool  deleteSekkanoki{};
};

// EvaluateSpendCost assembles the pure SpendCost result with host-injected RNG.
// meikyo/sekkanoki take priority; normal uses wsNoDepleteMod + roll for deplete.
// conserveRoll and conserveRestore are only applied when ConserveTPProcs.
inline auto EvaluateSpendCost(
    const bool hasMeikyoShisui,
    const bool hasSekkanoki,
    const int16 currentTP,
    const int16 wsNoDepleteMod,
    const int wsNoDepleteRoll0to99,
    const int16 conserveTPMod,
    const int conserveRoll0to99,
    const int16 conserveRestoreDraw) -> SpendCostResult
{
    SpendCostResult r{};
    const auto      path = ResolveSpendCostPath(hasMeikyoShisui, hasSekkanoki);
    r.spentTP            = ResolveSpentTP(path, currentTP);
    const bool deplete   = path == SpendCostPath::Normal
                               ? ShouldDepleteNormalTP(wsNoDepleteMod, wsNoDepleteRoll0to99)
                               : true;
    r.tpDrain            = ResolveTPDrain(path, r.spentTP, deplete);
    r.deleteSekkanoki    = ShouldDeleteSekkanoki(path);

    if (ConserveTPProcs(conserveTPMod, conserveRoll0to99))
    {
        r.tpRestore = ApplyConserveTPRestore(conserveRestoreDraw);
    }
    return r;
}

// IsSkillFinishCategory mirrors action.actiontype == ActionCategory::SkillFinish.
// Host injects the category ordinal; SkillFinish is typically 3 (ActionCategory).
inline auto IsSkillFinishCategory(const uint8 actionCategory, const uint8 skillFinishCategory) -> bool
{
    return actionCategory == skillFinishCategory;
}

// ExtractWeaponskillDamage mirrors weaponskillVar & 0xFFFFFF.
inline auto ExtractWeaponskillDamage(const uint32 weaponskillVar) -> uint32
{
    return weaponskillVar & WeaponskillDamageMask;
}

// ShouldExecuteWeaponskill mirrors isAlive && !IsCompleted().
inline auto ShouldExecuteWeaponskill(const bool isAlive, const bool completed) -> bool
{
    return isAlive && !completed;
}

// ShouldExitWeaponskill mirrors tick > finishTime (strict).
// Host injects whether tick is after finishTime.
inline auto ShouldExitWeaponskill(const bool tickAfterFinishTime) -> bool
{
    return tickAfterFinishTime;
}

// ShouldResetRestraint mirrors HasStatusEffect(Restraint) on skill finish path.
inline auto ShouldResetRestraint(const bool hasRestraint) -> bool
{
    return hasRestraint;
}

// ShouldCountPCHistoryWS mirrors objtype == TYPE_PC on exit.
inline auto ShouldCountPCHistoryWS(const bool isPC) -> bool
{
    return isPC;
}

// Target flags for ctor: self-target WS use TARGET_SELF else TARGET_ENEMY.
// TARGET_SELF = 0x01, TARGET_ENEMY = 0x20 typically — host supplies pins.
inline auto ResolveWeaponskillTargetFlags(const bool isSelfTargetWS, const uint16 targetSelf, const uint16 targetEnemy) -> uint16
{
    return isSelfTargetWS ? targetSelf : targetEnemy;
}

} // namespace weaponskillstatehelpers
