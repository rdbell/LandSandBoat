#pragma once

#include <cstdint>

// Pure direct sub-slot appearance projection from charutils::EquipArmor.

namespace equiparmorsublookhelpers
{

struct Plan
{
    bool          setSubLook = false;
    std::uint16_t modelID    = 0;
};

// PlanFor mirrors the direct SLOT_SUB model assignment.
constexpr auto PlanFor(const std::uint16_t modelID) -> Plan
{
    return {
        .setSubLook = true,
        .modelID    = modelID,
    };
}

} // namespace equiparmorsublookhelpers
