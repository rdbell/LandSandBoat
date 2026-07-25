#pragma once

#include <cstdint>

namespace equiparmorweaponslotstatehelpers
{
constexpr uint8_t SlotMain   = 0;
constexpr uint8_t SlotRanged = 2;
constexpr uint8_t SlotAmmo   = 3;

struct Plan
{
    bool    setWeapon{};
    uint8_t slot{};
};

constexpr Plan PlanFor(const uint8_t equipSlotID, const bool incomingIsWeapon)
{
    if (!incomingIsWeapon)
    {
        return {};
    }

    switch (equipSlotID)
    {
        case SlotMain:
        case SlotRanged:
        case SlotAmmo: return { .setWeapon = true, .slot = equipSlotID };
        default: return {};
    }
}
} // namespace equiparmorweaponslotstatehelpers
