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

// CanRest mirrors !REGEN_DOWN && !NoRest status flag.
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
