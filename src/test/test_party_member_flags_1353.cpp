#include "test_party_member_flags_1353.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party member flags 1353 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyMemberFlags1353SelfTests() -> bool
{
    bool ok = true;

    ok = expect(partyhelpers::ShouldRejectGetMemberFlags(true, false), "null") && ok;
    ok = expect(partyhelpers::ShouldRejectGetMemberFlags(false, true), "mismatch") && ok;
    ok = expect(!partyhelpers::ShouldRejectGetMemberFlags(false, false), "ok") && ok;
    ok = expect(partyhelpers::FormatGetMemberFlagsNullWarning() ==
                    "CParty::GetMemberFlags() - PEntity was null, or PParty mismatch.",
                "flags warn") &&
         ok;

    ok = expect(partyhelpers::IsAllianceLeaderForFlags(true, true, true), "ally leader") && ok;
    ok = expect(!partyhelpers::IsAllianceLeaderForFlags(true, true, false), "not main") && ok;
    ok = expect(!partyhelpers::IsAllianceLeaderForFlags(true, false, true), "not leader") && ok;
    ok = expect(!partyhelpers::IsAllianceLeaderForFlags(false, true, true), "no alliance") && ok;

    // MemberFlags assembly pins
    ok = expect(partyhelpers::MemberFlags(0, false, false, false, false) == 0, "empty") && ok;
    ok = expect(partyhelpers::MemberFlags(1, false, false, false, false) == partyhelpers::PartySecondFlag, "second") && ok;
    ok = expect(partyhelpers::MemberFlags(2, false, false, false, false) == partyhelpers::PartyThirdFlag, "third") && ok;
    ok = expect(partyhelpers::MemberFlags(0, true, false, false, false) == partyhelpers::PartyLeaderFlag, "leader") && ok;
    ok = expect(partyhelpers::MemberFlags(0, false, true, false, false) == partyhelpers::PartyQMFlag, "qm") && ok;
    ok = expect(partyhelpers::MemberFlags(0, false, false, true, false) == partyhelpers::PartySyncFlag, "sync") && ok;
    ok = expect(partyhelpers::MemberFlags(0, false, false, false, true) == partyhelpers::AllianceLeaderFlag, "ally flag") && ok;
    ok = expect(partyhelpers::MemberFlags(1, true, true, true, true) ==
                    static_cast<uint16>(partyhelpers::AllianceLeaderFlag | partyhelpers::PartySecondFlag |
                                        partyhelpers::PartyLeaderFlag | partyhelpers::PartyQMFlag |
                                        partyhelpers::PartySyncFlag),
                "all bits") &&
         ok;

    // Constructor
    ok = expect(partyhelpers::ShouldInitPartyFromEntity(false, false), "init ok") && ok;
    ok = expect(!partyhelpers::ShouldInitPartyFromEntity(true, false), "init null") && ok;
    ok = expect(!partyhelpers::ShouldInitPartyFromEntity(false, true), "init has party") && ok;
    ok = expect(partyhelpers::FormatCPartyCtorNullWarning() ==
                    "CParty::CParty() - PEntity was null, or party was not null.",
                "ctor warn") &&
         ok;
    ok = expect(partyhelpers::ResolvePartyTypeIsPC(true), "pc type") && ok;
    ok = expect(!partyhelpers::ResolvePartyTypeIsPC(false), "mob type") && ok;
    ok = expect(partyhelpers::PartyIDFromEntity(42) == 42, "party id") && ok;

    return ok;
}
