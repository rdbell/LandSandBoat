#include "test_alliance_dissolve_1339.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance dissolve 1339 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAllianceDissolve1339SelfTests() -> bool
{
    using path = alliancehelpers::dissolve_alliance_path;
    bool ok    = true;

    ok = expect(alliancehelpers::ClassifyDissolveAlliance(true) == path::PLAYER_IPC, "player") && ok;
    ok = expect(alliancehelpers::ClassifyDissolveAlliance(false) == path::SERVER_DB, "server") && ok;

    ok = expect(alliancehelpers::DissolvePartyFlagClearMask == 0x000B, "flag mask") && ok;

    ok = expect(alliancehelpers::ShouldApplyUnfilteredDissolveServerFilter(0, 0), "unfiltered zero ipp") && ok;
    ok = expect(!alliancehelpers::ShouldApplyUnfilteredDissolveServerFilter(1, 0), "filtered ip") && ok;
    ok = expect(!alliancehelpers::ShouldApplyUnfilteredDissolveServerFilter(0, 1), "filtered port") && ok;
    ok = expect(!alliancehelpers::ShouldApplyUnfilteredDissolveServerFilter(1, 1), "filtered both") && ok;

    return ok;
}
