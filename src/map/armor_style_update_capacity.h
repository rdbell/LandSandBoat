#pragma once

#include "style_update_capacity.h"

#include <cstdint>

// Pure assignment plan from charutils::UpdateArmorStyle.

namespace armorstyleupdatehelpers
{

struct Input
{
    bool          styleLocked        = false;
    bool          hasAppearance      = false;
    bool          stillHasAppearance = false;
    bool          canEquipAppearance = false;
    std::uint16_t modelID            = 0;
    std::uint8_t  equipSlotID        = 0;
};

struct Plan
{
    bool          setMainLook = false;
    std::uint8_t  slot        = 0;
    std::uint16_t modelID     = 0;
};

constexpr auto PlanFor(const Input& input) -> Plan
{
    if (styleupdatehelpers::ShouldSkipStyleUpdateWhenUnlocked(input.styleLocked) ||
        !styleupdatehelpers::ShouldApplyArmorStyle(input.canEquipAppearance) ||
        !styleupdatehelpers::IsArmorStyleSlot(input.equipSlotID))
    {
        return {};
    }

    return {
        .setMainLook = true,
        .slot        = input.equipSlotID,
        .modelID     = styleupdatehelpers::ArmorStyleAppearanceModel(input.hasAppearance, input.stillHasAppearance, input.modelID),
    };
}

} // namespace armorstyleupdatehelpers
