#include "test_char_death_homepoint_1452.h"

#include "map/char_death_homepoint_capacity.h"

#include <iostream>
#include <vector>

auto runCharDeathHomepoint1452SelfTests() -> bool
{
    std::vector<int> calls{};
    chardeathhomepointhelpers::Expire(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); });
    bool ok = calls == std::vector<int>{ 1, 2 };

    int deathTime = 10;
    chardeathhomepointhelpers::SetDeathTime(deathTime, 25);
    ok = deathTime == 25 && ok;

    calls.clear();
    auto elapsed = chardeathhomepointhelpers::TimeSinceDeath(
        -100,
        -100,
        [&]()
        {
            calls.push_back(3);
            return 50;
        },
        0);
    ok = elapsed == 0 && calls.empty() && ok;

    calls.clear();
    elapsed = chardeathhomepointhelpers::TimeSinceDeath(
        100,
        -100,
        [&]()
        {
            calls.push_back(4);
            return 160;
        },
        0);
    ok = elapsed == 60 && calls == std::vector<int>{ 4 } && ok;

    calls.clear();
    auto remaining = chardeathhomepointhelpers::TimeUntilDeathHomepoint(
        3600,
        [&]()
        {
            calls.push_back(5);
            return 60;
        });
    ok = remaining == 3540 && calls == std::vector<int>{ 5 } && ok;

    remaining = chardeathhomepointhelpers::TimeUntilDeathHomepoint(3600, []() { return 3700; });
    ok = remaining == -100 && ok;

    if (!ok)
    {
        std::cerr << "char death homepoint 1452 self-test failed\n";
    }
    return ok;
}
