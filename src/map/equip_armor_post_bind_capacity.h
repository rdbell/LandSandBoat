#pragma once

#include <cstdint>

namespace equiparmorpostbindhelpers
{
constexpr uint8_t SlotHead = 4;
constexpr uint8_t SlotFeet = 8;

struct Plan
{
    bool success{};
    bool refreshRemovedArmorLook{};
};

constexpr Plan PlanFor(const bool bindSucceeded, const uint8_t equipSlotID)
{
    if (!bindSucceeded)
    {
        return {};
    }

    return {
        .success                 = true,
        .refreshRemovedArmorLook = equipSlotID >= SlotHead && equipSlotID <= SlotFeet,
    };
}
} // namespace equiparmorpostbindhelpers
