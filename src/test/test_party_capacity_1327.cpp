#include "test_party_capacity_1327.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party capacity 1327 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyCapacity1327SelfTests() -> bool
{
    bool ok = true;

    ok = expect(partyhelpers::PCPartyLocalFullThreshold == 5, "local threshold") && ok;
    ok = expect(partyhelpers::PCPartyRemoteFullThreshold == 5, "remote threshold") && ok;

    ok = expect(!partyhelpers::IsLocalPartyFull(0), "local 0") && ok;
    ok = expect(!partyhelpers::IsLocalPartyFull(5), "local 5 not full") && ok;
    ok = expect(partyhelpers::IsLocalPartyFull(6), "local 6 full") && ok;
    ok = expect(partyhelpers::IsLocalPartyFull(7), "local 7 full") && ok;

    ok = expect(!partyhelpers::IsRemotePartyFull(0), "remote 0") && ok;
    ok = expect(!partyhelpers::IsRemotePartyFull(5), "remote 5 not full") && ok;
    ok = expect(partyhelpers::IsRemotePartyFull(6), "remote 6 full") && ok;

    ok = expect(!partyhelpers::IsPartyFull(5, 5), "neither full") && ok;
    ok = expect(partyhelpers::IsPartyFull(6, 0), "local full short-circuit") && ok;
    ok = expect(partyhelpers::IsPartyFull(3, 6), "remote full") && ok;
    ok = expect(partyhelpers::IsPartyFull(6, 6), "both full") && ok;

    ok = expect(!partyhelpers::HasOnlyOneLocalMember(0), "local zero") && ok;
    ok = expect(partyhelpers::HasOnlyOneLocalMember(1), "local one") && ok;
    ok = expect(!partyhelpers::HasOnlyOneLocalMember(2), "local two") && ok;

    ok = expect(!partyhelpers::HasOnlyOnePartyMember(2, 1), "local not one") && ok;
    ok = expect(!partyhelpers::HasOnlyOnePartyMember(1, 2), "remote not one") && ok;
    ok = expect(partyhelpers::HasOnlyOnePartyMember(1, 1), "both one") && ok;
    ok = expect(!partyhelpers::HasOnlyOnePartyMember(0, 0), "both zero") && ok;

    ok = expect(partyhelpers::LoadPartySizeForType(false, 4, 99) == 4, "mob local size") && ok;
    ok = expect(partyhelpers::LoadPartySizeForType(true, 4, 99) == 99, "pc db size") && ok;
    ok = expect(partyhelpers::LoadPartySizeForType(true, 4, 0) == 0, "pc db miss") && ok;
    const auto mobSizeQuery = partyhelpers::PlanLoadPartySizeQuery(false, 42);
    const auto pcSizeQuery  = partyhelpers::PlanLoadPartySizeQuery(true, 42);
    ok = expect(!mobSizeQuery.query && mobSizeQuery.partyID == 0, "mob skips size query") && ok;
    ok = expect(pcSizeQuery.query && pcSizeQuery.partyID == 42, "PC size query binding") && ok;

    ok = expect(partyhelpers::ShouldRejectPCAddFull(true, true, true), "reject full pc") && ok;
    ok = expect(!partyhelpers::ShouldRejectPCAddFull(true, true, false), "allow not full") && ok;
    ok = expect(!partyhelpers::ShouldRejectPCAddFull(false, true, true), "mob entity skip") && ok;
    ok = expect(!partyhelpers::ShouldRejectPCAddFull(true, false, true), "mob party skip") && ok;

    ok = expect(partyhelpers::ShouldRejectPCAddTrusts(true, true, true), "reject trusts") && ok;
    ok = expect(!partyhelpers::ShouldRejectPCAddTrusts(true, true, false), "allow no trusts") && ok;
    ok = expect(!partyhelpers::ShouldRejectPCAddTrusts(false, true, true), "trust mob entity skip") && ok;

    ok = expect(partyhelpers::PartyHasTrusts(true), "has trusts") && ok;
    ok = expect(!partyhelpers::PartyHasTrusts(false), "no trusts") && ok;

    return ok;
}
