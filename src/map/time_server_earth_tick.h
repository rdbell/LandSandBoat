#pragma once

#include <array>
#include <cstddef>

// Pure JST hourly action plan used by time_server after its boundary check.
namespace timeserverearthhelpers
{

constexpr auto kMonday = 1;

enum class Action
{
    CycleWeekly,
    CycleDaily,
    GuildPattern,
    JSTMidnight,
    SanrakuMobs,
    UpdateUnityRankings,
    ZNMDecay,
    CycleTimed,
};

struct Plan
{
    std::array<Action, 8> actions{};
    std::size_t           count = 0;
};

constexpr auto MakePlan(const int jstHour, const int jstWeekday) -> Plan
{
    auto plan = Plan{};
    const auto add = [&plan](const Action action)
    {
        plan.actions[plan.count++] = action;
    };

    if (jstHour == 0)
    {
        if (jstWeekday == kMonday)
        {
            add(Action::CycleWeekly);
        }
        add(Action::CycleDaily);
        add(Action::GuildPattern);
        add(Action::JSTMidnight);
        add(Action::SanrakuMobs);
    }

    add(Action::UpdateUnityRankings);
    if (jstHour % 2 == 0)
    {
        add(Action::ZNMDecay);
        if (jstHour % 4 == 0)
        {
            add(Action::CycleTimed);
        }
    }

    return plan;
}

} // namespace timeserverearthhelpers
