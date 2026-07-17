#include "test_zone_reject_increase_counter_3037.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldRejectIncreaseZoneCounter 3037 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline IncreaseZoneCounter admission formula for dual-wire cross-check
// (slice 3037):
//   charNull || alreadyInZone || hasTreasurePool
auto inlineShouldRejectIncreaseZoneCounter(
    const bool charNull,
    const bool alreadyInZone,
    const bool hasTreasurePool) -> bool
{
    return charNull || alreadyInZone || hasTreasurePool;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldRejectIncreaseZoneCounter
// (charNull || alreadyInZone || hasTreasurePool at IncreaseZoneCounter entry;
// slice 3037). Dense 2³ boolean space.
auto runZoneRejectIncreaseCounter3037SelfTests() -> bool
{
    using zonehelpers::ShouldRejectIncreaseZoneCounter;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldRejectIncreaseZoneCounter(true, false, false), "residual null char → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, true, false), "residual already zone → reject") && ok;
    ok = expect(!ShouldRejectIncreaseZoneCounter(false, false, false), "residual accept enter") && ok;

    const struct
    {
        bool        charNull;
        bool        alreadyInZone;
        bool        hasTreasurePool;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2³ boolean space.
        { false, false, false, false, "all false → accept enter" },
        { true, false, false, true, "null char → reject" },
        { false, true, false, true, "already in zone → reject" },
        { false, false, true, true, "has treasure pool → reject" },
        { true, true, false, true, "null + already zone → reject" },
        { true, false, true, true, "null + has pool → reject" },
        { false, true, true, true, "already zone + has pool → reject" },
        { true, true, true, true, "all true → reject" },

        // Residual 1363 pins.
        { true, false, false, true, "residual null char" },
        { false, true, false, true, "residual already zone" },
        { false, false, false, false, "residual accept enter" },

        // Identity / polarity repeats for dual-wire stability.
        { false, false, true, true, "identity pool pole" },
        { true, true, true, true, "identity all-true pole" },
        { false, false, false, false, "identity accept pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectIncreaseZoneCounter(c.charNull, c.alreadyInZone, c.hasTreasurePool);
        const bool inlineF = inlineShouldRejectIncreaseZoneCounter(c.charNull, c.alreadyInZone, c.hasTreasurePool);
        const bool wantPin = c.charNull || c.alreadyInZone || c.hasTreasurePool;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectIncreaseZoneCounter dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectIncreaseZoneCounter == pin formula OR of three") && ok;
    }

    // Pin composition: only (false, false, false) accepts.
    ok = expect(!ShouldRejectIncreaseZoneCounter(false, false, false), "all false must accept") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(true, false, false), "charNull must reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, true, false), "alreadyInZone must reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, false, true), "hasTreasurePool must reject") && ok;

    // Explicit polarity: reject iff any pole true.
    for (const bool charNull : { false, true })
    {
        for (const bool alreadyInZone : { false, true })
        {
            for (const bool hasTreasurePool : { false, true })
            {
                const bool got  = ShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool want = charNull || alreadyInZone || hasTreasurePool;
                ok              = expect(got == want, "polarity: reject == OR of three") && ok;
                ok              = expect(!(got && !(charNull || alreadyInZone || hasTreasurePool)),
                            "polarity: reject only when any pole true") &&
                     ok;
                ok = expect(!(!got && (charNull || alreadyInZone || hasTreasurePool)),
                            "polarity: accept only when all false") &&
                     ok;
            }
        }
    }

    // Host-style inject poles: IncreaseZoneCounter entry.
    //   charNull        = PChar == nullptr
    //   alreadyInZone   = PChar != nullptr && PChar->loc.zone != nullptr
    //   hasTreasurePool = PChar != nullptr && PChar->PTreasurePool != nullptr
    for (const bool charNull : { false, true })
    {
        for (const bool alreadyInZone : { false, true })
        {
            for (const bool hasTreasurePool : { false, true })
            {
                const bool reject = ShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool want   = charNull || alreadyInZone || hasTreasurePool;
                ok                = expect(reject == want, "host inject dual-wire identity") && ok;
                ok                = expect(reject == inlineShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool),
                            "host inject free == inline") &&
                     ok;
                // Accept path is exclusive of reject path.
                const bool acceptEnter = !reject;
                ok                     = expect(acceptEnter != reject, "accept and reject exclusive") && ok;
            }
        }
    }

    // Production host-safe inject poles (null short-circuits zone/pool to false)
    // plus counterfactual pure poles.
    const struct
    {
        bool        charNull;
        bool        alreadyInZone;
        bool        hasTreasurePool;
        const char* label;
    } hostPoles[] = {
        { false, false, false, "fresh enter accept" },
        { true, false, false, "null char host-safe" },
        { false, true, false, "already in zone" },
        { false, false, true, "has treasure pool" },
        { false, true, true, "zone + pool both set" },
        // Counterfactual pure poles (production won't inject zone/pool true
        // when char is null because of PChar != nullptr guards):
        { true, true, false, "null + forced already zone" },
        { true, false, true, "null + forced has pool" },
        { true, true, true, "null + forced both" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRejectIncreaseZoneCounter(p.charNull, p.alreadyInZone, p.hasTreasurePool);
        const bool inlineF = inlineShouldRejectIncreaseZoneCounter(p.charNull, p.alreadyInZone, p.hasTreasurePool);
        const bool want    = p.charNull || p.alreadyInZone || p.hasTreasurePool;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host pole free == inline") && ok;
    }

    // Production IncreaseZoneCounter path semantics:
    // any reject pole → ShowWarning + return; all false → admit enter.
    ok = expect(ShouldRejectIncreaseZoneCounter(true, false, false), "IncreaseZoneCounter null → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, true, false), "IncreaseZoneCounter already zone → reject") && ok;
    ok = expect(ShouldRejectIncreaseZoneCounter(false, false, true), "IncreaseZoneCounter has pool → reject") && ok;
    ok = expect(!ShouldRejectIncreaseZoneCounter(false, false, false), "IncreaseZoneCounter clean enter → admit") && ok;

    // Dense compose: full 2³ free == inline == pin.
    for (const bool charNull : { false, true })
    {
        for (const bool alreadyInZone : { false, true })
        {
            for (const bool hasTreasurePool : { false, true })
            {
                const bool got  = ShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool);
                const bool want = charNull || alreadyInZone || hasTreasurePool;
                ok              = expect(got == want, "dense free == pin") && ok;
                ok              = expect(got == inlineShouldRejectIncreaseZoneCounter(charNull, alreadyInZone, hasTreasurePool),
                            "dense free == inline") &&
                     ok;
            }
        }
    }

    return ok;
}
