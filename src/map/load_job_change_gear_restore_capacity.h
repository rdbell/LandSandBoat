#pragma once

#include <array>
#include <cstdint>

namespace loadjobchangegearrestorehelpers
{
constexpr std::array<uint8_t, 9> SearchContainers{ 0, 8, 10, 11, 12, 13, 14, 15, 16 };

struct Action
{
    uint8_t  equipSlotID{};
    uint16_t itemID{};
};

struct Plan
{
    std::array<Action, 16> actions{};
    uint8_t                actionCount{};
};

constexpr Plan PlanFor(const std::array<uint16_t, 16>& ids)
{
    Plan plan;
    for (uint8_t slot = 0; slot < ids.size(); ++slot)
    {
        if (ids[slot] != 0)
        {
            plan.actions[plan.actionCount++] = { .equipSlotID = slot, .itemID = ids[slot] };
        }
    }
    return plan;
}
} // namespace loadjobchangegearrestorehelpers
