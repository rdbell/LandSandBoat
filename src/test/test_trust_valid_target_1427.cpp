#include "test_trust_valid_target_1427.h"

#include "map/trust_valid_target_capacity.h"

#include <iostream>
#include <vector>

auto runTrustValidTarget1427SelfTests() -> bool
{
    using trustvalidtargethelpers::Context;
    const auto apply = [](Context ctx, bool pianissimo, bool base, std::vector<int>& calls)
    {
        return trustvalidtargethelpers::Apply(
            ctx,
            [&]() { calls.push_back(1); return pianissimo; },
            [&]() { calls.push_back(2); return base; });
    };

    std::vector<int> calls;
    bool ok = !apply(Context{ .passiveTrust = true }, true, true, calls) && calls.empty();
    calls.clear();
    ok = ok && apply(Context{ .initiatorIsTrust = true, .sameMaster = true }, false, false, calls) && calls.empty();
    calls.clear();
    ok = ok && apply(Context{ .pianissimoTarget = true, .sameAllegiance = true, .hasMaster = true }, true, false, calls) && calls == std::vector<int>{ 1 };
    calls.clear();
    ok = ok && apply(Context{ .pianissimoTarget = true, .sameAllegiance = true, .hasMaster = true }, false, true, calls) && calls == std::vector<int>{ 1, 2 };
    calls.clear();
    ok = ok && apply(Context{ .entrustTarget = true, .sameAllegiance = true, .hasMaster = true }, false, false, calls) && calls.empty();
    calls.clear();
    ok = ok && apply(Context{ .playerPartyTarget = true, .sameAllegiance = true, .initiatorIsPet = true }, false, false, calls) && calls.empty();
    calls.clear();
    ok = ok && !apply(Context{ .playerPartyTarget = true, .sameAllegiance = true, .hasMaster = true, .sameParty = false }, false, true, calls) && calls.empty();
    calls.clear();
    ok = ok && apply(Context{}, false, true, calls) && calls == std::vector<int>{ 2 };
    if (!ok)
    {
        std::cerr << "trust valid target 1427 self-test failed\n";
    }
    return ok;
}
