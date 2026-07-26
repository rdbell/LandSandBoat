#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace synthcriticalfailhelpers
{

inline constexpr uint16 ExcludedZone = 49;
inline constexpr uint16 MaxZoneID    = 300;

struct Plan
{
    std::array<bool, 8> markBroken{};
    bool                sendInterruptedInfo{};
    bool                sendInterruptedAnswer{};
    bool                commitTransaction{};
    bool                removeTransaction{};
    bool                setAnimationNone{};
    bool                setUpdateHP{};
    bool                sendCharStatus{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(const bool hasTransaction, const std::array<uint16, 8>& ingredientItemIDs, const uint16 zoneID) -> Plan
{
    if (!hasTransaction)
    {
        return {};
    }

    Plan plan{
        .sendInterruptedInfo   = zoneID != 0 && zoneID != ExcludedZone && zoneID < MaxZoneID,
        .sendInterruptedAnswer = true,
        .commitTransaction     = true,
        .removeTransaction     = true,
        .setAnimationNone      = true,
        .setUpdateHP           = true,
        .sendCharStatus        = true,
    };
    for (size_t index = 0; index < ingredientItemIDs.size(); ++index)
    {
        plan.markBroken[index] = ingredientItemIDs[index] != 0;
    }
    return plan;
}

} // namespace synthcriticalfailhelpers
