#include "test_party_remove_sync_low_3430.h"

#include "map/party_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldRemoveSyncForLowLevel 3430 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::RefreshSync low-level remove for dual-wire cross-check
// (dedicated 3430 expand residual 2974; prior dedicated 3378):
//   syncLevel < 10
auto inlineShouldRemoveSyncForLowLevel3430(const uint8_t syncLevel) -> bool
{
    return syncLevel < 10;
}

// Compact dual-wire pin matching Go pinShouldRemoveSyncForLowLevel3430 / C++ capacity
// exclusive lower-bound form (formula unchanged from 1330 / 2974 / 3378):
//   syncLevel < LevelSyncMinLevel  // 10
auto pinShouldRemoveSyncForLowLevel3430(const uint8_t syncLevel) -> bool
{
    return syncLevel < partyhelpers::LevelSyncMinLevel;
}

// Prior dedicated 3378 inline/pin for cross-suite independence pins.
auto inlineShouldRemoveSyncForLowLevel3378(const uint8_t syncLevel) -> bool
{
    return syncLevel < 10;
}

auto pinShouldRemoveSyncForLowLevel3378(const uint8_t syncLevel) -> bool
{
    return syncLevel < partyhelpers::LevelSyncMinLevel;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldRemoveSyncForLowLevel
// (CParty::RefreshSync syncLevel < 10 gate;
// OmegaXI internal/party; dedicated slice 3430; residual expand 2974 /
// prior dedicated 3378 / pure 1330).
//
// Coverage:
//   - free == inline == pin (syncLevel < 10 / LevelSyncMinLevel)
//   - residual 2974 / 1330 pins still hold
//   - prior 3378 dedicated poles still hold
//   - dense edge uint8 space free == inline == pin
//   - host inject poles for RefreshSync path
//   - residual independence (1330 ResolveSyncMemberLevel / 2999 ApplySync)
auto runPartyRemoveSyncLow3430SelfTests() -> bool
{
    using partyhelpers::LevelSyncMinLevel;
    using partyhelpers::ResolveSyncMemberLevel;
    using partyhelpers::ShouldApplySyncToMember;
    using partyhelpers::ShouldRemoveSyncForLowLevel;

    bool ok = true;

    // Residual 1330 / 2974 pins still hold under dedicated dual-wire.
    ok = expect(LevelSyncMinLevel == 10, "residual: LevelSyncMinLevel == 10") && ok;
    ok = expect(ShouldRemoveSyncForLowLevel(0), "residual: remove at 0") && ok;
    ok = expect(ShouldRemoveSyncForLowLevel(9), "residual: remove at 9") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10), "residual: keep at 10") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(11), "residual 2974: keep at 11") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(255), "residual 2974: keep at uint8 max") && ok;

    // --- Core poles: free == inline == pin exclusive lower-bound form ---
    const struct
    {
        uint8_t     syncLevel;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic remove poles — below exclusive min.
        { 0, true, "remove at 0" },
        { 1, true, "remove at 1" },
        { 5, true, "remove at 5" },
        { 9, true, "remove at 9 (just below min)" },

        // Boundary keep poles — at and above min.
        { 10, false, "keep at 10 (min boundary)" },
        { 11, false, "keep at 11" },
        { 75, false, "keep at typical sync level" },
        { 255, false, "keep at uint8 max" },

        // Residual 1330 / 2974 polarity pins.
        { 0, true, "residual remove 0" },
        { 9, true, "residual remove 9" },
        { 10, false, "residual keep 10" },

        // Prior dedicated 3378 poles still hold.
        { 0, true, "prior 3378 remove 0" },
        { 9, true, "prior 3378 remove 9" },
        { 10, false, "prior 3378 keep 10" },
        { 11, false, "prior 3378 keep 11" },
        { 255, false, "prior 3378 keep 255" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRemoveSyncForLowLevel(c.syncLevel);
        const bool inlineF = inlineShouldRemoveSyncForLowLevel3430(c.syncLevel);
        const bool pin     = pinShouldRemoveSyncForLowLevel3430(c.syncLevel);
        // Positive form pin composition (explicit exclusive lower bound).
        const bool wantPin = c.syncLevel < 10;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRemoveSyncForLowLevel free == inline == pin") &&
             ok;
        ok = expect(got == (c.syncLevel < LevelSyncMinLevel), "free == syncLevel < LevelSyncMinLevel") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRemoveSyncForLowLevel(0) == pinShouldRemoveSyncForLowLevel3430(0),
                "free == pin level 0 remove") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(9) == pinShouldRemoveSyncForLowLevel3430(9),
                "free == pin level 9 remove") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(10) == pinShouldRemoveSyncForLowLevel3430(10),
                "free == pin level 10 keep") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(255) == pinShouldRemoveSyncForLowLevel3430(255),
                "free == pin level 255 keep") &&
         ok;

    // Prior dedicated 3378 independence: free still matches prior inline/pin.
    ok = expect(ShouldRemoveSyncForLowLevel(0) == inlineShouldRemoveSyncForLowLevel3378(0) &&
                    ShouldRemoveSyncForLowLevel(0) == pinShouldRemoveSyncForLowLevel3378(0),
                "prior 3378 independence level 0") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(9) == inlineShouldRemoveSyncForLowLevel3378(9) &&
                    ShouldRemoveSyncForLowLevel(9) == pinShouldRemoveSyncForLowLevel3378(9),
                "prior 3378 independence level 9") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(10) == inlineShouldRemoveSyncForLowLevel3378(10) &&
                    ShouldRemoveSyncForLowLevel(10) == pinShouldRemoveSyncForLowLevel3378(10),
                "prior 3378 independence level 10") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(255) == inlineShouldRemoveSyncForLowLevel3378(255) &&
                    ShouldRemoveSyncForLowLevel(255) == pinShouldRemoveSyncForLowLevel3378(255),
                "prior 3378 independence level 255") &&
         ok;

    // Dense compose: edge uint8 values free == inline == pin.
    // Edges: 0, 9, 10, 11, 255 (and a few mid values for coverage).
    for (const uint8_t syncLevel : { uint8_t{ 0 }, uint8_t{ 1 }, uint8_t{ 5 }, uint8_t{ 9 }, uint8_t{ 10 },
                                     uint8_t{ 11 }, uint8_t{ 50 }, uint8_t{ 99 }, uint8_t{ 255 } })
    {
        const bool got     = ShouldRemoveSyncForLowLevel(syncLevel);
        const bool inlineF = inlineShouldRemoveSyncForLowLevel3430(syncLevel);
        const bool pin     = pinShouldRemoveSyncForLowLevel3430(syncLevel);
        const bool want    = syncLevel < 10;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        ok                 = expect(got == (syncLevel < LevelSyncMinLevel), "compose free == LevelSyncMinLevel pin") && ok;
    }

    // --- Production CParty::RefreshSync path semantics ---
    // Host injects:
    //   syncLevel = sync->jobs.job[sync->GetMJob()]
    // when true  → SetSyncTarget("", MsgStd::LevelSyncRemoveLowLevel)
    // when false → keep sync; continue per-member level apply
    ok = expect(ShouldRemoveSyncForLowLevel(0) && pinShouldRemoveSyncForLowLevel3430(0),
                "RefreshSync level 0 → remove path free/pin dual-wire") &&
         ok;
    ok = expect(ShouldRemoveSyncForLowLevel(9) && pinShouldRemoveSyncForLowLevel3430(9),
                "RefreshSync level 9 → remove path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10) && !pinShouldRemoveSyncForLowLevel3430(10),
                "RefreshSync level 10 → keep path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(11), "RefreshSync level 11 → keep sync path") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(255), "RefreshSync level 255 → keep sync path") && ok;

    // Host inject poles: free == inline == pin for RefreshSync edge injects.
    const struct
    {
        uint8_t     syncLevel;
        const char* label;
    } hostPoles[] = {
        { 0, "level 0 remove" },
        { 9, "level 9 remove" },
        { 10, "level 10 keep" },
        { 11, "level 11 keep" },
        { 255, "level 255 keep" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRemoveSyncForLowLevel(p.syncLevel);
        const bool inlineF = inlineShouldRemoveSyncForLowLevel3430(p.syncLevel);
        const bool pin     = pinShouldRemoveSyncForLowLevel3430(p.syncLevel);
        const bool want    = p.syncLevel < 10;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        ok                 = expect(got == (p.syncLevel < LevelSyncMinLevel), "host inject identity") && ok;
    }

    // Residual independence (1330 / 2974 / 3378 / 2999):
    // low-level remove is distinct from per-member resolve / apply filters.
    ok = expect(ResolveSyncMemberLevel(30, 50) == 30 && ResolveSyncMemberLevel(50, 30) == 30,
                "residual ResolveSyncMemberLevel pins") &&
         ok;
    ok = expect(ShouldApplySyncToMember(true, true) && !ShouldApplySyncToMember(false, true),
                "residual ShouldApplySyncToMember pins") &&
         ok;
    // Boundary polarity independent of sibling filters.
    ok = expect(ShouldRemoveSyncForLowLevel(9), "level 9 must remove via free gate") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10), "level 10 must keep via free gate") && ok;

    return ok;
}
