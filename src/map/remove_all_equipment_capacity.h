#pragma once

#include <array>
#include <cstdint>

namespace removeallequipmenthelpers
{
struct Plan
{
    std::array<uint8_t, 16> unequipSlots{};
    uint8_t                 unequipCount{};
    bool                    checkUnarmedWeapon{};
    bool                    buildWeaponSkills{};
    bool                    persistEquip{};
};

constexpr Plan PlanFor(const std::array<bool, 16>& equipped)
{
    Plan plan{
        .checkUnarmedWeapon = true,
        .buildWeaponSkills  = true,
        .persistEquip       = true,
    };
    for (uint8_t slot = 0; slot < equipped.size(); ++slot)
    {
        if (equipped[slot])
        {
            plan.unequipSlots[plan.unequipCount++] = slot;
        }
    }
    return plan;
}
} // namespace removeallequipmenthelpers
