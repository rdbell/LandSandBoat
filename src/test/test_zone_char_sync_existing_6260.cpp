#include "test_zone_char_sync_existing_6260.h"

#include "map/zone_char_sync_existing.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone character-sync existing 6260 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnPCs' production-wired existing-entry scoring and swap-pool policy.
auto runZoneCharSyncExisting6260SelfTests() -> bool
{
    using namespace zonecharsyncexisting;

    bool ok = true;
    ok      = expect(TotalScore(100.0f, 30.0f, 20.0f, 30.0f) == 140.0f, "existing score adds bonus and threshold then subtracts distance") && ok;
    ok      = expect(ShouldTrackForSwap(9999.0f, 10000.0f), "ordinary score is tracked for swap") && ok;
    ok      = expect(!ShouldTrackForSwap(10000.0f, 10000.0f), "alliance score is not tracked for swap") && ok;
    ok      = expect(!ShouldTrackForSwap(100000.0f, 10000.0f), "party score is not tracked for swap") && ok;

    ok = expect(ShouldAddToSwapPool(31, 32, 1.0f, 0.0f), "below capacity admits every ordinary entry") && ok;
    ok = expect(ShouldAddToSwapPool(32, 32, 1.0f, 2.0f), "full pool replaces strictly lower score") && ok;
    ok = expect(!ShouldAddToSwapPool(32, 32, 1.0f, 1.0f), "full pool keeps tie at boundary") && ok;
    ok = expect(!ShouldAddToSwapPool(32, 32, 1.0f, 0.0f), "full pool rejects lower score") && ok;
    ok = expect(!ShouldAddToSwapPool(-1, 32, 1.0f, 0.0f), "negative synthetic current count wraps above cap") && ok;
    return ok;
}
