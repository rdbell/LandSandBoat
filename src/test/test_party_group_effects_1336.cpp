#include "test_party_group_effects_1336.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party group effects 1336 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyGroupEffects1336SelfTests() -> bool
{
    bool ok = true;

    ok = expect(partyhelpers::ShouldPushEffectsPacket(true), "changed") && ok;
    ok = expect(!partyhelpers::ShouldPushEffectsPacket(false), "unchanged") && ok;

    ok = expect(partyhelpers::ShouldIncludeInGroupEffects(10, 10, 2, 1, true, true), "include") && ok;
    ok = expect(!partyhelpers::ShouldIncludeInGroupEffects(11, 10, 2, 1, true, true), "other party") && ok;
    ok = expect(!partyhelpers::ShouldIncludeInGroupEffects(10, 10, 1, 1, true, true), "self") && ok;
    ok = expect(!partyhelpers::ShouldIncludeInGroupEffects(10, 10, 2, 1, false, true), "not found") && ok;
    ok = expect(!partyhelpers::ShouldIncludeInGroupEffects(10, 10, 2, 1, true, false), "other zone") && ok;

    return ok;
}
