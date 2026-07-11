#include "test_alliance_capacity_1329.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance capacity 1329 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAllianceCapacity1329SelfTests() -> bool
{
    bool ok = true;

    ok = expect(alliancehelpers::AllianceLocalFullCount == 3, "local full count") && ok;
    ok = expect(alliancehelpers::AllianceRemoteFullCount == 3, "remote full count") && ok;

    // Local full uses == 3 (not > 2) for parity with production.
    ok = expect(!alliancehelpers::IsLocalAllianceFull(0), "local 0") && ok;
    ok = expect(!alliancehelpers::IsLocalAllianceFull(2), "local 2") && ok;
    ok = expect(alliancehelpers::IsLocalAllianceFull(3), "local 3 full") && ok;
    ok = expect(!alliancehelpers::IsLocalAllianceFull(4), "local 4 not ==3") && ok;

    ok = expect(!alliancehelpers::IsRemoteAllianceFull(0), "remote 0") && ok;
    ok = expect(!alliancehelpers::IsRemoteAllianceFull(2), "remote 2") && ok;
    ok = expect(alliancehelpers::IsRemoteAllianceFull(3), "remote 3 full") && ok;
    ok = expect(!alliancehelpers::IsRemoteAllianceFull(4), "remote 4 not ==3") && ok;

    ok = expect(!alliancehelpers::IsAllianceFull(2, 2), "neither full") && ok;
    ok = expect(alliancehelpers::IsAllianceFull(3, 0), "local short-circuit") && ok;
    ok = expect(alliancehelpers::IsAllianceFull(1, 3), "remote full") && ok;
    ok = expect(!alliancehelpers::IsAllianceFull(4, 4), "both >3 not full under ==3") && ok;

    ok = expect(!alliancehelpers::HasOnlyOneLocalParty(0), "local zero") && ok;
    ok = expect(alliancehelpers::HasOnlyOneLocalParty(1), "local one") && ok;
    ok = expect(!alliancehelpers::HasOnlyOneLocalParty(2), "local two") && ok;

    ok = expect(!alliancehelpers::HasOnlyOneAllianceParty(2, 1), "local not one") && ok;
    ok = expect(!alliancehelpers::HasOnlyOneAllianceParty(1, 2), "remote not one") && ok;
    ok = expect(alliancehelpers::HasOnlyOneAllianceParty(1, 1), "both one") && ok;

    ok = expect(alliancehelpers::LoadPartyCountFromQuery(false, 99) == 0, "query fail") && ok;
    ok = expect(alliancehelpers::LoadPartyCountFromQuery(true, 0) == 0, "empty rows") && ok;
    ok = expect(alliancehelpers::LoadPartyCountFromQuery(true, 3) == 3, "rows count") && ok;

    return ok;
}
