#pragma once

#include <cstdint>

// Pure range/damslot policy from CCharEntity::OnWeaponSkillFinished.
// Host injects distance, hitboxes, WS range/id, and target-find.

namespace charweaponskillrangehelpers
{

// SLOTTYPE pins.
constexpr std::uint8_t SlotMain   = 0;
constexpr std::uint8_t SlotRanged = 2;

// Ranged WS id inclusive band (archery/marksmanship skillchains table band).
constexpr std::uint16_t RangedWSIDMin = 192;
constexpr std::uint16_t RangedWSIDMax = 221;

// InRange mirrors distance <= (wsRange + casterHitbox + targetHitbox).
constexpr auto InRange(const float distance,
                       const float wsRange,
                       const float casterHitbox,
                       const float targetHitbox) -> bool
{
    return distance <= (wsRange + casterHitbox + targetHitbox);
}

// DamageSlot is SLOT_RANGED for WS IDs 192..221 inclusive, else SLOT_MAIN.
constexpr auto DamageSlot(const std::uint16_t weaponSkillID) -> std::uint8_t
{
    if (weaponSkillID >= RangedWSIDMin && weaponSkillID <= RangedWSIDMax)
    {
        return SlotRanged;
    }
    return SlotMain;
}

} // namespace charweaponskillrangehelpers
