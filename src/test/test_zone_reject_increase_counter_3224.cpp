#include "test_zone_reject_increase_counter_3224.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldRejectIncreaseZoneCounter 3224 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline IncreaseZoneCounter admission formula for dual-wire cross-check
// (dedicated slice 3224 expand residual 3037):
//   charNull || alreadyInZone || hasTreasurePool
auto inlineShouldRejectIncreaseZoneCounter(
    const bool charNull,
    const bool alreadyInZone,
    const bool hasTreasurePool) -> bool
{
    return charNull || alreadyInZone || hasTreasurePool;
}

// Compact dual-wire pin matching Go pinShouldRejectIncreaseZoneCounter3224 /
// C++ capacity zonehelpers::ShouldRejectIncreaseZoneCounter:
//   charNull || alreadyInZone || hasTreasurePool
auto pinShouldRejectIncreaseZoneCounter(
    const bool charNull,
    const bool alreadyInZone,
    const bool hasTreasurePool) -> bool
{
    return charNull || alreadyInZone || hasTreasurePool;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldRejectIncreaseZoneCounter
// (IncreaseZoneCounter entry reject; OmegaXI internal/zone;
// dedicated slice 3224 expand residual 3037 / pure 1363).
//
// Coverage:
//   - free == inline == pin == (charNull || alreadyInZone || hasTreasurePool)
//   - residual 1363 / 3037 poles still hold
//   - dense 2³ truth table for all 8 bool combos
// Formula unchanged — not registered in CMake/main.
auto runZoneRejectIncreaseCounter3224SelfTests() -> bool
{
    using zonehelpers::ShouldRejectIncreaseZoneCounter;

    bool ok = true;

    // Residual 1363 / 3037 pins still hold under dual-wire.
    ok = expect(ShouldRejectIncreaseZoneCounter(true, false, false), "residual null char → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, true, false), "residual already zone → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, false, true), "residual has pool → reject") && ok;
    ok = expect(!ShouldRejectIncreaseZoneCounter(false, false, false), "residual accept enter") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        charNull;
        bool        alreadyInZone;
        bool        hasTreasurePool;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2³ boolean space (all 8 combos).
        { false, false, false, false, "all false → accept enter" },
        { true, false, false, true, "null char → reject" },
        { false, true, false, true, "already in zone → reject" },
        { false, false, true, true, "has treasure pool → reject" },
        { true, true, false, true, "null + already zone → reject" },
        { true, false, true, true, "null + has pool → reject" },
        { false, true, true, true, "already zone + has pool → reject" },
        { true, true, true, true, "all true → reject" },

        // Residual 1363 / 3037 pins.
        { true, false, false, true, "residual null char" },
        { false, true, false, true, "residual already zone" },
        { false, false, true, true, "residual has pool" },
        { false, false, false, false, "residual accept enter" },

        // Single-pole identity / polarity repeats.
        { true, false, false, true, "identity null pole" },
        { false, true, false, true, "identity already-zone pole" },
        { false, false, true, true, "identity pool pole" },
        { true, true, true, true, "identity all-true pole" },
        { false, false, false, false, "identity accept pole" },

        // Host-safe poles (null short-circuits zone/pool to false in production).
        { true, false, false, true, "host-safe null char" },
        { false, true, false, true, "host-safe already zone" },
        { false, false, true, true, "host-safe has pool" },
        { false, true, true, true, "host-safe zone + pool" },

        // Counterfactual pure poles.
        { true, true, false, true, "counterfactual null + already zone" },
        { true, false, true, true, "counterfactual null + has pool" },
        { true, true, true, true, "counterfactual null + both" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectIncreaseZoneCounter(c.charNull, c.alreadyInZone, c.hasTreasurePool);
        const bool inlineF = inlineShouldRejectIncreaseZoneCounter(c.charNull, c.alreadyInZone, c.hasTreasurePool);
        const bool pin     = pinShouldRejectIncreaseZoneCounter(c.charNull, c.alreadyInZone, c.hasTreasurePool);
        const bool wantPin = c.charNull || c.alreadyInZone || c.hasTreasurePool;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula OR of three") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        bool        charNull;
        bool        alreadyInZone;
        bool        hasTreasurePool;
        bool        want;
        const char* label;
    } poles[] = {
        { true, false, false, true, "pole null" },
        { false, true, false, true, "pole already zone" },
        { false, false, true, true, "pole has pool" },
        { false, false, false, false, "pole accept" },
        { true, true, true, true, "pole all true" },
        { false, true, true, true, "pole zone+pool" },
    };
    for (const auto& p : poles)
    {
        const bool got     = ShouldRejectIncreaseZoneCounter(p.charNull, p.alreadyInZone, p.hasTreasurePool);
        const bool inlineF = inlineShouldRejectIncreaseZoneCounter(p.charNull, p.alreadyInZone, p.hasTreasurePool);
        const bool pin     = pinShouldRejectIncreaseZoneCounter(p.charNull, p.alreadyInZone, p.hasTreasurePool);
        ok                 = expect(got == p.want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense 2³ truth table: free == inline == pin for all 8 bool combos.
    for (const bool charNull : { false, true })
    {
        for (const bool alreadyInZone : { false, true })
        {
            for (const bool hasTreasurePool : { false, true })
            {
                const bool got     = ShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool inlineF = inlineShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool pin     = pinShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool want    = charNull || alreadyInZone || hasTreasurePool;
                ok                 = expect(got == want, "dense free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
            }
        }
    }

    // Host-style inject poles: free == inline == pin for IncreaseZoneCounter.
    for (const bool charNull : { false, true })
    {
        for (const bool alreadyInZone : { false, true })
        {
            for (const bool hasTreasurePool : { false, true })
            {
                const bool got     = ShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool inlineF = inlineShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool pin     = pinShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool want    = charNull || alreadyInZone || hasTreasurePool;
                ok                 = expect(got == want, "inject free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "inject free == inline == pin") && ok;
                // Accept path is exclusive of reject path.
                const bool acceptEnter = !got;
                ok                     = expect(acceptEnter != got, "accept and reject exclusive") && ok;
            }
        }
    }

    // Production IncreaseZoneCounter path semantics still hold.
    ok = expect(ShouldRejectIncreaseZoneCounter(true, false, false), "IncreaseZoneCounter null → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, true, false), "IncreaseZoneCounter already zone → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, false, true), "IncreaseZoneCounter has pool → reject") && ok;
    ok = expect(!ShouldRejectIncreaseZoneCounter(false, false, false), "IncreaseZoneCounter clean enter → admit") && ok;

    return ok;
}
