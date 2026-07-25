#pragma once

#include <cstdint>

// Pure direct main-slot appearance projection from charutils::EquipArmor.

namespace equiparmormainlookhelpers
{

struct Plan
{
    bool          setMainLook = false;
    std::uint16_t modelID     = 0;
};

// PlanFor mirrors the direct SLOT_MAIN model assignment.
constexpr auto PlanFor(const std::uint16_t modelID) -> Plan
{
    return {
        .setMainLook = true,
        .modelID     = modelID,
    };
}

} // namespace equiparmormainlookhelpers
