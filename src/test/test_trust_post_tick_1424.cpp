#include "test_trust_post_tick_1424.h"

#include "map/trust_post_tick_capacity.h"

#include <iostream>
#include <vector>

auto runTrustPostTick1424SelfTests() -> bool
{
    using clock = std::chrono::steady_clock;
    const auto epoch = clock::time_point{};
    const auto apply = [&](auto current, bool zone, bool mask, bool disappeared, bool master, bool party, bool hp, std::vector<int>& calls)
    {
        trustposttickhelpers::Apply(
            [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); return current; },
            [&]() { calls.push_back(3); return zone; }, [&]() { calls.push_back(4); return mask; },
            [&]() { calls.push_back(5); return disappeared; }, [&]() { calls.push_back(6); return epoch; },
            [&](auto) { calls.push_back(7); }, [&]() { calls.push_back(8); },
            [&]() { calls.push_back(9); return master; }, [&]() { calls.push_back(10); return party; },
            [&]() { calls.push_back(11); return hp; }, [&]() { calls.push_back(12); },
            [&]() { calls.push_back(13); });
    };

    std::vector<int> calls;
    apply(epoch + std::chrono::seconds(1), true, true, false, true, true, true, calls);
    bool ok = calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
    calls.clear();
    apply(epoch + std::chrono::seconds(1), false, true, false, true, true, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3 };
    calls.clear();
    apply(epoch, true, true, false, true, true, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6 };
    calls.clear();
    apply(epoch + std::chrono::seconds(1), true, true, false, false, true, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 13 };
    calls.clear();
    apply(epoch + std::chrono::seconds(1), true, true, false, true, true, false, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13 };
    if (!ok)
    {
        std::cerr << "trust post tick 1424 self-test failed\n";
    }
    return ok;
}
