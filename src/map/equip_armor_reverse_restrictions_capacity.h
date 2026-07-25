#pragma once

#include <cstdint>

// Pure reverse restriction predicate from charutils::EquipArmor.

namespace equiparmorreversehelpers
{

constexpr std::uint8_t SlotEndExclusive = 15;

struct Facts
{
    bool          isEquipment{};
    std::uint16_t removeSlots{};
    std::uint16_t incomingEquipSlots{};
};

// ShouldUnequip mirrors the per-slot reverse restriction condition. The host
// performs this predicate for every live slot from main through neck.
constexpr auto ShouldUnequip(const Facts& facts) -> bool
{
    return facts.isEquipment && (facts.removeSlots & facts.incomingEquipSlots) != 0;
}

} // namespace equiparmorreversehelpers
