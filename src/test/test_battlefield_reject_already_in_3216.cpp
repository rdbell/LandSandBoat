#include "test_battlefield_reject_already_in_3216.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldRejectAlreadyInBattlefield 3216 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity already-in-battlefield gate for dual-wire cross-check
// (dedicated 3216):
//   hasBattlefield
auto inlineShouldRejectAlreadyInBattlefield(const bool hasBattlefield) -> bool
{
    return hasBattlefield;
}

// Compact dual-wire pin matching Go pinShouldRejectAlreadyInBattlefield3216 /
// C++ capacity identity form (formula unchanged from 1361 / 3002):
//   hasBattlefield
auto pinShouldRejectAlreadyInBattlefield(const bool hasBattlefield) -> bool
{
    return hasBattlefield;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldRejectAlreadyInBattlefield
// (hasBattlefield identity; OmegaXI internal/battlefield; dedicated slice 3216;
// residual expand 3002 / pure 1361).
//
// Coverage:
//   - free == inline == pin identity form
//   - poles: already-in rejects; not already-in proceeds
//   - residual 3002 / 1361 pins still hold
//   - host-style inject + residual independence (3198 / 2930 / 3140 left alone)
auto runBattlefieldRejectAlreadyIn3216SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldAdvanceBattlefieldTick;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 / 3002 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectAlreadyInBattlefield(true), "residual: already-in battlefield rejects") && ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false), "residual: not already-in proceeds") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        hasBattlefield;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: already on a battlefield rejects (false before capacity / type).
        { true, true, "already-in battlefield rejects" },

        // Pole: not on a battlefield proceeds to capacity / type.
        { false, false, "not already-in proceeds" },

        // Residual 3002 / 1361 re-pins.
        { true, true, "residual 3002 already-in rejects" },
        { false, false, "residual 3002 not already-in proceeds" },
        { true, true, "residual 1361 already-in rejects" },
        { false, false, "residual 1361 not already-in proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool inlineF = inlineShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool pinGot  = pinShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool wantPin = c.hasBattlefield;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectAlreadyInBattlefield(true) == pinShouldRejectAlreadyInBattlefield(true),
                "free==pin already-in battlefield") &&
         ok;
    ok = expect(ShouldRejectAlreadyInBattlefield(false) == pinShouldRejectAlreadyInBattlefield(false),
                "free==pin not already-in") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool hasBattlefield : { false, true })
    {
        const bool got     = ShouldRejectAlreadyInBattlefield(hasBattlefield);
        const bool inlineF = inlineShouldRejectAlreadyInBattlefield(hasBattlefield);
        const bool pinGot  = pinShouldRejectAlreadyInBattlefield(hasBattlefield);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == hasBattlefield, "compose free==identity") && ok;
    }

    // Host-style inject poles: CBattlefield::InsertEntity injects
    // PEntity->PBattlefield != nullptr after null gate, before capacity.
    const struct
    {
        bool        hasBattlefield;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PBattlefield != nullptr → return false before capacity / type" },
        { false, false, "PBattlefield == nullptr → proceed to capacity / type" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool inlineF = inlineShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool pinGot  = pinShouldRejectAlreadyInBattlefield(c.hasBattlefield);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CBattlefield::InsertEntity path semantics:
    // already-in → reject before capacity; not already-in → proceed.
    ok = expect(ShouldRejectAlreadyInBattlefield(true) && pinShouldRejectAlreadyInBattlefield(true),
                "insert already-in → reject path") &&
         ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false) && !pinShouldRejectAlreadyInBattlefield(false),
                "insert not already-in → proceed path") &&
         ok;

    // Residual independence (1361 / 2930 / 3002 / 3198 / 3140 left alone):
    // already-in reject is distinct from null-insert / capacity / advance-tick.
    ok = expect(ShouldRejectAlreadyInBattlefield(true), "already-in must reject via free gate") && ok;
    ok = expect(ShouldRejectNullInsert(true), "null-insert still rejects under residual (3198 left alone)") && ok;
    ok = expect(!ShouldRejectNullInsert(false), "non-null must not reject via null-insert gate") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldAdvanceBattlefieldTick(true) && !ShouldAdvanceBattlefieldTick(false),
                "advance-tick residual still holds under dual-wire (3140 left alone)") &&
         ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false),
                "not already-in must proceed even if later capacity fails") &&
         ok;
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
