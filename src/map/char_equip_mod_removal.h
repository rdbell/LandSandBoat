#pragma once

#include <cstdint>

namespace equipmodremovalhelpers
{
constexpr std::uint8_t EquipSlotCount = 16;

struct Plan
{
    bool removeModifiers{};
    bool removeLatentEffects{};
    bool checkLatents{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves RemoveAllEquipMods' work for one equipment slot.
constexpr auto MakePlan(const bool itemPresent, const std::uint8_t itemRequiredLevel, const std::uint8_t mainLevel) -> Plan
{
    if (!itemPresent)
    {
        return {};
    }

    Plan plan{ .removeModifiers = true };
    if (itemRequiredLevel <= mainLevel)
    {
        plan.removeLatentEffects = true;
        plan.checkLatents         = true;
    }
    return plan;
}
} // namespace equipmodremovalhelpers
