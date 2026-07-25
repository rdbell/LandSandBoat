#pragma once

#include <cstdint>
#include <vector>

namespace weaponskillrosterbuildhelpers
{
struct Entry
{
    uint16_t id{};
    bool     canUse{};
};

struct Facts
{
    std::vector<Entry> melee{};
    int32_t            mainAddsWeaponSkill{};
    bool               considerRanged{};
    std::vector<Entry> ranged{};
    int32_t            rangedAddsWeaponSkill{};
};

struct Plan
{
    std::vector<uint16_t> addWeaponSkillIDs{};
};

inline auto PlanFor(const Facts& facts) -> Plan
{
    Plan plan;
    for (const auto& entry : facts.melee)
    {
        if (entry.canUse || static_cast<int32_t>(entry.id) == facts.mainAddsWeaponSkill)
        {
            plan.addWeaponSkillIDs.push_back(entry.id);
        }
    }
    if (!facts.considerRanged)
    {
        return plan;
    }
    for (const auto& entry : facts.ranged)
    {
        if (entry.canUse || static_cast<int32_t>(entry.id) == facts.rangedAddsWeaponSkill)
        {
            plan.addWeaponSkillIDs.push_back(entry.id);
        }
    }
    return plan;
}
} // namespace weaponskillrosterbuildhelpers
