#include "test_notoriety_prune_mob_3029.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldPruneMobFromNotoriety 3029 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::hasEnmity per-entry prune for dual-wire
// cross-check (slice 3029):
//   if !isMob → false
//   else      → (isAlive && notOnEnmityList) || isDead
auto inlineShouldPruneMobFromNotoriety(
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
// (CNotorietyContainer::hasEnmity per-entry prune; slice 3029).
auto runNotorietyPruneMob3029SelfTests() -> bool
{
    using notorietyhelpers::ShouldPruneMobFromNotoriety;

    bool ok = true;

    // Residual 2807 pins still hold under dual-wire.
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, false, true), "residual non-mob live missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, false, true, false), "residual non-mob dead") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, false, false, true), "residual non-mob transitional") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, true), "residual mob live missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, true, false, false), "residual mob live present") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, false), "residual mob dead with owner") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, true), "residual mob transitional missing retained") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, false), "residual mob transitional present retained") && ok;

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
        const bool inlineF = inlineShouldPruneMobFromNotoriety(c.isMob, c.isAlive, c.isDead, c.notOnEnmityList);
        const bool wantPin = c.isMob ? ((c.isAlive && c.notOnEnmityList) || c.isDead) : false;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldPruneMobFromNotoriety dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldPruneMobFromNotoriety == pin formula") && ok;
    }

    // Dense non-mob edges: full 2^3 over isAlive/isDead/notOnEnmityList must
    // never prune (isMob=false short-circuits).
    for (const bool isAlive : { false, true })
    {
        for (const bool isDead : { false, true })
        {
            for (const bool notOnEnmityList : { false, true })
            {
                const bool got = ShouldPruneMobFromNotoriety(false, isAlive, isDead, notOnEnmityList);
                ok             = expect(!got, "non-mob dense must not prune") && ok;
                ok             = expect(got == inlineShouldPruneMobFromNotoriety(false, isAlive, isDead, notOnEnmityList),
                            "non-mob dense free == inline") &&
                     ok;
            }
        }
    }

    // Dense transitional edges: isMob=true, neither alive nor dead — both
    // notOnEnmityList poles retain.
    for (const bool notOnEnmityList : { false, true })
    {
        const bool got = ShouldPruneMobFromNotoriety(true, false, false, notOnEnmityList);
        ok             = expect(!got, "transitional dense must retain") && ok;
        ok             = expect(got == inlineShouldPruneMobFromNotoriety(true, false, false, notOnEnmityList),
                    "transitional dense free == inline") &&
             ok;
        ok = expect(got == ((false && notOnEnmityList) || false), "transitional dense pin formula") && ok;
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
                    const bool got  = ShouldPruneMobFromNotoriety(isMob, isAlive, isDead, notOnEnmityList);
                    const bool want = isMob ? ((isAlive && notOnEnmityList) || isDead) : false;
                    ok              = expect(got == want, "compose free == pin formula") && ok;
                    ok              = expect(got == inlineShouldPruneMobFromNotoriety(isMob, isAlive, isDead, notOnEnmityList),
                                "compose free == inline") &&
                         ok;
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
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, false, true), "hasEnmity non-mob → retain") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, true), "hasEnmity live missing → prune") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, true, false, false), "hasEnmity live present → retain") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, false), "hasEnmity dead → prune") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, true), "hasEnmity transitional → retain") && ok;

    // Explicit dual-wire: free function matches pin formula of injects.
    for (const bool isMob : { false, true })
    {
        for (const bool isAlive : { false, true })
        {
            for (const bool isDead : { false, true })
            {
                for (const bool notOnEnmityList : { false, true })
                {
                    const bool want = isMob ? ((isAlive && notOnEnmityList) || isDead) : false;
                    ok              = expect(ShouldPruneMobFromNotoriety(isMob, isAlive, isDead, notOnEnmityList) == want,
                                "host inject identity") &&
                         ok;
                    ok = expect(ShouldPruneMobFromNotoriety(isMob, isAlive, isDead, notOnEnmityList) ==
                                    inlineShouldPruneMobFromNotoriety(isMob, isAlive, isDead, notOnEnmityList),
                                "host inject dual-wire free == inline") &&
                         ok;
                }
            }
        }
    }

    return ok;
}
