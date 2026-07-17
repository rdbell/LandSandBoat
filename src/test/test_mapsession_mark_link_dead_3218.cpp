#include "test_mapsession_mark_link_dead_3218.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldMarkLinkDead 3218 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::cleanupSessions mark-link-dead gate for dual-wire
// cross-check (dedicated slice 3218):
//   hasChar && !alreadyLinkDead
auto inlineShouldMarkLinkDead(const bool hasChar, const bool alreadyLinkDead) -> bool
{
    return hasChar && !alreadyLinkDead;
}

// Compact dual-wire pin matching Go pinShouldMarkLinkDead3218 /
// C++ capacity (positive if form — avoid De Morgan rewrites):
//   if (!hasChar) return false;
//   if (alreadyLinkDead) return false;
//   return true;
auto pinShouldMarkLinkDead(const bool hasChar, const bool alreadyLinkDead) -> bool
{
    if (!hasChar)
    {
        return false;
    }
    if (alreadyLinkDead)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldMarkLinkDead
// (hasChar && !alreadyLinkDead; dedicated slice 3218 expand residual 2978 /
// pure 2799).
//
// Coverage:
//   - free == inline == pin == (hasChar && !alreadyLinkDead)
//   - residual 2799 / 2978 pins still hold
//   - full 2² bool poles (hasChar × alreadyLinkDead)
//   - sibling residual independence (ShouldRecoverLinkDead / 2985,
//     ShouldCreatePendingSession / 3207, ShouldCreateSession / 3191 not re-expanded)
auto runMapsessionMarkLinkDead3218SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;
    using mapsessionhelpers::ShouldMarkLinkDead;
    using mapsessionhelpers::ShouldRecoverLinkDead;

    bool ok = true;

    // Residual 2799 / 2978 pins still hold under dual-wire.
    ok = expect(ShouldMarkLinkDead(true, false), "residual 2978/2799: has char not linkdead marks") && ok;
    ok = expect(!ShouldMarkLinkDead(true, true), "residual 2978/2799: already linkdead skips") && ok;
    ok = expect(!ShouldMarkLinkDead(false, false), "residual 2978/2799: no char rejects") && ok;
    ok = expect(!ShouldMarkLinkDead(false, true), "residual 2978/2799: no char ignores already flag") && ok;

    const struct
    {
        bool        hasChar;
        bool        alreadyLinkDead;
        bool        want;
        const char* label;
    } cases[] = {
        { true, false, true, "pole has char not linkdead marks" },
        { true, true, false, "pole already linkdead skips" },
        { false, false, false, "pole no char rejects" },
        { false, true, false, "pole no char ignores already flag" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool inlineF = inlineShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool pinF    = pinShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool wantPin = c.hasChar && !c.alreadyLinkDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldMarkLinkDead dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldMarkLinkDead dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldMarkLinkDead == pin formula hasChar && !alreadyLinkDead") && ok;
    }

    // Pin composition: only hasChar && !alreadyLinkDead admits.
    ok = expect(ShouldMarkLinkDead(true, false), "hasChar true, alreadyLinkDead false must admit") && ok;
    ok = expect(!ShouldMarkLinkDead(true, true), "alreadyLinkDead true must reject") && ok;
    ok = expect(!ShouldMarkLinkDead(false, false), "hasChar false must reject") && ok;
    ok = expect(!ShouldMarkLinkDead(false, true), "both false/true poles must reject") && ok;

    // Explicit polarity: mark only when hasChar && !alreadyLinkDead.
    for (const bool hasChar : { false, true })
    {
        for (const bool alreadyLinkDead : { false, true })
        {
            const bool got  = ShouldMarkLinkDead(hasChar, alreadyLinkDead);
            const bool want = hasChar && !alreadyLinkDead;
            ok              = expect(got == want, "polarity: free == pin formula") && ok;
            // Explicit poles (positive form; avoid De Morgan on !(a && !b)).
            if (hasChar && !alreadyLinkDead)
            {
                ok = expect(got, "polarity: always mark when hasChar && !alreadyLinkDead") && ok;
            }
            else
            {
                ok = expect(!got, "polarity: never mark unless hasChar && !alreadyLinkDead") && ok;
            }
        }
    }

    // Host path: cleanupSessions mark inject inside >5s inactive branch.
    // Timeout gate (now > last_update + 5s) is outside pure.
    const struct
    {
        bool        hasChar;
        bool        alreadyLinkDead;
        bool        wantMark;
        const char* label;
    } hostCases[] = {
        { true, false, true, "hasChar not-linkdead → PlanLinkDeadMark body" },
        { true, true, false, "already linkdead → skip mark body" },
        { false, false, false, "no char → skip mark body" },
        { false, true, false, "no char already-flag → skip mark body" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool inlineF = inlineShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool pinF    = pinShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);

        ok = expect(got == c.wantMark, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == (c.hasChar && !c.alreadyLinkDead),
                    "host compose free == hasChar && !alreadyLinkDead") &&
             ok;
    }

    // Dense compose: free == inline == pin formula for full 2×2 (2²).
    for (const bool hasChar : { false, true })
    {
        for (const bool alreadyLinkDead : { false, true })
        {
            const bool got  = ShouldMarkLinkDead(hasChar, alreadyLinkDead);
            const bool want = hasChar && !alreadyLinkDead;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldMarkLinkDead(hasChar, alreadyLinkDead),
                        "compose free == inline") &&
                 ok;
            ok = expect(got == pinShouldMarkLinkDead(hasChar, alreadyLinkDead), "compose free == pin") && ok;
        }
    }

    // Residual independence (2799 / 2978 / 2985): mark uses inverted polarity
    // on the link-dead bit vs recover. Sibling dual-wires left alone.
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
