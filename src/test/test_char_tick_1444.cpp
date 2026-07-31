#include "test_char_tick_1444.h"

#include "map/char_tick_capacity.h"

#include <chrono>
#include <iostream>
#include <vector>

auto runCharTick1444SelfTests() -> bool
{
    using namespace std::chrono_literals;

    bool ok = !chartickhelpers::HasDeathTimestamp(0s, 0s) &&
              chartickhelpers::HasDeathTimestamp(1s, 0s) &&
              !chartickhelpers::HasDeathTimestamp(1s, 1s) &&
              !chartickhelpers::DeathSyncDue(false, 20s, 20s) &&
              !chartickhelpers::DeathSyncDue(true, 19s, 20s) &&
              chartickhelpers::DeathSyncDue(true, 20s, 20s) &&
              chartickhelpers::DeathSyncDue(true, 21s, 20s);

    auto             nextDeathSync = 20s;
    uint8            updateMask    = 0x80;
    bool             deathActive   = false;
    std::vector<int> calls{};
    chartickhelpers::Apply(
        20s,
        nextDeathSync,
        updateMask,
        0x02,
        16s,
        [&]()
        {
            calls.push_back(1);
            deathActive = true;
        },
        [&]()
        {
            calls.push_back(2);
            return deathActive;
        },
        [&]()
        {
            calls.push_back(updateMask == 0x82 && nextDeathSync == 36s ? 3 : 30);
            return true;
        },
        [&]() { calls.push_back(4); });
    ok = updateMask == 0x82 && nextDeathSync == 36s && calls == std::vector<int>{ 1, 2, 3, 4 } && ok;

    nextDeathSync = 50s;
    updateMask    = 0x40;
    calls.clear();
    chartickhelpers::Apply(
        49s,
        nextDeathSync,
        updateMask,
        0x02,
        16s,
        [&]() { calls.push_back(4); },
        []() { return true; },
        [&]()
        {
            calls.push_back(5);
            return false;
        },
        [&]() { calls.push_back(6); });
    ok = updateMask == 0x40 && nextDeathSync == 50s && calls == std::vector<int>{ 4, 5 } && ok;

    nextDeathSync = 10s;
    updateMask    = 0x20;
    calls.clear();
    chartickhelpers::Apply(
        100s,
        nextDeathSync,
        updateMask,
        0x02,
        16s,
        [&]() { calls.push_back(7); },
        []() { return false; },
        []() { return false; },
        [&]() { calls.push_back(8); });
    ok = updateMask == 0x20 && nextDeathSync == 10s && calls == std::vector<int>{ 7 } && ok;

    if (!ok)
    {
        std::cerr << "char tick 1444 self-test failed\n";
    }
    return ok;
}
