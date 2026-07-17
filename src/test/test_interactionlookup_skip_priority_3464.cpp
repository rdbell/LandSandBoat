#include "test_interactionlookup_skip_priority_3464.h"

#include "map/interaction_lookup_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "interactionlookup ShouldSkipPrioritySelection 3464 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline interaction_lookup.lua getHighestPriorityActions early-return gate
// for dual-wire cross-check (dedicated 3464):
//   empty || firstIsNumber
auto inlineShouldSkipPrioritySelection(const bool empty, const bool firstIsNumber) -> bool
{
    return empty || firstIsNumber;
}

// Compact dual-wire pin matching Go pinShouldSkipPrioritySelection3464 /
// C++ capacity (formula unchanged from 1094 / 2953 / 3167 / 3246 / 3332 / 3410):
//   empty || firstIsNumber
auto pinShouldSkipPrioritySelection(const bool empty, const bool firstIsNumber) -> bool
{
    return empty || firstIsNumber;
}

} // namespace

// Pure dual-wire expansion for interactionlookuphelpers::ShouldSkipPrioritySelection
// (getHighestPriorityActions empty/number → Progress;
// OmegaXI internal/interactionlookup; dedicated slice 3464; residual expand
// 2953 / prior 3410 / 3332 / 3246 / 3167 / pure 1094).
//
// Coverage:
//   - free == inline == pin positive form (empty || firstIsNumber)
//   - residual poles + dense 2^2
//   - residual 2953 / 1094 / prior 3167 / prior 3246 / prior 3332 / prior 3410 pins still hold
//   - SkipPriorityMaxPriority Progress pin
auto runInteractionlookupSkipPriority3464SelfTests() -> bool
{
    using interactionlookuphelpers::ShouldSkipPrioritySelection;
    using interactionlookuphelpers::SkipPriorityMaxPriority;

    bool ok = true;

    // Residual 1094 / 2953 / prior 3167 / prior 3246 / prior 3332 / prior 3410 pins still hold under dedicated dual-wire.
    ok = expect(SkipPriorityMaxPriority == 1000, "residual SkipPriorityMaxPriority pin == 1000") && ok;
    ok = expect(ShouldSkipPrioritySelection(true, false), "residual: empty must skip") && ok;
    ok = expect(ShouldSkipPrioritySelection(false, true), "residual: firstIsNumber must skip") && ok;
    ok = expect(ShouldSkipPrioritySelection(true, true), "residual: both must skip") && ok;
    ok = expect(!ShouldSkipPrioritySelection(false, false), "residual: neither must not skip") && ok;

    // --- Core poles: free == inline == pin positive form ---
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

        // Residual 2953 / 1094 / prior 3167 / prior 3246 / prior 3332 / prior 3410 re-pins.
        { true, false, true, "residual 2953 empty → skip" },
        { false, true, true, "residual 2953 firstIsNumber → skip" },
        { true, true, true, "residual 2953 both → skip" },
        { false, false, false, "residual 2953 normal → no skip" },
        { true, false, true, "residual 1094 empty → skip" },
        { false, true, true, "residual 1094 firstIsNumber → skip" },
        { true, true, true, "residual 1094 both → skip" },
        { false, false, false, "residual 1094 normal → no skip" },
        { true, false, true, "prior 3167 empty → skip" },
        { false, true, true, "prior 3167 firstIsNumber → skip" },
        { true, true, true, "prior 3167 both → skip" },
        { false, false, false, "prior 3167 normal → no skip" },
        { true, false, true, "prior 3246 empty → skip" },
        { false, true, true, "prior 3246 firstIsNumber → skip" },
        { true, true, true, "prior 3246 both → skip" },
        { false, false, false, "prior 3246 normal → no skip" },
        { true, false, true, "prior 3332 empty → skip" },
        { false, true, true, "prior 3332 firstIsNumber → skip" },
        { true, true, true, "prior 3332 both → skip" },
        { false, false, false, "prior 3332 normal → no skip" },
        { true, false, true, "prior 3410 empty → skip" },
        { false, true, true, "prior 3410 firstIsNumber → skip" },
        { true, true, true, "prior 3410 both → skip" },
        { false, false, false, "prior 3410 normal → no skip" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipPrioritySelection(c.empty, c.firstIsNumber);
        const bool inlineF = inlineShouldSkipPrioritySelection(c.empty, c.firstIsNumber);
        const bool pinGot  = pinShouldSkipPrioritySelection(c.empty, c.firstIsNumber);
        const bool wantPin = c.empty || c.firstIsNumber;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==positive pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldSkipPrioritySelection(true, false) == pinShouldSkipPrioritySelection(true, false),
                "free==pin empty") &&
         ok;
    ok = expect(ShouldSkipPrioritySelection(false, true) == pinShouldSkipPrioritySelection(false, true),
                "free==pin firstIsNumber") &&
         ok;
    ok = expect(ShouldSkipPrioritySelection(true, true) == pinShouldSkipPrioritySelection(true, true),
                "free==pin both") &&
         ok;
    ok = expect(ShouldSkipPrioritySelection(false, false) == pinShouldSkipPrioritySelection(false, false),
                "free==pin neither") &&
         ok;

    // Dense compose: full 2^2 boolean space — free == inline == pin.
    for (const bool empty : { false, true })
    {
        for (const bool firstIsNumber : { false, true })
        {
            const bool got     = ShouldSkipPrioritySelection(empty, firstIsNumber);
            const bool inlineF = inlineShouldSkipPrioritySelection(empty, firstIsNumber);
            const bool pinGot  = pinShouldSkipPrioritySelection(empty, firstIsNumber);
            const bool want    = empty || firstIsNumber;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF, "compose free==inline") && ok;
            ok                 = expect(got == pinGot, "compose free==pin") && ok;
            // When skip is true, host returns Progress; pin agrees.
            if (got)
            {
                ok = expect(SkipPriorityMaxPriority == 1000, "skip path maxPriority pin") && ok;
            }
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
            ok = expect(got == pinShouldSkipPrioritySelection(empty, firstIsNumber),
                        "host inject free == pin") &&
                 ok;
        }
    }

    // Residual 1094 / 2953 / prior 3167 / prior 3246 / prior 3332 / prior 3410 poles still hold under dual-wire.
    ok = expect(ShouldSkipPrioritySelection(true, false), "residual empty") && ok;
    ok = expect(ShouldSkipPrioritySelection(false, true), "residual firstIsNumber") && ok;
    ok = expect(ShouldSkipPrioritySelection(true, true), "residual both") && ok;
    ok = expect(!ShouldSkipPrioritySelection(false, false), "residual normal") && ok;

    return ok;
}
