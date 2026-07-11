#include "test_alliance_assign_leader_1346.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance assign leader 1346 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAllianceAssignLeader1346SelfTests() -> bool
{
    using gate = alliancehelpers::assign_alliance_leader_gate;
    bool ok    = true;

    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, true) == gate::FOUND, "found") && ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(true, false) == gate::NOT_FOUND, "no row") && ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(false, true) == gate::NOT_FOUND, "query fail") && ok;
    ok = expect(alliancehelpers::ClassifyAssignAllianceLeader(false, false) == gate::NOT_FOUND, "both fail") && ok;

    ok = expect(alliancehelpers::AllianceLeaderFlag == 0x0008, "flag") && ok;
    ok = expect(alliancehelpers::NewAllianceIDFromLeaderChar(42) == 42, "new id") && ok;

    ok = expect(alliancehelpers::ShouldSetLocalMainParty(true), "local main") && ok;
    ok = expect(!alliancehelpers::ShouldSetLocalMainParty(false), "remote main") && ok;

    return ok;
}
