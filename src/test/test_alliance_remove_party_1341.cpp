#include "test_alliance_remove_party_1341.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance remove party 1341 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAllianceRemoveParty1341SelfTests() -> bool
{
    using plan = alliancehelpers::remove_alliance_party_plan;
    bool ok    = true;

    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(true, false, false) == plan::NULL_PARTY, "null") && ok;
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(false, true, true) == plan::DISSOLVE_STILL_MAIN, "still main") && ok;
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(false, true, false) == plan::DEL_AND_NOTIFY, "main after promote") && ok;
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(false, false, true) == plan::DEL_AND_NOTIFY, "non-main") && ok;

    ok = expect(alliancehelpers::ShouldAttemptAllianceLeaderPromote(true), "promote main") && ok;
    ok = expect(!alliancehelpers::ShouldAttemptAllianceLeaderPromote(false), "promote non-main") && ok;

    ok = expect(alliancehelpers::FormatRemoveAlliancePartyNullWarning() == "CAlliance::removeParty - party is null!", "null warn") && ok;
    ok = expect(alliancehelpers::FormatDelPartyNullWarning() == "CAlliance::delParty - party is null!", "del null warn") && ok;

    ok = expect(alliancehelpers::ShouldSkipDelPartyWhenEmpty(false, false), "no alliance") && ok;
    ok = expect(alliancehelpers::ShouldSkipDelPartyWhenEmpty(true, true), "empty list") && ok;
    ok = expect(!alliancehelpers::ShouldSkipDelPartyWhenEmpty(true, false), "has parties") && ok;

    return ok;
}
