#include "test_char_event_skip_1456.h"

#include "map/char_event_skip_capacity.h"

#include <iostream>
#include <vector>

auto runCharEventSkip1456SelfTests() -> bool
{
    std::vector<int> calls{};
    auto skipped = chareventskiphelpers::Skip(
        [&]()
        {
            calls.push_back(1);
            return false;
        },
        [&]()
        {
            calls.push_back(2);
            return false;
        },
        [&]()
        {
            calls.push_back(3);
            return true;
        },
        [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); },
        [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); return 0; },
        [&](int) { calls.push_back(8); },
        [&]() { calls.push_back(9); });
    bool ok = !skipped && calls == std::vector<int>{ 1 };

    calls.clear();
    skipped = chareventskiphelpers::Skip(
        [&]() { calls.push_back(10); return true; },
        [&]() { calls.push_back(11); return true; },
        [&]() { calls.push_back(12); return true; },
        [&]() { calls.push_back(13); },
        [&]() { calls.push_back(14); },
        [&]() { calls.push_back(15); },
        [&]() { calls.push_back(16); return 0; },
        [&](int) { calls.push_back(17); },
        [&]() { calls.push_back(18); });
    ok = !skipped && calls == std::vector<int>{ 10, 11 } && ok;

    calls.clear();
    skipped = chareventskiphelpers::Skip(
        [&]() { calls.push_back(19); return true; },
        [&]() { calls.push_back(20); return false; },
        [&]() { calls.push_back(21); return false; },
        [&]() { calls.push_back(22); },
        [&]() { calls.push_back(23); },
        [&]() { calls.push_back(24); },
        [&]() { calls.push_back(25); return 0; },
        [&](int) { calls.push_back(26); },
        [&]() { calls.push_back(27); });
    ok = !skipped && calls == std::vector<int>{ 19, 20, 21 } && ok;

    calls.clear();
    skipped = chareventskiphelpers::Skip(
        [&]() { calls.push_back(28); return true; },
        [&]() { calls.push_back(29); return false; },
        [&]() { calls.push_back(30); return true; },
        [&]() { calls.push_back(31); },
        [&]() { calls.push_back(32); },
        [&]() { calls.push_back(33); },
        [&]() { calls.push_back(34); return 0; },
        [&](int) { calls.push_back(35); },
        [&]() { calls.push_back(36); });
    ok = skipped && calls == std::vector<int>{ 28, 29, 30, 31, 32, 33, 34, 36 } && ok;

    calls.clear();
    skipped = chareventskiphelpers::Skip(
        [&]() { calls.push_back(37); return true; },
        [&]() { calls.push_back(38); return false; },
        [&]() { calls.push_back(39); return true; },
        [&]() { calls.push_back(40); },
        [&]() { calls.push_back(41); },
        [&]() { calls.push_back(42); },
        [&]() { calls.push_back(43); return 123; },
        [&](const int text) { calls.push_back(text == 123 ? 44 : 440); },
        [&]() { calls.push_back(45); });
    ok = skipped && calls == std::vector<int>{ 37, 38, 39, 40, 41, 42, 43, 44, 45 } && ok;

    if (!ok)
    {
        std::cerr << "char event skip 1456 self-test failed\n";
    }
    return ok;
}
