#pragma once

#include <cstdint>

// Pure direct ranged-slot appearance projection from charutils::EquipArmor.

namespace equiparmorrangedlookhelpers
{

struct Plan
{
    bool          setRangedLook = false;
    std::uint16_t modelID       = 0;
};

// PlanFor mirrors the direct SLOT_RANGED model assignment.
constexpr auto PlanFor(const std::uint16_t modelID) -> Plan
{
    return {
        .setRangedLook = true,
        .modelID       = modelID,
    };
}

} // namespace equiparmorrangedlookhelpers
