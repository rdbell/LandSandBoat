#include "test_party_disband_1345.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party disband 1345 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyDisband1345SelfTests() -> bool
{
    using path = partyhelpers::disband_party_member_path;
    bool ok    = true;

    ok = expect(partyhelpers::ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "pc full") && ok;
    ok = expect(partyhelpers::ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "mob clear") && ok;
    ok = expect(partyhelpers::ClassifyDisbandPartyMemberPath(false, false) == path::NONE, "none") && ok;
    // PC wins if both true (shouldn't happen).
    ok = expect(partyhelpers::ClassifyDisbandPartyMemberPath(true, true) == path::PC_FULL, "pc over mob") && ok;

    ok = expect(partyhelpers::ShouldDetachAllianceOnDisband(true), "detach yes") && ok;
    ok = expect(!partyhelpers::ShouldDetachAllianceOnDisband(false), "detach no") && ok;

    ok = expect(partyhelpers::ShouldNotifyPartyDisbandIPC(true), "ipc yes") && ok;
    ok = expect(!partyhelpers::ShouldNotifyPartyDisbandIPC(false), "ipc no") && ok;

    ok = expect(partyhelpers::ShouldReplaceSoloTreasurePool(true, false), "replace solo") && ok;
    ok = expect(!partyhelpers::ShouldReplaceSoloTreasurePool(true, true), "keep zone pool") && ok;
    ok = expect(!partyhelpers::ShouldReplaceSoloTreasurePool(false, false), "no pool") && ok;

    ok = expect(partyhelpers::MsgLevelSyncRemoveLeftParty == 553, "msg std") && ok;
    ok = expect(partyhelpers::LevelSyncDisableDurationSeconds == 30, "duration") && ok;

    return ok;
}
