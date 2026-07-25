#pragma once

#include <array>
#include <cstdint>

// Pure source and target predicates from charutils::UpdateRemovedSlotsLook.

namespace equiparmorremovedlookhelpers
{

constexpr std::uint8_t SourceSlotHead = 4;
constexpr std::uint8_t SourceSlotFeet = 8;
constexpr std::uint8_t SourceSlotCount = SourceSlotFeet - SourceSlotHead;

// IsSourceSlot preserves the native loop's exclusive SLOT_FEET bound.
constexpr auto IsSourceSlot(const std::uint8_t slot) -> bool
{
    return slot >= SourceSlotHead && slot < SourceSlotFeet;
}

// ShouldSetTargetLook selects the inclusive head-through-feet target range.
constexpr auto ShouldSetTargetLook(const std::uint32_t removeSlotLookID, const std::uint8_t slot) -> bool
{
    return slot >= SourceSlotHead && slot <= SourceSlotFeet && (removeSlotLookID & (1u << slot)) != 0;
}

struct Input
{
    bool          itemPresent      = false;
    bool          itemIsEquipment  = false;
    std::uint16_t modelID          = 0;
    std::uint32_t removeSlotLookID = 0;
};

struct Action
{
    std::uint8_t  sourceSlot = 0;
    std::uint8_t  targetSlot = 0;
    std::uint16_t modelID    = 0;

    auto operator==(const Action&) const -> bool = default;
};

struct Plan
{
    std::array<Action, SourceSlotCount * (SourceSlotFeet - SourceSlotHead + 1)> actions{};
    std::uint8_t                                                               actionCount = 0;
};

// PlanFor mirrors UpdateRemovedSlotsLook's source scan and ordered look writes.
constexpr auto PlanFor(const std::array<Input, SourceSlotCount>& inputs) -> Plan
{
    Plan plan;
    for (std::uint8_t sourceSlot = SourceSlotHead; IsSourceSlot(sourceSlot); ++sourceSlot)
    {
        const auto& input = inputs[sourceSlot - SourceSlotHead];
        if (!input.itemPresent || !input.itemIsEquipment || input.removeSlotLookID == 0)
        {
            continue;
        }

        for (std::uint8_t targetSlot = SourceSlotHead; targetSlot <= SourceSlotFeet; ++targetSlot)
        {
            if (ShouldSetTargetLook(input.removeSlotLookID, targetSlot))
            {
                plan.actions[plan.actionCount++] = {
                    .sourceSlot = sourceSlot,
                    .targetSlot = targetSlot,
                    .modelID    = input.modelID,
                };
            }
        }
    }
    return plan;
}

} // namespace equiparmorremovedlookhelpers
