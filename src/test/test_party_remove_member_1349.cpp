#include "test_party_remove_member_1349.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party remove member 1349 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyRemoveMember1349SelfTests() -> bool
{
    using gate = partyhelpers::remove_member_gate;
    bool ok    = true;

    ok = expect(partyhelpers::PartyQMFlag == 0x0010, "qm flag") && ok;
    ok = expect(partyhelpers::ShouldSetQuarterMasterDBFlag(true), "set qm") && ok;
    ok = expect(!partyhelpers::ShouldSetQuarterMasterDBFlag(false), "clear qm only") && ok;

    ok = expect(partyhelpers::ClassifyRemoveMember(true, false, false) == gate::REJECT_NULL_OR_MISMATCH, "null") && ok;
    ok = expect(partyhelpers::ClassifyRemoveMember(false, true, false) == gate::REJECT_NULL_OR_MISMATCH, "mismatch") && ok;
    ok = expect(partyhelpers::ClassifyRemoveMember(false, false, true) == gate::REMOVE_AS_LEADER, "leader") && ok;
    ok = expect(partyhelpers::ClassifyRemoveMember(false, false, false) == gate::REMOVE_NON_LEADER, "non-leader") && ok;

    ok = expect(partyhelpers::FormatRemoveMemberNullWarning() ==
                    "CParty::RemoveMember() - PEntity was null, or PParty mismatch.",
                "null warn") &&
         ok;

    ok = expect(partyhelpers::ShouldClearQuarterMasterOnRemove(true), "clear qm") && ok;
    ok = expect(partyhelpers::ShouldDisableSyncOnRemove(true), "disable sync") && ok;
    ok = expect(partyhelpers::ShouldRunPCRemoveCleanup(true, true), "pc cleanup") && ok;
    ok = expect(!partyhelpers::ShouldRunPCRemoveCleanup(true, false), "mob entity") && ok;
    ok = expect(!partyhelpers::ShouldRunPCRemoveCleanup(false, true), "mob party") && ok;

    ok = expect(partyhelpers::ShouldApplyLeavingSyncCountdown(true, false, true, true), "leave countdown") && ok;
    ok = expect(!partyhelpers::ShouldApplyLeavingSyncCountdown(true, true, true, true), "leaver is target") && ok;
    ok = expect(!partyhelpers::ShouldApplyLeavingSyncCountdown(false, false, true, true), "no sync target") && ok;
    ok = expect(!partyhelpers::ShouldApplyLeavingSyncCountdown(true, false, false, true), "disappear") && ok;
    ok = expect(!partyhelpers::ShouldApplyLeavingSyncCountdown(true, false, true, false), "no infinite sync") && ok;

    return ok;
}
