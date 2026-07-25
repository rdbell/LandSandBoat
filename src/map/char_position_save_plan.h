#pragma once

#include "common/cbasetypes.h"

namespace positionsavehelpers
{
inline constexpr uint8 kStatusDisappear = 2;

struct Plan
{
    bool persist;

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(uint8 status) -> Plan
{
    return { status != kStatusDisappear };
}
} // namespace positionsavehelpers
