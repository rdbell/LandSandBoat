#include "test_time_server_vana_tick_6930.h"

#include "map/time_server_vana_tick.h"

#include <array>
#include <iostream>

namespace
{

template <std::size_t N>
auto matches(const timeservervanatickhelpers::Plan& plan, const std::array<timeservervanatickhelpers::Action, N>& expected) -> bool
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

auto runTimeServerVanaTick6930SelfTests() -> bool
{
    using timeservervanatickhelpers::Action;

    if (!matches(timeservervanatickhelpers::MakePlan(7, false), std::array{ Action::OnGameHour }) ||
        !matches(timeservervanatickhelpers::MakePlan(0, false), std::array{
                                                               Action::OnGameHour,
                                                               Action::OnGameDay,
                                                           }) ||
        !matches(timeservervanatickhelpers::MakePlan(7, true), std::array{
                                                               Action::OnGameHour,
                                                               Action::TotdChange,
                                                           }) ||
        !matches(timeservervanatickhelpers::MakePlan(0, true), std::array{
                                                              Action::OnGameHour,
                                                              Action::OnGameDay,
                                                              Action::TotdChange,
                                                          }))
    {
        std::cerr << "time server Vana'diel tick 6930 self-test failed\n";
        return false;
    }
    return true;
}
