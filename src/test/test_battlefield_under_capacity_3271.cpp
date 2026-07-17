#include "test_battlefield_under_capacity_3271.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldAcceptPCUnderCapacity 3271 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity TYPE_PC capacity gate for dual-wire cross-check
// (dedicated 3271):
//   playerCount < maxParticipants
auto inlineShouldAcceptPCUnderCapacity(const uint8 playerCount, const uint8 maxParticipants) -> bool
{
    return playerCount < maxParticipants;
}

// Compact dual-wire pin matching Go pinShouldAcceptPCUnderCapacity3271 /
// C++ capacity strict less-than form (formula unchanged from 1361 / 2930):
//   playerCount < maxParticipants
auto pinShouldAcceptPCUnderCapacity(const uint8 playerCount, const uint8 maxParticipants) -> bool
{
    return playerCount < maxParticipants;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldAcceptPCUnderCapacity
// (playerCount < maxParticipants; OmegaXI internal/battlefield; dedicated slice
// 3271; residual expand 2930 / pure 1361).
//
// Coverage:
//   - free == inline == pin strict less-than form
//   - poles: under / at / over capacity; zero max; unit; uint8 extremes
//   - residual 2930 / 1361 pins still hold
//   - dense compose free == inline == pin
//   - host-style inject + residual independence (3198 / 3216 / 3140 left alone)
auto runBattlefieldUnderCapacity3271SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldAdvanceBattlefieldTick;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 / 2930 pins still hold under dedicated dual-wire.
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "residual capacity pins") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(0, 1), "residual: 0 < 1 must accept") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(1, 1), "residual: 1 < 1 must reject") && ok;

    // --- Core poles: free == inline == pin strict less-than form ---
    const struct
    {
        uint8       playerCount;
        uint8       maxParticipants;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic under / at / over
        { 0, 6, true, "empty under cap 6" },
        { 2, 6, true, "partial under cap 6" },
        { 5, 6, true, "one slot left" },
        { 6, 6, false, "at cap 6 rejects" },
        { 7, 6, false, "over cap 6 rejects" },

        // Default constructor max (8)
        { 0, 8, true, "empty under default 8" },
        { 7, 8, true, "one slot left default 8" },
        { 8, 8, false, "at default 8 rejects" },

        // Zero max
        { 0, 0, false, "zero max empty rejects" },
        { 1, 0, false, "zero max non-empty rejects" },

        // Unit capacity
        { 0, 1, true, "unit capacity empty accepts" },
        { 1, 1, false, "unit capacity full rejects" },
        { 2, 1, false, "unit capacity over rejects" },

        // Alliance-sized
        { 17, 18, true, "alliance-size one left" },
        { 18, 18, false, "alliance-size full" },

        // uint8 extremes
        { 0, 255, true, "empty under max uint8" },
        { 254, 255, true, "one under max uint8" },
        { 255, 255, false, "at max uint8 rejects" },
        { 255, 0, false, "max count zero max rejects" },

        // Residual 2930 / 1361 re-pins
        { 2, 6, true, "residual 2930 under accepts" },
        { 6, 6, false, "residual 2930 at rejects" },
        { 0, 1, true, "residual 1361 unit empty accepts" },
        { 1, 1, false, "residual 1361 unit full rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool inlineF = inlineShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool pinGot  = pinShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool wantPin = c.playerCount < c.maxParticipants;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==strict less-than pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) == pinShouldAcceptPCUnderCapacity(2, 6),
                "free==pin under capacity") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(6, 6) == pinShouldAcceptPCUnderCapacity(6, 6),
                "free==pin at capacity") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(0, 0) == pinShouldAcceptPCUnderCapacity(0, 0),
                "free==pin zero max") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(255, 255) == pinShouldAcceptPCUnderCapacity(255, 255),
                "free==pin uint8 max full") &&
         ok;

    // Dense compose: free == inline == pin over a (count, max) grid.
    for (uint8 maxParticipants = 0; maxParticipants <= 8; ++maxParticipants)
    {
        for (uint8 playerCount = 0; playerCount <= 10; ++playerCount)
        {
            const bool got     = ShouldAcceptPCUnderCapacity(playerCount, maxParticipants);
            const bool inlineF = inlineShouldAcceptPCUnderCapacity(playerCount, maxParticipants);
            const bool pinGot  = pinShouldAcceptPCUnderCapacity(playerCount, maxParticipants);
            const bool want    = playerCount < maxParticipants;
            ok                 = expect(got == inlineF, "compose free==inline") && ok;
            ok                 = expect(got == pinGot, "compose free==pin") && ok;
            ok                 = expect(got == want, "compose free==strict less-than") && ok;
        }
    }

    // Host-style inject poles: CBattlefield::InsertEntity injects
    // GetPlayerCount() / GetMaxParticipants() for the TYPE_PC capacity gate.
    const struct
    {
        uint8       playerCount;
        uint8       maxParticipants;
        bool        wantAccept;
        const char* label;
    } hostCases[] = {
        { 0, 6, true, "InsertEntity empty → capacity path open" },
        { 5, 6, true, "InsertEntity one left → capacity path open" },
        { 6, 6, false, "InsertEntity at cap → full reject" },
        { 7, 6, false, "InsertEntity over cap → full reject" },
        { 0, 0, false, "InsertEntity zero max → full reject" },
        { 0, 1, true, "InsertEntity unit empty → capacity path open" },
        { 1, 1, false, "InsertEntity unit full → full reject" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool inlineF = inlineShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool pinGot  = pinShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);

        ok = expect(got == c.wantAccept, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CBattlefield::InsertEntity path semantics:
    // under capacity → enter/register may proceed; at/over → full reject.
    ok = expect(ShouldAcceptPCUnderCapacity(0, 6) && pinShouldAcceptPCUnderCapacity(0, 6),
                "insert empty → capacity path open") &&
         ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(6, 6) && !pinShouldAcceptPCUnderCapacity(6, 6),
                "insert at cap → full reject path") &&
         ok;

    // Residual independence (1361 / 2930 / 3198 / 3216 / 3140 left alone):
    // under-capacity is distinct from null-insert / already-in / advance-tick.
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity must pin strict less-than") &&
         ok;
    ok = expect(ShouldRejectNullInsert(true), "null-insert still rejects under residual (3198 left alone)") && ok;
    ok = expect(!ShouldRejectNullInsert(false), "non-null must not reject via null-insert gate") && ok;
    ok = expect(ShouldRejectAlreadyInBattlefield(true),
                "already-in still rejects under residual (3216 left alone)") &&
         ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false), "not already-in must proceed under residual") && ok;
    ok = expect(ShouldAdvanceBattlefieldTick(true) && !ShouldAdvanceBattlefieldTick(false),
                "advance-tick residual still holds under dual-wire (3140 left alone)") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6),
                "under capacity must accept even when null/already-in would reject") &&
         ok;
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
