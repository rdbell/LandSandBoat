#pragma once

#include <cstdint>

// Pure source and target predicates from charutils::UpdateRemovedSlotsLook.

namespace equiparmorremovedlookhelpers
{

constexpr std::uint8_t SourceSlotHead = 4;
constexpr std::uint8_t SourceSlotFeet = 8;

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

} // namespace equiparmorremovedlookhelpers
