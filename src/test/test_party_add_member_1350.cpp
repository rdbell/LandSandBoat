#include "test_party_add_member_1350.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party add member 1350 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyAddMember1350SelfTests() -> bool
{
    using gate = partyhelpers::add_member_gate;
    bool ok    = true;

    // Ordered admission gates
    ok = expect(partyhelpers::ClassifyAddMember(true, false, false, true, true, false, false) == gate::REJECT_NULL_OR_HAS_PARTY, "null") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, true, false, true, true, false, false) == gate::REJECT_NULL_OR_HAS_PARTY, "has party") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, false, true, true, true, false, false) == gate::REJECT_ALREADY_MEMBER, "already") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, false, false, true, true, true, false) == gate::REJECT_FULL, "full") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, false, false, true, true, false, true) == gate::REJECT_TRUSTS, "trusts") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, false, false, true, true, false, false) == gate::PROCEED, "proceed pc") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, false, false, false, false, true, true) == gate::PROCEED, "proceed mob ignores full/trusts") && ok;

    // Priority: null/has-party before already-in-list
    ok = expect(partyhelpers::ClassifyAddMember(true, false, true, true, true, true, true) == gate::REJECT_NULL_OR_HAS_PARTY, "null beats full") && ok;
    ok = expect(partyhelpers::ClassifyAddMember(false, true, true, true, true, true, true) == gate::REJECT_NULL_OR_HAS_PARTY, "has-party beats already") && ok;
    // Priority: already-in-list before full
    ok = expect(partyhelpers::ClassifyAddMember(false, false, true, true, true, true, true) == gate::REJECT_ALREADY_MEMBER, "already beats full") && ok;
    // Priority: full before trusts
    ok = expect(partyhelpers::ClassifyAddMember(false, false, false, true, true, true, true) == gate::REJECT_FULL, "full beats trusts") && ok;

    // Warning strings
    ok = expect(partyhelpers::FormatAddMemberNullWarning() ==
                    "CParty::AddMember() - PEntity was null, or PParty not null.",
                "null warn") &&
         ok;
    ok = expect(partyhelpers::FormatAddMemberAlreadyInListWarning() ==
                    "CParty::AddMember() - PEntity was already in the member list!",
                "already warn") &&
         ok;
    ok = expect(partyhelpers::FormatAddMemberFullWarning() ==
                    "CParty::AddMember() - Party was full when trying to add a member.",
                "full warn") &&
         ok;
    ok = expect(partyhelpers::FormatAddMemberTrustsWarning() ==
                    "CParty::AddMember() - Party had summoned trusts when trying to add a member.",
                "trusts warn") &&
         ok;
    ok = expect(partyhelpers::FormatAddMemberNonPlayerWarning("Foo") == "Non-Player passed into function (Foo).", "non-player warn") && ok;

    // Post-append gates
    ok = expect(partyhelpers::ShouldStampLeaderCreatedPartyTime(true, 2), "stamp size 2") && ok;
    ok = expect(!partyhelpers::ShouldStampLeaderCreatedPartyTime(true, 1), "no stamp size 1") && ok;
    ok = expect(!partyhelpers::ShouldStampLeaderCreatedPartyTime(false, 2), "no stamp mob") && ok;

    ok = expect(partyhelpers::ShouldRunPCAddPostProcess(true), "pc post") && ok;
    ok = expect(!partyhelpers::ShouldRunPCAddPostProcess(false), "mob no post") && ok;

    ok = expect(partyhelpers::ShouldClearSeekingParty(true), "clear seeking") && ok;
    ok = expect(!partyhelpers::ShouldClearSeekingParty(false), "keep not seeking") && ok;

    ok = expect(partyhelpers::ShouldApplyPartyLevelSyncOnJoin(true), "sync on join") && ok;
    ok = expect(!partyhelpers::ShouldApplyPartyLevelSyncOnJoin(false), "no sync target") && ok;

    // Existing full/trust reject helpers still compose
    ok = expect(partyhelpers::ShouldRejectPCAddFull(true, true, true), "reject full") && ok;
    ok = expect(partyhelpers::ShouldRejectPCAddTrusts(true, true, true), "reject trusts") && ok;

    return ok;
}
