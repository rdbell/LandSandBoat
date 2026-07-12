#include "test_char_valid_target_1448.h"

#include "map/char_valid_target_capacity.h"

#include <iostream>
#include <vector>

auto runCharValidTarget1448SelfTests() -> bool
{
    using charvalidtargethelpers::Context;
    using charvalidtargethelpers::Relations;

    std::vector<int> calls{};
    auto apply = [&](const Context& ctx, const Relations& relations, bool base, bool pianissimo, bool entrust)
    {
        calls.clear();
        return charvalidtargethelpers::Apply(
            ctx,
            [&]()
            {
                calls.push_back(1);
                return base;
            },
            [&]()
            {
                calls.push_back(2);
                return relations;
            },
            [&]()
            {
                calls.push_back(3);
                return pianissimo;
            },
            [&]()
            {
                calls.push_back(4);
                return entrust;
            });
    };

    bool ok = !apply(Context{}, Relations{}, true, true, true) && calls.empty();
    ok = apply(Context{ true, true, true, false, false }, Relations{}, false, false, false) && calls.empty() && ok;
    ok = !apply(Context{ true, true, false, true, true }, Relations{}, true, true, true) && calls.empty() && ok;
    ok = apply(Context{ true, false, false, true, true }, Relations{}, false, false, false) && calls.empty() && ok;
    ok = apply(Context{ true, false, false, true, false }, Relations{}, true, false, false) &&
         calls == std::vector<int>{ 1 } && ok;
    ok = apply(Context{ true, false, false, false, false }, Relations{}, true, true, true) &&
         calls == std::vector<int>{ 1 } && ok;

    Relations alliance{
        .sameAlliance      = true,
        .targetsAlliance   = true,
        .pianissimoTarget  = true,
        .entrustTarget     = true,
        .differentCharacter = true,
    };
    ok = apply(Context{ true }, alliance, false, false, false) &&
         calls == std::vector<int>{ 1, 2, 3, 4 } && ok;

    Relations party{
        .sameParty         = true,
        .targetsParty      = true,
        .differentCharacter = true,
    };
    ok = apply(Context{ true }, party, false, false, false) && calls == std::vector<int>{ 1, 2 } && ok;

    Relations petMaster{
        .partyPetMaster    = true,
        .targetsParty      = true,
        .differentCharacter = true,
    };
    ok = apply(Context{ true }, petMaster, false, false, false) && ok;

    Relations soloPetMaster{
        .soloPetMaster     = true,
        .targetsParty      = true,
        .differentCharacter = true,
    };
    ok = apply(Context{ true }, soloPetMaster, false, false, false) && ok;

    Relations selfParty{
        .sameParty    = true,
        .targetsParty = true,
    };
    ok = !apply(Context{ true }, selfParty, false, false, false) && ok;

    Relations pianist{
        .sameParty          = true,
        .pianissimoTarget   = true,
        .differentCharacter = true,
    };
    ok = apply(Context{ true }, pianist, false, true, false) && calls == std::vector<int>{ 1, 2, 3 } && ok;
    ok = !apply(Context{ true }, pianist, false, false, true) && calls == std::vector<int>{ 1, 2, 3 } && ok;

    Relations entrustSelf{
        .sameParty     = true,
        .entrustTarget = true,
    };
    ok = apply(Context{ true }, entrustSelf, false, false, true) && calls == std::vector<int>{ 1, 2, 4 } && ok;

    Relations entrustTrust{
        .entrustTarget    = true,
        .initiatorIsTrust = true,
    };
    ok = apply(Context{ true }, entrustTrust, false, false, true) && ok;

    Relations entrustMiss{ .entrustTarget = true };
    ok = !apply(Context{ true }, entrustMiss, false, false, true) && ok;

    if (!ok)
    {
        std::cerr << "char valid target 1448 self-test failed\n";
    }
    return ok;
}
