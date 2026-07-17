#include "test_interaction_skip_priority_2953.h"

#include "map/interaction_lookup_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "interactionlookup ShouldSkipPrioritySelection 2953 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline interaction_lookup.lua getHighestPriorityActions early-return gate
// for dual-wire cross-check (slice 2953):
//   empty || firstIsNumber
auto inlineShouldSkipPrioritySelection(const bool empty, const bool firstIsNumber) -> bool
{
    return empty || firstIsNumber;
}

} // namespace

// Pure dual-wire expansion for interactionlookuphelpers::ShouldSkipPrioritySelection
// (getHighestPriorityActions empty/number → Progress; slice 2953).
auto runInteractionSkipPriority2953SelfTests() -> bool
{
    using interactionlookuphelpers::ShouldSkipPrioritySelection;
    using interactionlookuphelpers::SkipPriorityMaxPriority;

    bool ok = true;

    ok = expect(SkipPriorityMaxPriority == 1000, "SkipPriorityMaxPriority pin == 1000") && ok;

    const struct
    {
        bool        empty;
        bool        firstIsNumber;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles (full 2^2 boolean space).
        { false, false, false, "normal candidates → no skip" },
        { true, false, true, "empty → skip" },
        { false, true, true, "firstIsNumber → skip" },
        { true, true, true, "both empty and number → skip" },

        // Residual 1094 pins.
        { true, false, true, "residual empty → skip" },
        { false, true, true, "residual firstIsNumber → skip" },
        { true, true, true, "residual both → skip" },
        { false, false, false, "residual normal → no skip" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipPrioritySelection(c.empty, c.firstIsNumber);
        const bool inlineF = inlineShouldSkipPrioritySelection(c.empty, c.firstIsNumber);
        const bool wantPin = c.empty || c.firstIsNumber;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipPrioritySelection dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSkipPrioritySelection == pin formula empty||firstIsNumber") && ok;
    }

    // Pin composition: OR identity poles.
    ok = expect(ShouldSkipPrioritySelection(true, false), "empty must skip") && ok;
    ok = expect(ShouldSkipPrioritySelection(false, true), "firstIsNumber must skip") && ok;
    ok = expect(ShouldSkipPrioritySelection(true, true), "both must skip") && ok;
    ok = expect(!ShouldSkipPrioritySelection(false, false), "neither must not skip") && ok;

    // Dense compose: full 2^2 boolean space.
    for (const bool empty : { false, true })
    {
        for (const bool firstIsNumber : { false, true })
        {
            const bool got  = ShouldSkipPrioritySelection(empty, firstIsNumber);
            const bool want = empty || firstIsNumber;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldSkipPrioritySelection(empty, firstIsNumber),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production getHighestPriorityActions path semantics ---
    // Host injects:
    //   empty         = (#possibleActions == 0)
    //   firstIsNumber = (type(possibleActions[1]) == 'number')
    // when true  → return possibleActions, Action.Priority.Progress
    // when false → run HighestPriorityActions selection loop
    ok = expect(ShouldSkipPrioritySelection(true, false), "empty list → Progress short-circuit") && ok;
    ok = expect(ShouldSkipPrioritySelection(false, true), "onZoneIn CS ID numbers → Progress short-circuit") && ok;
    ok = expect(!ShouldSkipPrioritySelection(false, false), "ActionCandidate list → selection loop") && ok;

    // Host-style maxPriority pin when skip is true.
    ok = expect(SkipPriorityMaxPriority == 1000, "skip maxPriority Progress pin") && ok;

    // Explicit dual-wire: free == empty||firstIsNumber for host-style poles.
    for (const bool empty : { false, true })
    {
        for (const bool firstIsNumber : { false, true })
        {
            const bool got = ShouldSkipPrioritySelection(empty, firstIsNumber);
            ok             = expect(got == (empty || firstIsNumber), "host inject dual-wire identity") && ok;
            ok             = expect(got == inlineShouldSkipPrioritySelection(empty, firstIsNumber),
                        "host inject free == inline") &&
                 ok;
            // When skip is true, host returns Progress; pin agrees.
            if (got)
            {
                ok = expect(SkipPriorityMaxPriority == 1000, "skip path maxPriority pin") && ok;
            }
        }
    }

    // Residual 1094 poles still hold under dual-wire.
    ok = expect(ShouldSkipPrioritySelection(true, false), "residual empty") && ok;
    ok = expect(ShouldSkipPrioritySelection(false, true), "residual firstIsNumber") && ok;
    ok = expect(ShouldSkipPrioritySelection(true, true), "residual both") && ok;
    ok = expect(!ShouldSkipPrioritySelection(false, false), "residual normal") && ok;

    return ok;
}
