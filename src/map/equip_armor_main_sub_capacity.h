#pragma once

// Pure two-handed-main sub-slot plan from charutils::EquipArmor.

namespace equiparmormainsubhelpers
{

struct Facts
{
    bool incomingIsH2H{};
    bool hasSubEquipment{};
    bool subIsWeapon{};
    bool subSkillNone{};
};

struct Plan
{
    bool unequipSub{};
    bool setH2HSubLook{};
};

// PlanFor mirrors the branch for hand-to-hand and two-handed main weapons.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{ .setH2HSubLook = facts.incomingIsH2H };
    if (!facts.hasSubEquipment)
    {
        return plan;
    }
    plan.unequipSub = !facts.subIsWeapon || !facts.subSkillNone || facts.incomingIsH2H;
    return plan;
}

} // namespace equiparmormainsubhelpers
