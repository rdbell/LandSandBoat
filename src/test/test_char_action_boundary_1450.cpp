#include "test_char_action_boundary_1450.h"

#include "map/char_action_boundary_capacity.h"

#include <iostream>
#include <memory>
#include <vector>

auto runCharActionBoundary1450SelfTests() -> bool
{
    std::vector<int> calls{};
    auto             canUse = charactionboundaryhelpers::CanUseSpell(
        [&]()
        {
            calls.push_back(1);
            return false;
        },
        [&]()
        {
            calls.push_back(2);
            return true;
        });
    bool ok = !canUse && calls == std::vector<int>{ 1 };

    calls.clear();
    canUse = charactionboundaryhelpers::CanUseSpell(
        [&]()
        {
            calls.push_back(3);
            return true;
        },
        [&]()
        {
            calls.push_back(4);
            return false;
        });
    ok = !canUse && calls == std::vector<int>{ 3, 4 } && ok;

    calls.clear();
    canUse = charactionboundaryhelpers::CanUseSpell(
        [&]()
        {
            calls.push_back(5);
            return true;
        },
        [&]()
        {
            calls.push_back(6);
            return true;
        });
    ok = canUse && calls == std::vector<int>{ 5, 6 } && ok;

    calls.clear();
    auto attackResult = charactionboundaryhelpers::Attack(
        [&]()
        {
            calls.push_back(7);
            return 123;
        },
        [&](const int attackTime) { calls.push_back(attackTime == 123 ? 8 : 80); },
        [&]()
        {
            calls.push_back(9);
            return true;
        });
    ok = attackResult && calls == std::vector<int>{ 7, 8, 9 } && ok;

    calls.clear();
    attackResult = charactionboundaryhelpers::Attack(
        [&]()
        {
            calls.push_back(10);
            return 456;
        },
        [&](const int attackTime) { calls.push_back(attackTime == 456 ? 11 : 110); },
        [&]()
        {
            calls.push_back(12);
            return false;
        });
    ok = !attackResult && calls == std::vector<int>{ 10, 11, 12 } && ok;

    bool hasError = false;
    calls.clear();
    charactionboundaryhelpers::CastInterrupted(
        [&]()
        {
            calls.push_back(13);
            hasError = true;
        },
        [&]()
        {
            calls.push_back(14);
            return hasError;
        },
        [&]()
        {
            calls.push_back(15);
            return std::make_unique<int>(789);
        },
        [&](std::unique_ptr<int> message) { calls.push_back(message && *message == 789 ? 16 : 160); });
    ok = calls == std::vector<int>{ 13, 14, 15, 16 } && ok;

    calls.clear();
    charactionboundaryhelpers::CastInterrupted(
        [&]() { calls.push_back(17); },
        [&]()
        {
            calls.push_back(18);
            return false;
        },
        [&]()
        {
            calls.push_back(19);
            return std::make_unique<int>(1);
        },
        [&](std::unique_ptr<int>) { calls.push_back(20); });
    ok = calls == std::vector<int>{ 17, 18 } && ok;

    calls.clear();
    charactionboundaryhelpers::CastInterrupted(
        [&]() { calls.push_back(21); },
        [&]()
        {
            calls.push_back(22);
            return true;
        },
        [&]()
        {
            calls.push_back(23);
            return std::unique_ptr<int>{};
        },
        [&](std::unique_ptr<int>) { calls.push_back(24); });
    ok = calls == std::vector<int>{ 21, 22, 23 } && ok;

    if (!ok)
    {
        std::cerr << "char action boundary 1450 self-test failed\n";
    }
    return ok;
}
