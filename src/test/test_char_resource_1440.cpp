#include "test_char_resource_1440.h"

#include "common/cbasetypes.h"
#include "map/char_resource_capacity.h"

#include <iostream>
#include <vector>

auto runCharResource1440SelfTests() -> bool
{
    std::vector<int> calls{};
    int16             received = 0;

    const auto applied = charresourcehelpers::Apply<int16>(
        -700,
        [&](const int16 requested)
        {
            calls.push_back(1);
            received = requested;
            return -321;
        },
        [&]() { calls.push_back(2); });

    bool ok = applied == 321 && received == -700 && calls == std::vector<int>{ 1, 2 };

    calls.clear();
    unsigned baseCalls   = 0;
    unsigned latentCalls = 0;
    const auto zero      = charresourcehelpers::Apply<int32>(
        99,
        [&](const int32 requested)
        {
            ++baseCalls;
            calls.push_back(3);
            return requested == 99 ? 0 : 1;
        },
        [&]()
        {
            ++latentCalls;
            calls.push_back(4);
        });
    ok = zero == 0 && baseCalls == 1 && latentCalls == 1 && calls == std::vector<int>{ 3, 4 } && ok;

    if (!ok)
    {
        std::cerr << "char resource 1440 self-test failed\n";
    }
    return ok;
}
