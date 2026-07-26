#include "test_party_assign_role_1347.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party assign role 1347 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyAssignRole1347SelfTests() -> bool
{
    using action = partyhelpers::assign_party_role_action;
    bool ok      = true;

    ok = expect(partyhelpers::ClassifyAssignPartyRole(false, true, true, 0) == action::REJECT_MOB_PARTY, "mob") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, false, false, 0) == action::REJECT_NOT_MEMBER, "query fail") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, false, 0) == action::REJECT_NOT_MEMBER, "no row") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 0) == action::SET_LEADER, "leader") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 4) == action::SET_QUARTERMASTER, "qm") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 5) == action::CLEAR_QUARTERMASTER, "lottery") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 6) == action::SET_LEVEL_SYNC, "sync") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 7) == action::DISABLE_LEVEL_SYNC, "unsync") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 1) == action::REJECT_UNKNOWN_ROLE, "alliance leader") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 2) == action::REJECT_UNKNOWN_ROLE, "pearl") && ok;
    ok = expect(partyhelpers::ClassifyAssignPartyRole(true, true, true, 99) == action::REJECT_UNKNOWN_ROLE, "unknown") && ok;

    ok = expect(partyhelpers::ShouldNotifyAllianceReloadOnRole(true), "accepted alliance role reload") && ok;
    ok = expect(!partyhelpers::ShouldNotifyAllianceReloadOnRole(false), "accepted party role reload") && ok;

    ok = expect(partyhelpers::ShouldNotifyAllianceReloadOnRole(true), "alliance reload") && ok;
    ok = expect(!partyhelpers::ShouldNotifyAllianceReloadOnRole(false), "party reload") && ok;

    ok = expect(partyhelpers::MsgLevelSyncSet == 238, "sync set msg") && ok;
    ok = expect(partyhelpers::FormatAssignRoleMobPartyWarning(0, "Bob") ==
                    "Attempting to assign role (0) to Bob in Mob Party.",
                "mob warn") &&
         ok;

    return ok;
}
