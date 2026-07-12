#include "test_char_error_delivery_1451.h"

#include "map/char_error_delivery_capacity.h"

#include <iostream>
#include <memory>
#include <vector>

auto runCharErrorDelivery1451SelfTests() -> bool
{
    std::vector<int> calls{};
    auto             message = std::unique_ptr<int>{};
    charerrordeliveryhelpers::DeliverError(
        message,
        [&]()
        {
            calls.push_back(1);
            return false;
        },
        [&](std::unique_ptr<int>) { calls.push_back(2); });
    bool ok = !message && calls.empty();

    message = std::make_unique<int>(123);
    calls.clear();
    charerrordeliveryhelpers::DeliverError(
        message,
        [&]()
        {
            calls.push_back(3);
            return true;
        },
        [&](std::unique_ptr<int>) { calls.push_back(4); });
    ok = message && *message == 123 && calls == std::vector<int>{ 3 } && ok;

    calls.clear();
    charerrordeliveryhelpers::DeliverError(
        message,
        [&]()
        {
            calls.push_back(5);
            return false;
        },
        [&](std::unique_ptr<int> delivered)
        {
            calls.push_back(delivered && *delivered == 123 ? 6 : 60);
        });
    ok = !message && calls == std::vector<int>{ 5, 6 } && ok;

    int tickCalls = 0;
    int stored    = 0;
    calls.clear();
    auto allowed = charerrordeliveryhelpers::AttackError(
        [&]()
        {
            calls.push_back(7);
            return 100;
        },
        [&]()
        {
            calls.push_back(8);
            return 20;
        },
        [&]()
        {
            calls.push_back(9 + tickCalls);
            ++tickCalls;
            return tickCalls == 1 ? 121 : 122;
        },
        [&](const int errorTime)
        {
            calls.push_back(11);
            stored = errorTime;
        });
    ok = allowed && stored == 122 && tickCalls == 2 && calls == std::vector<int>{ 7, 8, 9, 10, 11 } && ok;

    tickCalls = 0;
    stored    = 0;
    calls.clear();
    allowed = charerrordeliveryhelpers::AttackError(
        [&]()
        {
            calls.push_back(12);
            return 100;
        },
        [&]()
        {
            calls.push_back(13);
            return 20;
        },
        [&]()
        {
            calls.push_back(14);
            ++tickCalls;
            return 120;
        },
        [&](const int errorTime)
        {
            calls.push_back(15);
            stored = errorTime;
        });
    ok = !allowed && stored == 0 && tickCalls == 1 && calls == std::vector<int>{ 12, 13, 14 } && ok;

    if (!ok)
    {
        std::cerr << "char error delivery 1451 self-test failed\n";
    }
    return ok;
}
