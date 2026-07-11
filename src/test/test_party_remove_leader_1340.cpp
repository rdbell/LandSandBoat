#include "test_party_remove_leader_1340.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party remove leader 1340 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyRemoveLeader1340SelfTests() -> bool
{
    using plan = partyhelpers::remove_party_leader_plan;
    bool ok    = true;

    ok = expect(partyhelpers::ShouldAttemptPCLeaderPromote(false), "pc promote") && ok;
    ok = expect(!partyhelpers::ShouldAttemptPCLeaderPromote(true), "mob no promote") && ok;

    ok = expect(partyhelpers::ClassifyRemovePartyLeader(true, false, false, true) == plan::EMPTY_LIST, "empty") && ok;
    ok = expect(partyhelpers::ClassifyRemovePartyLeader(false, true, true, true) == plan::MOB_PROMOTE_AND_DEL, "mob promote") && ok;
    ok = expect(partyhelpers::ClassifyRemovePartyLeader(false, true, false, true) == plan::DISBAND, "mob alone disband") && ok;
    ok = expect(partyhelpers::ClassifyRemovePartyLeader(false, false, true, true) == plan::DISBAND, "pc still leader") && ok;
    ok = expect(partyhelpers::ClassifyRemovePartyLeader(false, false, true, false) == plan::REMOVE_MEMBER, "pc removed after promote") && ok;
    ok = expect(partyhelpers::ClassifyRemovePartyLeader(false, false, false, false) == plan::REMOVE_MEMBER, "pc not leader") && ok;

    ok = expect(!partyhelpers::RemovePartyLeaderReturnValue(plan::EMPTY_LIST), "ret empty") && ok;
    ok = expect(partyhelpers::RemovePartyLeaderReturnValue(plan::MOB_PROMOTE_AND_DEL), "ret mob") && ok;
    ok = expect(!partyhelpers::RemovePartyLeaderReturnValue(plan::DISBAND), "ret disband") && ok;
    ok = expect(partyhelpers::RemovePartyLeaderReturnValue(plan::REMOVE_MEMBER), "ret remove") && ok;

    ok = expect(partyhelpers::FormatRemovePartyLeaderEmptyWarning() ==
                    "CParty::RemovePartyLeader - called when \"member\" list was empty",
                "empty warning") &&
         ok;

    return ok;
}
