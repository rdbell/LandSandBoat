#include "test_time_server_tick_6931.h"

#include "map/time_server_tick.h"

#include <array>
#include <iostream>

namespace
{

template <std::size_t N>
auto matches(const timeservertickhelpers::Plan& plan, const std::array<timeservertickhelpers::Action, N>& expected) -> bool
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

auto runTimeServerTick6931SelfTests() -> bool
{
    using timeservertickhelpers::Action;

    if (!matches(timeservertickhelpers::MakePlan(false, false), std::array{ Action::Tail }) ||
        !matches(timeservertickhelpers::MakePlan(true, false), std::array{
                                                                   Action::Earth,
                                                                   Action::Tail,
                                                               }) ||
        !matches(timeservertickhelpers::MakePlan(false, true), std::array{
                                                                   Action::Vana,
                                                                   Action::Tail,
                                                               }) ||
        !matches(timeservertickhelpers::MakePlan(true, true), std::array{
                                                                  Action::Earth,
                                                                  Action::Vana,
                                                                  Action::Tail,
                                                              }))
    {
        std::cerr << "time server phase plan 6931 self-test failed\\n";
        return false;
    }
    return true;
}
