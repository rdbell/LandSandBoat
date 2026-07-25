#pragma once

#include "common/cbasetypes.h"

namespace deathtimesavehelpers
{
struct Plan
{
    bool   persist;
    uint32 secondsSinceDeath;

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(int64 elapsedSeconds) -> Plan
{
    return { true, static_cast<uint32>(elapsedSeconds) };
}
} // namespace deathtimesavehelpers
