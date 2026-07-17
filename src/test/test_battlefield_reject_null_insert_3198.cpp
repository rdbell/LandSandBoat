#include "test_battlefield_reject_null_insert_3198.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldRejectNullInsert 3198 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity null-PEntity gate for dual-wire cross-check
// (dedicated 3198):
//   entityNull
auto inlineShouldRejectNullInsert(const bool entityNull) -> bool
{
    return entityNull;
}

// Compact dual-wire pin matching Go pinShouldRejectNullInsert3198 / C++ capacity
// identity form (formula unchanged from 1361 / 2994):
//   entityNull
auto pinShouldRejectNullInsert(const bool entityNull) -> bool
{
    return entityNull;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldRejectNullInsert
// (entityNull identity; OmegaXI internal/battlefield; dedicated slice 3198;
// residual expand 2994 / pure 1361).
//
// Coverage:
//   - free == inline == pin identity form
//   - poles: null rejects; non-null proceeds
//   - residual 2994 / 1361 pins still hold
//   - host-style inject + residual independence (3002 / 2930 / 3140 left alone)
auto runBattlefieldRejectNullInsert3198SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldAdvanceBattlefieldTick;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 / 2994 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectNullInsert(true), "residual: null entity rejects") && ok;
    ok = expect(!ShouldRejectNullInsert(false), "residual: non-null entity proceeds") && ok;

    // --- Core poles: free == inline == pin identity form ---
    const struct
    {
        bool        entityNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: null PEntity rejects (warning + false before other gates).
        { true, true, "null entity rejects" },

        // Pole: non-null PEntity proceeds to already-in / capacity.
        { false, false, "non-null entity proceeds" },

        // Residual 2994 / 1361 re-pins.
        { true, true, "residual 2994 null rejects" },
        { false, false, "residual 2994 non-null proceeds" },
        { true, true, "residual 1361 null rejects" },
        { false, false, "residual 1361 non-null proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullInsert(c.entityNull);
        const bool inlineF = inlineShouldRejectNullInsert(c.entityNull);
        const bool pinGot  = pinShouldRejectNullInsert(c.entityNull);
        const bool wantPin = c.entityNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRejectNullInsert(true) == pinShouldRejectNullInsert(true), "free==pin null entity") && ok;
    ok = expect(ShouldRejectNullInsert(false) == pinShouldRejectNullInsert(false), "free==pin non-null entity") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool entityNull : { false, true })
    {
        const bool got     = ShouldRejectNullInsert(entityNull);
        const bool inlineF = inlineShouldRejectNullInsert(entityNull);
        const bool pinGot  = pinShouldRejectNullInsert(entityNull);
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == entityNull, "compose free==identity") && ok;
    }

    // Host-style inject poles: CBattlefield::InsertEntity injects PEntity == nullptr.
    const struct
    {
        bool        entityNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PEntity == nullptr → warning + false before other gates" },
        { false, false, "PEntity non-null → proceed to already-in / capacity" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullInsert(c.entityNull);
        const bool inlineF = inlineShouldRejectNullInsert(c.entityNull);
        const bool pinGot  = pinShouldRejectNullInsert(c.entityNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CBattlefield::InsertEntity path semantics:
    // null → reject before other gates; non-null → proceed.
    ok = expect(ShouldRejectNullInsert(true) && pinShouldRejectNullInsert(true), "insert null → reject path") && ok;
    ok = expect(!ShouldRejectNullInsert(false) && !pinShouldRejectNullInsert(false), "insert non-null → proceed path") && ok;

    // Residual independence (1361 / 2930 / 2994 / 3002 / 3140 left alone):
    // null reject is distinct from already-in / capacity / advance-tick.
    ok = expect(ShouldRejectNullInsert(true), "null entity must reject via free gate") && ok;
    ok = expect(ShouldRejectAlreadyInBattlefield(true), "already-in still rejects under residual (3002 left alone)") && ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false), "not already-in must not reject via already-in gate") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldAdvanceBattlefieldTick(true) && !ShouldAdvanceBattlefieldTick(false),
                "advance-tick residual still holds under dual-wire (3140 left alone)") &&
         ok;
    ok = expect(!ShouldRejectNullInsert(false), "non-null must proceed even if later gates fail") && ok;
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
