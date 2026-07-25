#include "test_time_server_earth_tick_6929.h"

#include "map/time_server_earth_tick.h"

#include <array>
#include <iostream>

namespace
{

template <std::size_t N>
auto matches(const timeserverearthhelpers::Plan& plan, const std::array<timeserverearthhelpers::Action, N>& expected) -> bool
{
    if (plan.count != expected.size())
    {
        return false;
    }
    for (std::size_t index = 0; index < expected.size(); ++index)
    {
        if (plan.actions[index] != expected[index])
        {
            return false;
        }
    }
    return true;
}

} // namespace

auto runTimeServerEarthTick6929SelfTests() -> bool
{
    using timeserverearthhelpers::Action;

    const auto mondayMidnight = timeserverearthhelpers::MakePlan(0, 1);
    if (!matches(mondayMidnight, std::array{
                                      Action::CycleWeekly,
                                      Action::CycleDaily,
                                      Action::GuildPattern,
                                      Action::JSTMidnight,
                                      Action::SanrakuMobs,
                                      Action::UpdateUnityRankings,
                                      Action::ZNMDecay,
                                      Action::CycleTimed,
                                  }))
    {
        std::cerr << "time server earth tick 6929 self-test failed: Monday midnight\n";
        return false;
    }

    if (!matches(timeserverearthhelpers::MakePlan(0, 0), std::array{
                                                            Action::CycleDaily,
                                                            Action::GuildPattern,
                                                            Action::JSTMidnight,
                                                            Action::SanrakuMobs,
                                                            Action::UpdateUnityRankings,
                                                            Action::ZNMDecay,
                                                            Action::CycleTimed,
                                                        }) ||
        !matches(timeserverearthhelpers::MakePlan(2, 4), std::array{
                                                            Action::UpdateUnityRankings,
                                                            Action::ZNMDecay,
                                                        }) ||
        !matches(timeserverearthhelpers::MakePlan(3, 4), std::array{
                                                            Action::UpdateUnityRankings,
                                                        }))
    {
        std::cerr << "time server earth tick 6929 self-test failed: hourly action selection\n";
        return false;
    }

    return true;
}
