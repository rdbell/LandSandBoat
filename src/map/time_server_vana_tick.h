#pragma once

#include <array>
#include <cstddef>

// Pure Vana'diel hourly action plan used by time_server after its boundary check.
namespace timeservervanatickhelpers
{

enum class Action
{
    OnGameHour,
    OnGameDay,
    TotdChange,
};

struct Plan
{
    std::array<Action, 3> actions{};
    std::size_t           count = 0;
};

constexpr auto MakePlan(const int vanaHour, const bool totdChanged) -> Plan
{
    auto plan = Plan{};
    plan.actions[plan.count++] = Action::OnGameHour;
    if (vanaHour == 0)
    {
        plan.actions[plan.count++] = Action::OnGameDay;
    }
    if (totdChanged)
    {
        plan.actions[plan.count++] = Action::TotdChange;
    }
    return plan;
}

} // namespace timeservervanatickhelpers
