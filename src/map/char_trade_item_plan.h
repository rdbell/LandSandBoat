#pragma once

#include "common/cbasetypes.h"

#include <array>

namespace tradeitemplanhelpers
{
enum class Action : uint8 { CloneToTarget, AddToTarget, ClearReserve, RemoveFromSource, ClearTradeSlot };
struct Plan
{
    std::array<Action, 4> actions{};
    uint8 count{};
    auto operator==(const Plan&) const -> bool = default;
};
constexpr auto BuildPlan(const bool present, const uint16 stackSize, const uint32 reserve) -> Plan
{
    if (!present) return {};
    return { .actions = { stackSize == 1 && reserve == 1 ? Action::CloneToTarget : Action::AddToTarget, Action::ClearReserve, Action::RemoveFromSource, Action::ClearTradeSlot }, .count = 4 };
}
} // namespace tradeitemplanhelpers
