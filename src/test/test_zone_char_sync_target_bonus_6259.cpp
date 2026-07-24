#include "test_zone_char_sync_target_bonus_6259.h"

#include "map/zone_char_sync_target_bonus.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone character-sync target bonus 6259 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnPCs' production-wired combat-target score-bonus admission.
auto runZoneCharSyncTargetBonus6259SelfTests() -> bool
{
    using zonecharsynctargetbonus::ShouldAward;

    bool ok = true;
    ok      = expect(!ShouldAward(false, false, false), "missing target is excluded") && ok;
    ok      = expect(!ShouldAward(true, false, false), "non-character target is excluded") && ok;
    ok      = expect(!ShouldAward(true, true, true), "origin target is excluded") && ok;
    ok      = expect(ShouldAward(true, true, false), "other character target receives bonus") && ok;
    return ok;
}
