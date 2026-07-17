#include "test_mapsession_destroy_pending_ptr_3721.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldDestroyPendingByPointer 3721 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline MapSessionContainer::destroyPendingSession(MapSession*) gate for
// dual-wire cross-check (dedicated slice 3721):
//   found && pointerMatches
auto inlineShouldDestroyPendingByPointer(const bool found, const bool pointerMatches) -> bool
{
    return found && pointerMatches;
}

// Compact dual-wire pin matching Go pinShouldDestroyPendingByPointer3721 /
// C++ capacity (positive if form — avoid De Morgan rewrites):
//   if (!found) return false;
//   if (!pointerMatches) return false;
//   return true;
auto pinShouldDestroyPendingByPointer(const bool found, const bool pointerMatches) -> bool
{
    if (!found)
    {
        return false;
    }
    if (!pointerMatches)
    {
        return false;
    }
    return true;
}

// Prior dedicated slice 3676 inline/pin for independence cross-check.
auto inlineShouldDestroyPendingByPointer3676(const bool found, const bool pointerMatches) -> bool
{
    return found && pointerMatches;
}

auto pinShouldDestroyPendingByPointer3676(const bool found, const bool pointerMatches) -> bool
{
    if (!found)
    {
        return false;
    }
    if (!pointerMatches)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldDestroyPendingByPointer
// (found && pointerMatches; dedicated slice 3721 expand residual 3056 /
// pure 2787; prior dedicated 3676 / 3631 / 3586 / 3542 / 3480 / 3426 / 3376 retained).
//
// Coverage:
//   - free == inline == pin == pin3676 == (found && pointerMatches)
//   - residual 2787 / 3056 / prior dedicated 3376 / 3426 / 3480 / 3542 / 3586 / 3631 / 3676 pins still hold
//   - full 2² bool poles (found × pointerMatches)
//   - prior dedicated 3676 independence (free == 3676 inline/pin)
//   - sibling residual independence (ShouldDestroyPendingByCharID / 3066,
//     ShouldCreateSession / 3191, ShouldCreatePendingSession / 3207 left alone;
//     recover_link_dead not thrashed)
auto runMapsessionDestroyPendingPtr3721SelfTests() -> bool
{
    using mapsessionhelpers::ShouldCreatePendingSession;
    using mapsessionhelpers::ShouldCreateSession;
    using mapsessionhelpers::ShouldDestroyPendingByCharID;
    using mapsessionhelpers::ShouldDestroyPendingByPointer;

    bool ok = true;

    // Residual 2787 / 3056 / prior dedicated 3376 / 3426 / 3480 / 3542 / 3586 / 3631 / 3676 pins still hold under dual-wire.
    ok = expect(ShouldDestroyPendingByPointer(true, true), "residual 3676/3631/3586/3542/3480/3426/3376/3056/2787: found match destroys") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(true, false), "residual 3676/3631/3586/3542/3480/3426/3376/3056/2787: found mismatch rejects") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false), "residual 3676/3631/3586/3542/3480/3426/3376/3056/2787: missing rejects") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, true), "residual 3676/3631/3586/3542/3480/3426/3376/3056/2787: missing ignores match flag") && ok;

    const struct
    {
        bool        found;
        bool        pointerMatches;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, true, "pole found match destroys" },
        { true, false, false, "pole found mismatch rejects" },
        { false, false, false, "pole missing rejects" },
        { false, true, false, "pole missing ignores match flag" },
    };

    for (const auto& c : cases)
    {
        const bool got      = ShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool inlineF  = inlineShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool pinF     = pinShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool pinPrior = pinShouldDestroyPendingByPointer3676(c.found, c.pointerMatches);
        const bool wantPin  = c.found && c.pointerMatches;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDestroyPendingByPointer dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldDestroyPendingByPointer dual-wire == pin formula") && ok;
        ok = expect(got == pinPrior, "ShouldDestroyPendingByPointer dual-wire == pin3676") && ok;
        ok = expect(got == wantPin, "ShouldDestroyPendingByPointer == pin formula found && pointerMatches") && ok;
    }

    // Pin composition: only found && pointerMatches admits.
    ok = expect(ShouldDestroyPendingByPointer(true, true), "found true, pointerMatches true must admit") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(true, false), "pointerMatches false must reject") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false), "found false must reject") && ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, true), "both false/true poles must reject") && ok;

    // Explicit polarity: destroy only when found && pointerMatches.
    for (const bool found : { false, true })
    {
        for (const bool pointerMatches : { false, true })
        {
            const bool got  = ShouldDestroyPendingByPointer(found, pointerMatches);
            const bool want = found && pointerMatches;
            ok              = expect(got == want, "polarity: free == pin formula") && ok;
            // Explicit poles (positive form; avoid De Morgan on !(a && b)).
            if (found && pointerMatches)
            {
                ok = expect(got, "polarity: always destroy when found && pointerMatches") && ok;
            }
            else
            {
                ok = expect(!got, "polarity: never destroy unless found && pointerMatches") && ok;
            }
        }
    }

    // Host path: destroyPendingSession(MapSession*) inject after null guard.
    // Null session guard is outside pure.
    const struct
    {
        bool        found;
        bool        pointerMatches;
        bool        wantDestroy;
        const char* label;
    } hostCases[] = {
        { true, true, true, "found match → erase pending + delete MapSession" },
        { true, false, false, "found mismatch → skip erase/delete" },
        { false, false, false, "missing → skip erase/delete" },
        { false, true, false, "missing ignore-match → skip erase/delete" },
    };

    for (const auto& c : hostCases)
    {
        const bool got      = ShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool inlineF  = inlineShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool pinF     = pinShouldDestroyPendingByPointer(c.found, c.pointerMatches);
        const bool pinPrior = pinShouldDestroyPendingByPointer3676(c.found, c.pointerMatches);

        ok = expect(got == c.wantDestroy, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == pinPrior, "host compose dual-wire free == pin3676") && ok;
        ok = expect(got == (c.found && c.pointerMatches),
                    "host compose free == found && pointerMatches") &&
             ok;
    }

    // Dense compose: free == inline == pin == pin3676 residual pins for full 2×2 (2²).
    for (const bool found : { false, true })
    {
        for (const bool pointerMatches : { false, true })
        {
            const bool got  = ShouldDestroyPendingByPointer(found, pointerMatches);
            const bool want = found && pointerMatches;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldDestroyPendingByPointer(found, pointerMatches),
                        "compose free == inline") &&
                 ok;
            ok = expect(got == pinShouldDestroyPendingByPointer(found, pointerMatches), "compose free == pin") && ok;
            ok = expect(got == pinShouldDestroyPendingByPointer3676(found, pointerMatches),
                        "compose free == pin3676") &&
                 ok;
        }
    }

    // Residual independence (2787 / 3056 / 3066 / 3376 / 3426 / 3480 / 3542 / 3586 / 3631 / 3676): pointer path needs identity;
    // charID path needs presence only. Sibling dual-wires left alone.
    // Prior dedicated 3676 independence: free == prior suite inline/pin forms.
    ok = expect(!ShouldDestroyPendingByPointer(true, false) && ShouldDestroyPendingByCharID(true),
                "mismatch must only block pointer destroyPendingSession") &&
         ok;
    ok = expect(ShouldDestroyPendingByPointer(true, true) && ShouldDestroyPendingByCharID(true),
                "found match admits both pointer and charID residual paths") &&
         ok;
    ok = expect(!ShouldDestroyPendingByPointer(false, false) && !ShouldDestroyPendingByCharID(false),
                "missing must reject both pointer and charID residual paths") &&
         ok;
    for (const bool found : { false, true })
    {
        for (const bool pointerMatches : { false, true })
        {
            const bool got = ShouldDestroyPendingByPointer(found, pointerMatches);
            ok             = expect(got == inlineShouldDestroyPendingByPointer3676(found, pointerMatches),
                        "prior 3676 independence: free == 3676 inline") &&
                 ok;
            ok = expect(got == pinShouldDestroyPendingByPointer3676(found, pointerMatches),
                        "prior 3676 independence: free == 3676 pin") &&
                 ok;
        }
    }
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
