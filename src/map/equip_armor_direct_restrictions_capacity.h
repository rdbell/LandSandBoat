#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

// Pure direct remove-slot action plan from charutils::EquipArmor.

namespace equiparmordirecthelpers
{

constexpr std::uint8_t MaskSlotCount = 16;
constexpr std::uint8_t ArmorLookFirst = 4;
constexpr std::uint8_t ArmorLookLast  = 8;
constexpr std::size_t  MaxActions     = MaskSlotCount + (ArmorLookLast - ArmorLookFirst + 1);

enum class ActionKind : std::uint8_t
{
    Unequip,
    SetArmorLook,
};

struct Facts
{
    std::uint8_t  equipSlotID{};
    std::uint16_t itemEquipSlots{};
    std::uint16_t removeSlots{};
    std::uint16_t modelID{};
};

struct Action
{
    ActionKind    kind{};
    std::uint8_t  slot{};
    std::uint16_t modelID{};

    constexpr auto operator==(const Action&) const -> bool = default;
};

struct Plan
{
    bool                             applies{};
    std::array<Action, MaxActions>   actions{};
    std::size_t                      actionCount{};
};

// PlanFor mirrors the direct remove-slot loop, including per-slot armor-look
// replacement by the incoming item after removal.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{};
    if (facts.equipSlotID >= MaskSlotCount || (facts.itemEquipSlots & (std::uint16_t{ 1 } << facts.equipSlotID)) == 0)
    {
        return plan;
    }

    plan.applies = true;
    for (std::uint8_t slot = 0; slot < MaskSlotCount; ++slot)
    {
        if ((facts.removeSlots & (std::uint16_t{ 1 } << slot)) == 0)
        {
            continue;
        }
        plan.actions[plan.actionCount++] = { .kind = ActionKind::Unequip, .slot = slot };
        if (slot >= ArmorLookFirst && slot <= ArmorLookLast)
        {
            plan.actions[plan.actionCount++] = { .kind = ActionKind::SetArmorLook, .slot = slot, .modelID = facts.modelID };
        }
    }
    return plan;
}

} // namespace equiparmordirecthelpers
