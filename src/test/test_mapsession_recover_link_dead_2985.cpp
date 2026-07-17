#include "test_mapsession_recover_link_dead_2985.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldRecoverLinkDead 2985 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::cleanupSessions recover-link-dead gate for dual-wire
// cross-check (slice 2985):
//   hasChar && isLinkDead
auto inlineShouldRecoverLinkDead(const bool hasChar, const bool isLinkDead) -> bool
{
    return hasChar && isLinkDead;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldRecoverLinkDead
// (cleanupSessions recover-link-dead gate hasChar && isLinkDead; slice 2985).
auto runMapsessionRecoverLinkDead2985SelfTests() -> bool
{
    using mapsessionhelpers::ShouldMarkLinkDead;
    using mapsessionhelpers::ShouldRecoverLinkDead;

    bool ok = true;

    // Residual 2799 pins still hold under dual-wire.
    ok = expect(ShouldRecoverLinkDead(true, true), "residual: has char linkdead recovers") && ok;
    ok = expect(!ShouldRecoverLinkDead(true, false), "residual: not linkdead skips") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, true), "residual: no char rejects") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, false), "residual: no char not linkdead") && ok;

    const struct
    {
        bool        hasChar;
        bool        isLinkDead;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — has char, is link-dead.
        { true, true, true, "has char linkdead recovers" },

        // Residual 2799 reject poles.
        { true, false, false, "not linkdead skips" },
        { false, true, false, "no char rejects" },
        { false, false, false, "no char not linkdead" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool inlineF = inlineShouldRecoverLinkDead(c.hasChar, c.isLinkDead);
        const bool wantPin = c.hasChar && c.isLinkDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRecoverLinkDead dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRecoverLinkDead == pin formula hasChar && isLinkDead") && ok;
    }

    // Pin composition: only hasChar && isLinkDead admits.
    ok = expect(ShouldRecoverLinkDead(true, true), "hasChar true, isLinkDead true must admit") && ok;
    ok = expect(!ShouldRecoverLinkDead(true, false), "isLinkDead false must reject") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, true), "hasChar false must reject") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, false), "both false poles must reject") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
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
        }
    }

    // --- Production MapSessionContainer::cleanupSessions recover path semantics ---
    // Host injects (else of now > last_update + 5s; timeout gate outside pure):
    //   hasChar    = PChar != nullptr
    //   isLinkDead = hasChar && PChar->isLinkDead
    // when true  → PlanLinkDeadRecover body (SQL / clear isLinkDead / mask /
    //              SpawnPCs / SaveCharStats)
    // when false → skip recover body
    ok = expect(ShouldRecoverLinkDead(true, true), "recover hasChar linkdead → plan body path") && ok;
    ok = expect(!ShouldRecoverLinkDead(true, false), "recover not linkdead → skip") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, true), "recover no char → skip") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, false), "recover no char not-linkdead → skip") && ok;

    // Explicit dual-wire: free function is hasChar && isLinkDead of injects.
    for (const bool hasChar : { false, true })
    {
        for (const bool isLinkDead : { false, true })
        {
            ok = expect(ShouldRecoverLinkDead(hasChar, isLinkDead) == (hasChar && isLinkDead),
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldRecoverLinkDead(hasChar, isLinkDead) ==
                            inlineShouldRecoverLinkDead(hasChar, isLinkDead),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Sibling dual-wire independence: ShouldMarkLinkDead uses inverted polarity
    // on the link-dead bit (!alreadyLinkDead vs isLinkDead). mark stays independent.
    ok = expect(ShouldMarkLinkDead(true, false) && !ShouldRecoverLinkDead(true, false),
                "mark admits when not linkdead; recover does not") &&
         ok;
    ok = expect(!ShouldMarkLinkDead(true, true) && ShouldRecoverLinkDead(true, true),
                "recover admits when linkdead; mark does not") &&
         ok;
    ok = expect(!ShouldMarkLinkDead(false, false) && !ShouldRecoverLinkDead(false, true),
                "no-char rejects both mark and recover") &&
         ok;

    return ok;
}
