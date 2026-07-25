#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

// Pure style-update dispatch from GP_CLI_COMMAND_LOCKSTYLE::process Set mode.

namespace lockstylesetstyleupdatehelpers
{

constexpr std::uint8_t SlotMain   = 0;
constexpr std::uint8_t SlotSub    = 1;
constexpr std::uint8_t SlotRanged = 2;
constexpr std::uint8_t SlotAmmo   = 3;
constexpr std::uint8_t SlotHead   = 4;
constexpr std::uint8_t SlotFeet   = 8;

enum class ActionKind : std::uint8_t
{
    Weapon,
    Armor,
};

struct Action
{
    ActionKind   kind = ActionKind::Weapon;
    std::uint8_t slot = 0;
};

struct Plan
{
    std::array<Action, 9> actions{};
    std::size_t           actionCount = 0;
};

constexpr auto PlanFor(const bool hasH2HInMainSlot) -> Plan
{
    auto plan = Plan{};
    const auto append = [&plan](const ActionKind kind, const std::uint8_t slot)
    {
        plan.actions[plan.actionCount++] = { kind, slot };
    };

    append(ActionKind::Weapon, SlotMain);
    if (!hasH2HInMainSlot)
    {
        append(ActionKind::Weapon, SlotSub);
    }
    append(ActionKind::Weapon, SlotRanged);
    append(ActionKind::Weapon, SlotAmmo);
    for (std::uint8_t slot = SlotHead; slot <= SlotFeet; ++slot)
    {
        append(ActionKind::Armor, slot);
    }
    return plan;
}

} // namespace lockstylesetstyleupdatehelpers
