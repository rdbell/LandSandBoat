#pragma once

namespace mobcontrollerroamactiondispatch
{
enum class Action
{
    None,
    SpecialSkill,
    Scripted,
    Roam,
};

// Resolve selects the top-level roam action after eligibility checks complete.
constexpr auto Resolve(const bool specialSkillUsed, const bool scripted, const bool canRoam) -> Action
{
    if (specialSkillUsed)
    {
        return Action::SpecialSkill;
    }
    if (scripted)
    {
        return Action::Scripted;
    }
    if (canRoam)
    {
        return Action::Roam;
    }
    return Action::None;
}
} // namespace mobcontrollerroamactiondispatch
