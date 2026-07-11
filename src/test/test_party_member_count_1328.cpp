#include "test_party_member_count_1328.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party member count 1328 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyMemberCount1328SelfTests() -> bool
{
    bool ok = true;

    // zone match, non-PC → 1
    ok = expect(partyhelpers::MemberCountContribution(true, false, 0) == 1, "zone match mob") && ok;
    // zone mismatch, non-PC → 0
    ok = expect(partyhelpers::MemberCountContribution(false, false, 0) == 0, "zone miss mob") && ok;
    // zone match, PC, 0 trusts → 1
    ok = expect(partyhelpers::MemberCountContribution(true, true, 0) == 1, "zone match pc") && ok;
    // zone mismatch, PC, 2 trusts → 2 (trusts always counted)
    ok = expect(partyhelpers::MemberCountContribution(false, true, 2) == 2, "trusts without zone") && ok;
    // zone match, PC, 3 trusts → 4
    ok = expect(partyhelpers::MemberCountContribution(true, true, 3) == 4, "zone+trusts") && ok;
    // non-PC ignores trustCount inject
    ok = expect(partyhelpers::MemberCountContribution(true, false, 5) == 1, "mob ignores trusts") && ok;

    // Accumulate
    ok = expect(partyhelpers::AccumulateMemberCount(0, 0) == 0, "acc zero") && ok;
    ok = expect(partyhelpers::AccumulateMemberCount(1, 2) == 3, "acc sum") && ok;
    ok = expect(partyhelpers::AccumulateMemberCount(250, 10) == 4, "acc uint8 wrap") && ok;

    // Multi-member simulation: two zone matches + one remote PC with 1 trust.
    uint8 total = 0;
    total       = partyhelpers::AccumulateMemberCount(total, partyhelpers::MemberCountContribution(true, true, 0));  // 1
    total       = partyhelpers::AccumulateMemberCount(total, partyhelpers::MemberCountContribution(true, false, 0)); // +1 = 2
    total       = partyhelpers::AccumulateMemberCount(total, partyhelpers::MemberCountContribution(false, true, 1)); // +1 = 3
    ok          = expect(total == 3, "multi member simulation") && ok;

    return ok;
}
