#pragma once

#include <cstdint>

// Pure visible packet-item admission from GP_CLI_COMMAND_LOCKSTYLE::process Set mode.

namespace lockstylesetitemhelpers
{

constexpr std::uint8_t SlotMain = 0;
constexpr std::uint8_t SlotFeet = 8;

struct Input
{
    std::uint8_t  packetIndex   = 0;
    std::uint8_t  equipKind     = 0;
    std::uint16_t itemID        = 0;
    bool          itemFound     = false;
    bool          isVisibleItem = false;
    bool          fitsEquipKind = false;
    bool          isHandToHand  = false;
};

struct Plan
{
    bool          writeStyleItem = false;
    std::uint8_t  styleSlot      = 0;
    std::uint16_t styleItemID    = 0;
    bool          mainHasH2H     = false;
};

constexpr auto PlanFor(const Input& input) -> Plan
{
    if (input.equipKind > SlotFeet)
    {
        return {};
    }

    return {
        .writeStyleItem = true,
        .styleSlot      = input.equipKind,
        .styleItemID    = input.itemFound && input.isVisibleItem && input.fitsEquipKind ? input.itemID : std::uint16_t{ 0 },
        .mainHasH2H     = input.packetIndex == SlotMain && input.itemFound && input.isHandToHand,
    };
}

} // namespace lockstylesetitemhelpers
