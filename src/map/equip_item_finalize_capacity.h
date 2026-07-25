#pragma once

// Pure post-mutation finalization from charutils::EquipItem.

namespace equipitemfinalizehelpers
{

struct Facts
{
    bool clearTP{};
    bool mainNeedsUnarmed{};
};

struct Plan
{
    bool clearTP{};
    bool clearAftermath{};
    bool checkUnarmedWeapon{};
    bool buildWeaponSkills{};
    bool buildSkills{};
    bool updateHealth{};
    bool markUpdateHP{};
    bool markUpdateLook{};
};

// PlanFor mirrors EquipItem's tail after either mutation path has completed.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan{
        .buildSkills    = true,
        .updateHealth   = true,
        .markUpdateHP   = true,
        .markUpdateLook = true,
    };
    if (!facts.clearTP)
    {
        return plan;
    }

    plan.clearTP            = true;
    plan.clearAftermath     = true;
    plan.checkUnarmedWeapon = facts.mainNeedsUnarmed;
    plan.buildWeaponSkills  = true;
    return plan;
}

} // namespace equipitemfinalizehelpers
