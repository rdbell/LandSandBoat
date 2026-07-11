#include "test_char_pet_zoning_1434.h"

#include "map/char_pet_zoning_capacity.h"

#include <iostream>
#include <vector>

auto runCharPetZoning1434SelfTests() -> bool
{
    const auto capture = [](bool jug, bool avatar, bool automaton, bool wyvern, bool transient, bool keep, std::vector<int>& calls)
    {
        charpetzoninghelpers::ApplyCapture(
            jug, avatar, automaton, wyvern, transient,
            [&]() { calls.push_back(1); return keep; },
            [&]() { calls.push_back(2); }, [&]() { calls.push_back(3); },
            [&]() { calls.push_back(4); }, [&]() { calls.push_back(5); });
    };

    std::vector<int> calls;
    capture(true, false, false, false, false, true, calls);
    bool ok = calls == std::vector<int>{ 2, 1, 3, 4, 5 };
    calls.clear();
    capture(true, false, false, false, false, false, calls);
    ok = ok && calls == std::vector<int>{ 2, 1, 5 };
    calls.clear();
    capture(false, true, false, false, true, true, calls);
    ok = ok && calls == std::vector<int>{ 2, 5 };
    calls.clear();
    capture(false, true, false, false, false, true, calls);
    ok = ok && calls == std::vector<int>{ 2, 4, 5 };
    calls.clear();
    capture(false, false, true, false, false, true, calls);
    ok = ok && calls == std::vector<int>{ 2, 4, 5 };
    calls.clear();
    capture(false, false, false, false, false, true, calls);
    ok = ok && calls == std::vector<int>{ 2, 5 };

    calls.clear();
    charpetzoninghelpers::ApplyReset(
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); }, [&]() { calls.push_back(8); });
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };

    calls.clear();
    ok = ok && !charpetzoninghelpers::ShouldPersist(false, false, false, false, false, false, [&]() { calls.push_back(1); return true; }) && calls.empty();
    ok = ok && charpetzoninghelpers::ShouldPersist(true, false, true, false, false, false, [&]() { calls.push_back(1); return false; }) && calls.empty();
    ok = ok && charpetzoninghelpers::ShouldPersist(false, true, false, true, false, false, [&]() { calls.push_back(1); return false; }) && calls.empty();
    ok = ok && !charpetzoninghelpers::ShouldPersist(true, false, false, false, false, true, [&]() { calls.push_back(1); return false; }) && calls == std::vector<int>{ 1 };
    calls.clear();
    ok = ok && charpetzoninghelpers::ShouldPersist(false, true, false, false, false, true, [&]() { calls.push_back(1); return true; }) && calls == std::vector<int>{ 1 };
    if (!ok)
    {
        std::cerr << "char pet zoning 1434 self-test failed\n";
    }
    return ok;
}
