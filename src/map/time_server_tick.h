#pragma once

#include <array>
#include <cstddef>

// Pure top-level time_server phase plan. The tail runs every scheduler tick,
// after any due earth and Vana'diel hourly work.
namespace timeservertickhelpers
{

enum class Action
{
    Earth,
    Vana,
    Tail,
};

struct Plan
{
    std::array<Action, 3> actions{};
    std::size_t           count = 0;
};

constexpr auto MakePlan(const bool earthDue, const bool vanaDue) -> Plan
{
    auto plan = Plan{};
    if (earthDue)
    {
        plan.actions[plan.count++] = Action::Earth;
    }
    if (vanaDue)
    {
        plan.actions[plan.count++] = Action::Vana;
    }
    plan.actions[plan.count++] = Action::Tail;
    return plan;
}

} // namespace timeservertickhelpers
