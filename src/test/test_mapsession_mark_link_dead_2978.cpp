#include "test_mapsession_mark_link_dead_2978.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldMarkLinkDead 2978 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::cleanupSessions mark-link-dead gate for dual-wire
// cross-check (slice 2978):
//   hasChar && !alreadyLinkDead
auto inlineShouldMarkLinkDead(const bool hasChar, const bool alreadyLinkDead) -> bool
{
    return hasChar && !alreadyLinkDead;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldMarkLinkDead
// (cleanupSessions mark-link-dead gate hasChar && !alreadyLinkDead; slice 2978).
auto runMapsessionMarkLinkDead2978SelfTests() -> bool
{
    using mapsessionhelpers::ShouldMarkLinkDead;
    using mapsessionhelpers::ShouldRecoverLinkDead;

    bool ok = true;

    // Residual 2799 pins still hold under dual-wire.
    ok = expect(ShouldMarkLinkDead(true, false), "residual: has char not linkdead marks") && ok;
    ok = expect(!ShouldMarkLinkDead(true, true), "residual: already linkdead skips") && ok;
    ok = expect(!ShouldMarkLinkDead(false, false), "residual: no char rejects") && ok;
    ok = expect(!ShouldMarkLinkDead(false, true), "residual: no char ignores already flag") && ok;

    const struct
    {
        bool        hasChar;
        bool        alreadyLinkDead;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — has char, not already link-dead.
        { true, false, true, "has char not linkdead marks" },

        // Residual 2799 reject poles.
        { true, true, false, "already linkdead skips" },
        { false, false, false, "no char rejects" },
        { false, true, false, "no char ignores already flag" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool inlineF = inlineShouldMarkLinkDead(c.hasChar, c.alreadyLinkDead);
        const bool wantPin = c.hasChar && !c.alreadyLinkDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldMarkLinkDead dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldMarkLinkDead == pin formula hasChar && !alreadyLinkDead") && ok;
    }

    // Pin composition: only hasChar && !alreadyLinkDead admits.
    ok = expect(ShouldMarkLinkDead(true, false), "hasChar true, alreadyLinkDead false must admit") && ok;
    ok = expect(!ShouldMarkLinkDead(true, true), "alreadyLinkDead true must reject") && ok;
    ok = expect(!ShouldMarkLinkDead(false, false), "hasChar false must reject") && ok;
    ok = expect(!ShouldMarkLinkDead(false, true), "both false/true poles must reject") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
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
        }
    }

    // --- Production MapSessionContainer::cleanupSessions mark path semantics ---
    // Host injects (inside now > last_update + 5s; timeout gate outside pure):
    //   hasChar          = PChar != nullptr
    //   alreadyLinkDead  = hasChar && PChar->isLinkDead
    // when true  → PlanLinkDeadMark body (SQL / set isLinkDead / mask / SpawnPCs)
    // when false → skip mark body
    ok = expect(ShouldMarkLinkDead(true, false), "mark hasChar not-linkdead → plan body path") && ok;
    ok = expect(!ShouldMarkLinkDead(true, true), "mark already linkdead → skip") && ok;
    ok = expect(!ShouldMarkLinkDead(false, false), "mark no char → skip") && ok;
    ok = expect(!ShouldMarkLinkDead(false, true), "mark no char already-flag → skip") && ok;

    // Explicit dual-wire: free function is hasChar && !alreadyLinkDead of injects.
    for (const bool hasChar : { false, true })
    {
        for (const bool alreadyLinkDead : { false, true })
        {
            ok = expect(ShouldMarkLinkDead(hasChar, alreadyLinkDead) ==
                            (hasChar && !alreadyLinkDead),
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldMarkLinkDead(hasChar, alreadyLinkDead) ==
                            inlineShouldMarkLinkDead(hasChar, alreadyLinkDead),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Sibling residual independence: ShouldRecoverLinkDead remains inverted
    // polarity on the link-dead bit (not dual-wired in this slice).
    ok = expect(ShouldMarkLinkDead(true, false) && !ShouldRecoverLinkDead(true, false),
                "mark admits when not linkdead; recover does not") &&
         ok;
    ok = expect(!ShouldMarkLinkDead(true, true) && ShouldRecoverLinkDead(true, true),
                "recover admits when linkdead; mark does not") &&
         ok;

    return ok;
}
