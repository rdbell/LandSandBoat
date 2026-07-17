#include "test_notoriety_prune_mob_3208.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldPruneMobFromNotoriety 3208 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::hasEnmity per-entry prune for dual-wire
// cross-check (dedicated 3208):
//   if !isMob → false
//   else      → (isAlive && notOnEnmityList) || isDead
auto inlineShouldPruneMobFromNotoriety3208(
    const bool isMob,
    const bool isAlive,
    const bool isDead,
    const bool notOnEnmityList) -> bool
{
    if (!isMob)
    {
        return false;
    }
    return (isAlive && notOnEnmityList) || isDead;
}

// Compact dual-wire pin matching Go pinShouldPruneMobFromNotoriety3208 / C++
// capacity positive if/else form (formula unchanged from 2807 / 3029). Avoid
// De Morgan compact rewrites — keep free-function if/else shape.
auto pinShouldPruneMobFromNotoriety3208(
    const bool isMob,
    const bool isAlive,
    const bool isDead,
    const bool notOnEnmityList) -> bool
{
    if (!isMob)
    {
        return false;
    }
    return (isAlive && notOnEnmityList) || isDead;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldPruneMobFromNotoriety
// (CNotorietyContainer::hasEnmity per-entry prune four-bool;
// OmegaXI internal/notoriety; dedicated slice 3208; residual expand 3029 / pure 2807).
//
// Coverage:
//   - free == inline == pin positive if/else form
//   - residual 3029 / 2807 pins still hold
//   - dense poles: non-mob 2^3, transitional, full 2^4 free == inline == pin
//   - host inject poles
auto runNotorietyPruneMob3208SelfTests() -> bool
{
    using notorietyhelpers::ShouldPruneMobFromNotoriety;

    bool ok = true;

    // Residual 2807 / 3029 pins still hold under dedicated dual-wire.
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, false, true), "residual non-mob live missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, false, true, false), "residual non-mob dead") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, false, false, true), "residual non-mob transitional") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, true), "residual mob live missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, true, false, false), "residual mob live present") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, false), "residual mob dead with owner") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, true), "residual mob transitional missing retained") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, false), "residual mob transitional present retained") && ok;

    // --- Core poles: free == inline == pin positive if/else form ---
    const struct
    {
        bool        isMob;
        bool        isAlive;
        bool        isDead;
        bool        notOnEnmityList;
        bool        want;
        const char* label;
    } cases[] = {
        // Non-mob never pruned (dense edges across liveness / enmity).
        { false, true, false, true, false, "non-mob live missing" },
        { false, true, false, false, false, "non-mob live present" },
        { false, false, true, true, false, "non-mob dead missing" },
        { false, false, true, false, false, "non-mob dead present" },
        { false, false, false, true, false, "non-mob transitional missing" },
        { false, false, false, false, false, "non-mob transitional present" },
        { false, true, true, true, false, "non-mob both missing" },
        { false, true, true, false, false, "non-mob both present" },

        // Mob classic poles.
        { true, true, false, true, true, "mob live missing" },
        { true, true, false, false, false, "mob live present" },
        { true, false, true, true, true, "mob dead missing" },
        { true, false, true, false, true, "mob dead present" },

        // Transitional (neither alive nor dead) — retain both enmity poles.
        { true, false, false, true, false, "mob transitional missing retained" },
        { true, false, false, false, false, "mob transitional present retained" },

        // Host may inject alive+dead both true; dead branch still prunes.
        { true, true, true, true, true, "mob both missing" },
        { true, true, true, false, true, "mob both present" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPruneMobFromNotoriety(c.isMob, c.isAlive, c.isDead, c.notOnEnmityList);
        const bool inlineF = inlineShouldPruneMobFromNotoriety3208(c.isMob, c.isAlive, c.isDead, c.notOnEnmityList);
        const bool pin     = pinShouldPruneMobFromNotoriety3208(c.isMob, c.isAlive, c.isDead, c.notOnEnmityList);
        // Positive if/else form pin composition (matches free function).
        const bool wantPin = c.isMob ? ((c.isAlive && c.notOnEnmityList) || c.isDead) : false;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldPruneMobFromNotoriety free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldPruneMobFromNotoriety(false, true, false, true) ==
                    pinShouldPruneMobFromNotoriety3208(false, true, false, true),
                "free == pin non-mob live missing") &&
         ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, true) ==
                    pinShouldPruneMobFromNotoriety3208(true, true, false, true),
                "free == pin mob live missing") &&
         ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, false) ==
                    pinShouldPruneMobFromNotoriety3208(true, true, false, false),
                "free == pin mob live present") &&
         ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, false) ==
                    pinShouldPruneMobFromNotoriety3208(true, false, true, false),
                "free == pin mob dead") &&
         ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, false, true) ==
                    pinShouldPruneMobFromNotoriety3208(true, false, false, true),
                "free == pin mob transitional") &&
         ok;

    // Dense non-mob edges: full 2^3 over isAlive/isDead/notOnEnmityList must
    // never prune (isMob=false short-circuits); free == inline == pin.
    for (const bool isAlive : { false, true })
    {
        for (const bool isDead : { false, true })
        {
            for (const bool notOnEnmityList : { false, true })
            {
                const bool got     = ShouldPruneMobFromNotoriety(false, isAlive, isDead, notOnEnmityList);
                const bool inlineF = inlineShouldPruneMobFromNotoriety3208(false, isAlive, isDead, notOnEnmityList);
                const bool pin     = pinShouldPruneMobFromNotoriety3208(false, isAlive, isDead, notOnEnmityList);
                ok                 = expect(!got, "non-mob dense must not prune") && ok;
                ok                 = expect(got == inlineF && got == pin, "non-mob dense free == inline == pin") && ok;
            }
        }
    }

    // Dense transitional edges: isMob=true, neither alive nor dead — both
    // notOnEnmityList poles retain; free == inline == pin.
    for (const bool notOnEnmityList : { false, true })
    {
        const bool got     = ShouldPruneMobFromNotoriety(true, false, false, notOnEnmityList);
        const bool inlineF = inlineShouldPruneMobFromNotoriety3208(true, false, false, notOnEnmityList);
        const bool pin     = pinShouldPruneMobFromNotoriety3208(true, false, false, notOnEnmityList);
        ok                 = expect(!got, "transitional dense must retain") && ok;
        ok                 = expect(got == inlineF && got == pin, "transitional dense free == inline == pin") && ok;
    }

    // Dense compose: full 2^4 boolean space free == inline == pin.
    for (const bool isMob : { false, true })
    {
        for (const bool isAlive : { false, true })
        {
            for (const bool isDead : { false, true })
            {
                for (const bool notOnEnmityList : { false, true })
                {
                    const bool got     = ShouldPruneMobFromNotoriety(isMob, isAlive, isDead, notOnEnmityList);
                    const bool inlineF = inlineShouldPruneMobFromNotoriety3208(isMob, isAlive, isDead, notOnEnmityList);
                    const bool pin     = pinShouldPruneMobFromNotoriety3208(isMob, isAlive, isDead, notOnEnmityList);
                    const bool want    = isMob ? ((isAlive && notOnEnmityList) || isDead) : false;
                    ok                 = expect(got == want, "compose free == pin formula") && ok;
                    ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
                }
            }
        }
    }

    // --- Production CNotorietyContainer::hasEnmity per-entry path semantics ---
    // Host injects:
    //   isMob           = dynamic_cast<CMobEntity*>(entry) != nullptr
    //   isAlive/isDead  = mob liveness when isMob; else false
    //   notOnEnmityList = enmity list missing owner id (uint16) when isMob
    // when true  → collect entry for deferred remove
    // when false → retain entry
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, false, true) &&
                    !pinShouldPruneMobFromNotoriety3208(false, true, false, true),
                "hasEnmity non-mob → retain free/pin dual-wire") &&
         ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, true) &&
                    pinShouldPruneMobFromNotoriety3208(true, true, false, true),
                "hasEnmity live missing → prune free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, true, false, false), "hasEnmity live present → retain") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, false), "hasEnmity dead → prune") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, true), "hasEnmity transitional → retain") && ok;

    // Explicit dual-wire: free == inline == pin for host-style poles.
    const struct
    {
        bool        isMob;
        bool        isAlive;
        bool        isDead;
        bool        notOnEnmityList;
        const char* label;
    } hostPoles[] = {
        { false, true, false, true, "non-mob retain" },
        { true, true, false, true, "live missing prune" },
        { true, true, false, false, "live present retain" },
        { true, false, true, false, "dead prune" },
        { true, false, false, true, "transitional retain" },
        { true, false, false, false, "transitional present retain" },
        { true, true, true, false, "alive+dead prune via dead" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldPruneMobFromNotoriety(p.isMob, p.isAlive, p.isDead, p.notOnEnmityList);
        const bool inlineF = inlineShouldPruneMobFromNotoriety3208(p.isMob, p.isAlive, p.isDead, p.notOnEnmityList);
        const bool pin     = pinShouldPruneMobFromNotoriety3208(p.isMob, p.isAlive, p.isDead, p.notOnEnmityList);
        const bool want    = p.isMob ? ((p.isAlive && p.notOnEnmityList) || p.isDead) : false;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
