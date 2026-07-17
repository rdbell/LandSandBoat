#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace battleresthelpers
{

// ResolveResourcePercent mirrors GetHPP/GetMPP:
// 0 when current is 0; else max(1, floor(current/max * 100)).
// maxResource is GetMaxHP/GetMaxMP (modhp/modmp). Division by zero → treat as 0%.
inline auto ResolveResourcePercent(const int32 current, const int32 maxResource) -> uint8
{
    if (current == 0 || maxResource <= 0)
    {
        return 0;
    }
    const float ratio = (static_cast<float>(current) / static_cast<float>(maxResource)) * 100.0f;
    return static_cast<uint8>(std::max<uint8>(1, static_cast<uint8>(std::floor(ratio))));
}

// --- Slice 3253: CanRest dedicated dual-wire expand residual 3006 ---
// Index:
//   - 1635: residual pure port (GetHPP/GetMPP, CanRest, Rest policy suite)
//   - 3006: CanRest residual dual-wire suite
//   - 3253: CanRest dedicated dual-wire expand residual 3006
//
// Formula index:
//   - 1635 / 3006: CanRest residual pure dual-wire
//   - 3253: CanRest = !hasRegenDown && !hasNoRestFlag
//     dedicated dual-wire expand residual 3006
//
// Residual pure port: slice 1635 (GetHPP/GetMPP, CanRest, Rest policy suite).
// Residual dual-wire: slice 3006 (CanRest free-function dual-wire suite).
// Production host: CBattleEntity::CanRest injects
// getMod(Mod::REGEN_DOWN) != 0 and
// StatusEffectContainer->HasStatusEffectByFlag(StatusEffectFlag::NoRest)
// into CanRest (battle_entity.cpp ~404). Controllers (mob_controller /
// trust_controller) call CanRest before Rest(rate); Rest itself does not.
// Go dual-wire: aistate.CanRest (internal/aistate/can_rest.go; slice 3253).
// Sibling residual: ResolveResourcePercent / ResolveRestPlan (1635 suite).
// Coverage: test_battle_can_rest_3006 (residual dual-wire),
// test_aistate_can_rest_3253 (dedicated expand residual 3006; not in
// CMake/main).
//
// Dual-wire notes (slice 3253):
//   Formula unchanged from pure 1635 / residual dual-wire 3006:
//     !hasRegenDown && !hasNoRestFlag
//   Host inject and call sites unchanged (battle_entity.cpp ~404).
//   Dedicated suite expands free == inline == pin (direct return) + residual
//   poles + dense 2² bool space. Residual 3006 suite retained.

// CanRest mirrors !REGEN_DOWN && !NoRest status flag.
//
// Formula (slice 3253 dedicated dual-wire expand residual 3006; pure 1635 —
// formula unchanged):
//   !hasRegenDown && !hasNoRestFlag
//
// hasRegenDown  — host-injected getMod(Mod::REGEN_DOWN) != 0
// hasNoRestFlag — host-injected HasStatusEffectByFlag(NoRest)
// true  → entity is eligible to rest/heal
// false → blocked by regen-down mod and/or NoRest status flag
//
// Dual-wire of Go aistate.CanRest (can_rest.go / slice 3253).
// Call site: CBattleEntity::CanRest (~404).
// Coverage: test_aistate_can_rest_3253 (dedicated expand residual 3006;
// not in CMake/main); residual 3006 suite retained.
inline auto CanRest(const bool hasRegenDown, const bool hasNoRestFlag) -> bool
{
    return !hasRegenDown && !hasNoRestFlag;
}

// RestPlan is the pure side-effect plan for Rest(rate).
struct RestPlan
{
    bool   recoverResources{ false }; // addHP/addMP when not full on maxhp/maxmp
    uint32 recoverHP{ 0 };
    uint32 recoverMP{ 0 };
    bool   drainTP{ false };
    int16  tpDelta{ 0 }; // rate * -500
    bool   didRest{ false };
};

// ResolveRestPlan mirrors Rest(rate) using raw maxhp/maxmp (not modhp).
// rate is the rest tick fraction; currentHP/MP compared to maxhp/maxmp.
inline auto ResolveRestPlan(
    const int32 currentHP,
    const int32 maxHP,
    const int32 currentMP,
    const int32 maxMP,
    const int16 currentTP,
    const float rate) -> RestPlan
{
    RestPlan plan{};

    if (currentHP != maxHP || currentMP != maxMP)
    {
        plan.recoverResources = true;
        plan.recoverHP        = static_cast<uint32>(static_cast<float>(maxHP) * rate);
        plan.recoverMP        = static_cast<uint32>(static_cast<float>(maxMP) * rate);
        plan.didRest          = true;
    }

    if (currentTP > 0)
    {
        plan.drainTP  = true;
        plan.tpDelta  = static_cast<int16>(rate * -500.0f);
        plan.didRest  = true;
    }

    return plan;
}

} // namespace battleresthelpers
