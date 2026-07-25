#pragma once

#include <array>

#include <common/cbasetypes.h>

// Pure post-frame seam for MapNetworking::parse.
namespace mapnetworkingparsepostprocesshelpers
{

constexpr std::size_t kEquipmentSlotCount = 16;

struct Plan
{
    std::array<uint8, kEquipmentSlotCount> latentEquipSlots{};
    uint8                                  latentEquipSlotCount{};
    bool                                   resetRetriggerLatents{};
    bool                                   flushEquipChanges{};
};

// MakePlan preserves parse's ordered occupied-slot latent checks, conditional
// retrigger reset, and unconditional batched equipment flush.
inline auto MakePlan(
    const bool retriggerLatents,
    const std::array<bool, kEquipmentSlotCount>& equipped) -> Plan
{
    Plan plan{
        .resetRetriggerLatents = retriggerLatents,
        .flushEquipChanges     = true,
    };
    if (!retriggerLatents)
    {
        return plan;
    }

    for (uint8 slot = 0; slot < kEquipmentSlotCount; ++slot)
    {
        if (equipped[slot])
        {
            plan.latentEquipSlots[plan.latentEquipSlotCount++] = slot;
        }
    }
    return plan;
}

} // namespace mapnetworkingparsepostprocesshelpers
