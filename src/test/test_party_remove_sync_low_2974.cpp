#include "test_party_remove_sync_low_2974.h"

#include "map/party_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRemoveSyncForLowLevel 2974 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::RefreshSync low-level remove for dual-wire cross-check
// (slice 2974):
//   syncLevel < 10
auto inlineShouldRemoveSyncForLowLevel(const uint8_t syncLevel) -> bool
{
    return syncLevel < 10;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRemoveSyncForLowLevel
// (CParty::RefreshSync syncLevel < 10 gate; slice 2974).
auto runPartyRemoveSyncLow2974SelfTests() -> bool
{
    using partyhelpers::LevelSyncMinLevel;
    using partyhelpers::ShouldRemoveSyncForLowLevel;

    bool ok = true;

    // Residual 1330 pins still hold under dual-wire.
    ok = expect(LevelSyncMinLevel == 10, "residual LevelSyncMinLevel == 10") && ok;
    ok = expect(ShouldRemoveSyncForLowLevel(0), "residual remove at 0") && ok;
    ok = expect(ShouldRemoveSyncForLowLevel(9), "residual remove at 9") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10), "residual keep at 10") && ok;

    const struct
    {
        uint8_t     syncLevel;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic remove poles — below exclusive min.
        { 0, true, "remove at 0" },
        { 1, true, "remove at 1" },
        { 9, true, "remove at 9 (just below min)" },

        // Boundary keep poles — at and above min.
        { 10, false, "keep at 10 (min boundary)" },
        { 11, false, "keep at 11" },
        { 75, false, "keep at typical sync level" },
        { 255, false, "keep at uint8 max" },

        // Residual 1330 polarity pins.
        { 0, true, "residual remove 0" },
        { 9, true, "residual remove 9" },
        { 10, false, "residual keep 10" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRemoveSyncForLowLevel(c.syncLevel);
        const bool inlineF = inlineShouldRemoveSyncForLowLevel(c.syncLevel);
        const bool wantPin = c.syncLevel < 10;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRemoveSyncForLowLevel dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRemoveSyncForLowLevel == pin formula syncLevel < 10") && ok;
        ok = expect(got == (c.syncLevel < LevelSyncMinLevel), "free == syncLevel < LevelSyncMinLevel") && ok;
    }

    // Pin composition: exclusive lower bound at 10.
    ok = expect(ShouldRemoveSyncForLowLevel(9), "9 must remove") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10), "10 must keep") && ok;

    // Dense compose: edge uint8 values free == inline == pin.
    // Edges: 0, 9, 10, 11, 255 (and a few mid values for coverage).
    for (const uint8_t syncLevel : { uint8_t{ 0 }, uint8_t{ 1 }, uint8_t{ 5 }, uint8_t{ 9 }, uint8_t{ 10 },
                                     uint8_t{ 11 }, uint8_t{ 50 }, uint8_t{ 99 }, uint8_t{ 255 } })
    {
        const bool got  = ShouldRemoveSyncForLowLevel(syncLevel);
        const bool want = syncLevel < 10;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRemoveSyncForLowLevel(syncLevel),
                    "compose free == inline") &&
             ok;
        ok = expect(got == (syncLevel < LevelSyncMinLevel), "compose free == LevelSyncMinLevel pin") && ok;
    }

    // --- Production CParty::RefreshSync path semantics ---
    // Host injects:
    //   syncLevel = sync->jobs.job[sync->GetMJob()]
    // when true  → SetSyncTarget("", MsgStd::LevelSyncRemoveLowLevel)
    // when false → keep sync; continue per-member level apply
    ok = expect(ShouldRemoveSyncForLowLevel(0), "RefreshSync level 0 → remove sync path") && ok;
    ok = expect(ShouldRemoveSyncForLowLevel(9), "RefreshSync level 9 → remove sync path") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10), "RefreshSync level 10 → keep sync path") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(11), "RefreshSync level 11 → keep sync path") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(255), "RefreshSync level 255 → keep sync path") && ok;

    // Explicit dual-wire: free function is syncLevel < LevelSyncMinLevel of injects.
    for (const uint8_t syncLevel : { uint8_t{ 0 }, uint8_t{ 9 }, uint8_t{ 10 }, uint8_t{ 11 }, uint8_t{ 255 } })
    {
        ok = expect(ShouldRemoveSyncForLowLevel(syncLevel) == (syncLevel < LevelSyncMinLevel),
                    "host inject identity") &&
             ok;
        ok = expect(ShouldRemoveSyncForLowLevel(syncLevel) == inlineShouldRemoveSyncForLowLevel(syncLevel),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    return ok;
}
