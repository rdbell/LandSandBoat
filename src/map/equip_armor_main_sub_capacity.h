#pragma once

#include <cstdint>

// Pure two-handed-main sub-slot plan from charutils::EquipArmor.

namespace equiparmormainsubhelpers
{

constexpr std::uint16_t H2HSubLookOffset = 0x1000;

struct Facts
{
    bool incomingIsH2H{};
    bool hasSubEquipment{};
    bool subIsWeapon{};
    bool subSkillNone{};
    std::uint16_t modelID{};
};

struct Plan
{
    bool unequipSub{};
    bool setH2HSubLook{};
    std::uint16_t h2hSubModel{};
};

// PlanFor mirrors the branch for hand-to-hand and two-handed main weapons.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{ .setH2HSubLook = facts.incomingIsH2H };
    if (plan.setH2HSubLook)
    {
        plan.h2hSubModel = static_cast<std::uint16_t>(facts.modelID + H2HSubLookOffset);
    }
    if (!facts.hasSubEquipment)
    {
        return plan;
    }
    plan.unequipSub = !facts.subIsWeapon || !facts.subSkillNone || facts.incomingIsH2H;
    return plan;
}

} // namespace equiparmormainsubhelpers
