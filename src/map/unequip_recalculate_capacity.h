#pragma once

namespace unequiprecalculatehelpers
{
struct Plan
{
    bool buildSkills{};
    bool updateHealth{};
    bool markUpdateHP{};
    bool markUpdateLook{};
};

constexpr Plan PlanFor(bool recalculate)
{
    if (!recalculate)
    {
        return {};
    }
    return { .buildSkills = true, .updateHealth = true, .markUpdateHP = true, .markUpdateLook = true };
}
} // namespace unequiprecalculatehelpers
