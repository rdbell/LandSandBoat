#include "test_mapsession_recover_link_dead_3350.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldRecoverLinkDead 3350 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::cleanupSessions recover-link-dead gate for dual-wire
// cross-check (dedicated slice 3350):
//   hasChar && isLinkDead
auto inlineShouldRecoverLinkDead(const bool hasChar, const bool isLinkDead) -> bool
{
    return hasChar && isLinkDead;
}

// Compact dual-wire pin matching Go pinShouldRecoverLinkDead3350 /
// C++ capacity (positive if form — avoid De Morgan rewrites):
//   if (!hasChar) return false;
//   if (!isLinkDead) return false;
//   return true;
auto pinShouldRecoverLinkDead(const bool hasChar, const bool isLinkDead) -> bool
{
    if (!hasChar)
    {
        return false;
    }
    if (!isLinkDead)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldRecoverLinkDead
// (hasChar && isLinkDead; dedicated slice 3350 expand residual 2985 /
// pure 2799).
//
// Coverage:
//   - free == inline == pin == (hasChar && isLinkDead)
//   - residual 2799 / 2985 pins still hold
//   - full 2² bool poles (hasChar × isLinkDead)
//   - sibling residual independence (ShouldMarkLinkDead / 3218,
//     ShouldCreatePendingSession / 3207, ShouldCreateSession / 3191 not re-expanded)
auto runMapsessionRecoverLinkDead3350SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;
    using mapsessionhelpers::ShouldMarkLinkDead;
    using mapsessionhelpers::ShouldRecoverLinkDead;

    bool ok = true;

    // Residual 2799 / 2985 pins still hold under dual-wire.
    ok = expect(ShouldRecoverLinkDead(true, true), "residual 2985/2799: has char linkdead recovers") && ok;
    ok = expect(!ShouldRecoverLinkDead(true, false), "residual 2985/2799: not linkdead skips") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, true), "residual 2985/2799: no char rejects") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, false), "residual 2985/2799: no char not linkdead") && ok;

    const struct
    {
        bool        hasChar;
        bool        isLinkDead;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, true, "pole has char linkdead recovers" },
        { true, false, false, "pole not linkdead skips" },
        { false, true, false, "pole no char rejects" },
        { false, false, false, "pole no char not linkdead" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool inlineF = inlineShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool pinF    = pinShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool wantPin = c.hasChar && c.isLinkDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRecoverLinkDead dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldRecoverLinkDead dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldRecoverLinkDead == pin formula hasChar && isLinkDead") && ok;
    }

    // Pin composition: only hasChar && isLinkDead admits.
    ok = expect(ShouldRecoverLinkDead(true, true), "hasChar true, isLinkDead true must admit") && ok;
    ok = expect(!ShouldRecoverLinkDead(true, false), "isLinkDead false must reject") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, true), "hasChar false must reject") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, false), "both false poles must reject") && ok;

    // Explicit polarity: recover only when hasChar && isLinkDead.
    for (const bool hasChar : { false, true })
    {
        for (const bool isLinkDead : { false, true })
        {
            const bool got  = ShouldRecoverLinkDead(hasChar, isLinkDead);
            const bool want = hasChar && isLinkDead;
            ok              = expect(got == want, "polarity: free == pin formula") && ok;
            // Explicit poles (positive form; avoid De Morgan on !(a && b)).
            if (hasChar && isLinkDead)
            {
                ok = expect(got, "polarity: always recover when hasChar && isLinkDead") && ok;
            }
            else
            {
                ok = expect(!got, "polarity: never recover unless hasChar && isLinkDead") && ok;
            }
        }
    }

    // Host path: cleanupSessions recover inject on else of >5s inactive branch.
    // Timeout gate (else of now > last_update + 5s / TimeoutActive) is outside pure.
    const struct
    {
        bool        hasChar;
        bool        isLinkDead;
        bool        wantRecover;
        const char* label;
    } hostCases[] = {
        { true, true, true, "hasChar linkdead → PlanLinkDeadRecover body" },
        { true, false, false, "not linkdead → skip recover body" },
        { false, true, false, "no char → skip recover body" },
        { false, false, false, "no char not-linkdead → skip recover body" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool inlineF = inlineShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool pinF    = pinShouldRecoverLinkDead(c.hasChar, c.isLinkDead);

        ok = expect(got == c.wantRecover, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == (c.hasChar && c.isLinkDead),
                    "host compose free == hasChar && isLinkDead") &&
             ok;
    }

    // Dense compose: free == inline == pin formula for full 2×2 (2²).
    for (const bool hasChar : { false, true })
    {
        for (const bool isLinkDead : { false, true })
        {
            const bool got  = ShouldRecoverLinkDead(hasChar, isLinkDead);
            const bool want = hasChar && isLinkDead;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRecoverLinkDead(hasChar, isLinkDead),
                        "compose free == inline") &&
                 ok;
            ok = expect(got == pinShouldRecoverLinkDead(hasChar, isLinkDead), "compose free == pin") && ok;
        }
    }

    // Residual independence (2799 / 2985 / 3218): recover uses inverted polarity
    // on the link-dead bit vs mark. Sibling dual-wires left alone.
    ok = expect(ShouldMarkLinkDead(true, false) && !ShouldRecoverLinkDead(true, false),
                "mark admits when not linkdead; recover does not") &&
         ok;
    ok = expect(!ShouldMarkLinkDead(true, true) && ShouldRecoverLinkDead(true, true),
                "recover admits when linkdead; mark does not") &&
         ok;
    ok = expect(!ShouldMarkLinkDead(false, false) && !ShouldRecoverLinkDead(false, true),
                "no-char poles reject both mark and recover") &&
         ok;
    // Sibling create dual-wires not re-expanded under this suite (3191 / 3207).
    ok = expect(!ShouldCreateSession(true, false) && ShouldCreatePendingSession(true),
                "sibling create dual-wires remain independent (empty row / pending queryOK)") &&
         ok;
    ok = expect(!ShouldCreateSession(false, false) && !ShouldCreatePendingSession(false),
                "sibling create dual-wires remain independent (query fail poles)") &&
         ok;
    ok = expect(ShouldCreateSession(true, true) && ShouldCreatePendingSession(true),
                "sibling create dual-wires remain independent (full-true poles)") &&
         ok;

    return ok;
}
