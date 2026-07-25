#pragma once

#include "style_update_capacity.h"

#include <cstdint>

// Pure mainlook write plan from charutils::UpdateWeaponStyle.

namespace weaponstyleupdatehelpers
{

constexpr std::uint8_t SlotMain   = 0;
constexpr std::uint8_t SlotSub    = 1;
constexpr std::uint8_t SlotRanged = 2;

struct Input
{
    bool          styleLocked          = false;
    bool          styleValid           = false;
    bool          incomingItemNil      = false;
    bool          incomingIsWeapon     = false;
    bool          incomingIsHandToHand = false;
    bool          incomingIsTwoHanded  = false;
    std::uint8_t  equipSlotID          = 0;
    std::uint16_t appearanceModel      = 0;
    std::uint16_t currentMainModel     = 0;
    std::uint16_t currentSubModel      = 0;
    std::uint16_t currentRangedModel   = 0;
};

struct Plan
{
    bool          setMainLook   = false;
    bool          setSubLook    = false;
    bool          setRangedLook = false;
    std::uint16_t mainModel     = 0;
    std::uint16_t subModel      = 0;
    std::uint16_t rangedModel   = 0;
};

constexpr auto PlanFor(const Input& input) -> Plan
{
    if (styleupdatehelpers::ShouldSkipStyleUpdateWhenUnlocked(input.styleLocked))
    {
        return {};
    }

    switch (input.equipSlotID)
    {
        case SlotMain:
        {
            auto plan = Plan{
                .setMainLook = true,
                .mainModel   = input.styleValid ? input.appearanceModel : input.currentMainModel,
            };
            if (input.incomingItemNil)
            {
                plan.setSubLook = true;
                plan.subModel   = input.currentSubModel;
            }
            else if (input.incomingIsWeapon && input.incomingIsHandToHand)
            {
                plan.setSubLook = true;
                plan.subModel   = styleupdatehelpers::H2HSubLookModel(input.appearanceModel);
            }
            else if (input.incomingIsWeapon && input.incomingIsTwoHanded)
            {
                plan.setSubLook = true;
                plan.subModel   = input.currentSubModel;
            }
            return plan;
        }
        case SlotSub:
            return {
                .setSubLook = true,
                .subModel   = input.styleValid ? input.appearanceModel : input.currentSubModel,
            };
        case SlotRanged:
            return {
                .setRangedLook = true,
                .rangedModel   = input.styleValid ? input.appearanceModel : input.currentRangedModel,
            };
        default:
            return {};
    }
}

} // namespace weaponstyleupdatehelpers
