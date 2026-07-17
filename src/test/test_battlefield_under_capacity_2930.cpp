#include "test_battlefield_under_capacity_2930.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldAcceptPCUnderCapacity 2930 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity PC capacity formula for dual-wire cross-check (slice 2930):
//   playerCount < maxParticipants
auto inlineShouldAcceptPCUnderCapacity(const uint8 playerCount, const uint8 maxParticipants) -> bool
{
    return playerCount < maxParticipants;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldAcceptPCUnderCapacity
// (InsertEntity TYPE_PC capacity gate; slice 2930).
// Accept when playerCount < maxParticipants; equal/greater reject.
auto runBattlefieldUnderCapacity2930SelfTests() -> bool
{
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;

    bool ok = true;

    // --- Classic under / at / over capacity ---
    ok = expect(ShouldAcceptPCUnderCapacity(0, 6), "empty under cap 6") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6), "partial under cap 6") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(5, 6), "one slot left") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(6, 6), "at cap 6 rejects") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(7, 6), "over cap 6 rejects") && ok;

    // --- Default constructor max (8) ---
    ok = expect(ShouldAcceptPCUnderCapacity(0, 8), "empty under default 8") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(7, 8), "one slot left default 8") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(8, 8), "at default 8 rejects") && ok;

    // --- Zero max: never accept ---
    ok = expect(!ShouldAcceptPCUnderCapacity(0, 0), "zero max empty rejects") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(1, 0), "zero max non-empty rejects") && ok;

    // --- Unit capacity ---
    ok = expect(ShouldAcceptPCUnderCapacity(0, 1), "unit capacity empty accepts") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(1, 1), "unit capacity full rejects") && ok;

    // --- uint8 extremes ---
    ok = expect(ShouldAcceptPCUnderCapacity(0, 255), "empty under max uint8") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(254, 255), "one under max uint8") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(255, 255), "at max uint8 rejects") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(255, 0), "max count zero max rejects") && ok;

    // --- Composition table: dual-wire pure gate from host-injected scalars ---
    const struct
    {
        uint8       playerCount;
        uint8       maxParticipants;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 6, true, "table empty under 6" },
        { 2, 6, true, "table partial under 6" },
        { 5, 6, true, "table one left 6" },
        { 6, 6, false, "table at 6" },
        { 7, 6, false, "table over 6" },
        { 0, 0, false, "table zero max" },
        { 0, 1, true, "table unit empty" },
        { 1, 1, false, "table unit full" },
        { 0, 8, true, "table default empty" },
        { 8, 8, false, "table default full" },
        { 17, 18, true, "table alliance-size one left" },
        { 18, 18, false, "table alliance-size full" },
        { 254, 255, true, "table uint8 near max" },
        { 255, 255, false, "table uint8 at max" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool inlineGot = inlineShouldAcceptPCUnderCapacity(c.playerCount, c.maxParticipants);
        const bool wantPin   = c.playerCount < c.maxParticipants;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "ShouldAcceptPCUnderCapacity dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAcceptPCUnderCapacity == pin formula") && ok;
    }

    // Pin composition: strict less-than only.
    ok = expect(ShouldAcceptPCUnderCapacity(0, 1), "0 < 1 must accept") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(1, 1), "1 < 1 must reject") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(2, 1), "2 < 1 must reject") && ok;

    // Dense compose over a small (count, max) grid in the uint8 domain.
    for (uint8 maxParticipants = 0; maxParticipants <= 8; ++maxParticipants)
    {
        for (uint8 playerCount = 0; playerCount <= 10; ++playerCount)
        {
            const bool got  = ShouldAcceptPCUnderCapacity(playerCount, maxParticipants);
            const bool want = playerCount < maxParticipants;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldAcceptPCUnderCapacity(playerCount, maxParticipants),
                       "compose free == inline") &&
                 ok;
        }
    }

    // --- Production InsertEntity path semantics ---
    // under capacity → enter/register PC path may proceed
    // at/over capacity → PC insert rejected (battlefield full)
    ok = expect(ShouldAcceptPCUnderCapacity(0, 6), "InsertEntity empty → capacity path open") && ok;
    ok = expect(ShouldAcceptPCUnderCapacity(5, 6), "InsertEntity one left → capacity path open") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(6, 6), "InsertEntity at cap → full reject") && ok;
    ok = expect(!ShouldAcceptPCUnderCapacity(7, 6), "InsertEntity over cap → full reject") && ok;

    return ok;
}
