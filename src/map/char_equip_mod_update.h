#pragma once

#include <cstdint>

namespace equipmodupdatehelpers
{
constexpr std::uint8_t EquipSlotCount = 16;

struct Plan
{
    bool updateModifiers{};
    bool updateLatentEffects{};
    bool checkLatents{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves the shared per-slot policy in ApplyAllEquipMods and
// RemoveAllEquipMods. Their hosts choose the add or remove APIs.
constexpr auto MakePlan(const bool itemPresent, const std::uint8_t itemRequiredLevel, const std::uint8_t mainLevel) -> Plan
{
    if (!itemPresent)
    {
        return {};
    }

    Plan plan{ .updateModifiers = true };
    if (itemRequiredLevel <= mainLevel)
    {
        plan.updateLatentEffects = true;
        plan.checkLatents        = true;
    }
    return plan;
}
} // namespace equipmodupdatehelpers
