#pragma once

#include "entities/battle_entity.h"

#include <array>

namespace inventoryequiphelpers
{

constexpr auto BuildStandardSlotPlan() -> std::array<SLOTTYPE, SLOT_LINK1>
{
    auto plan = std::array<SLOTTYPE, SLOT_LINK1>{};
    for (uint8 slot = SLOT_MAIN; slot < plan.size(); ++slot)
    {
        plan[slot] = static_cast<SLOTTYPE>(slot);
    }
    return plan;
}

} // namespace inventoryequiphelpers
