#include "test_battlefield_enter_pc_3599.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldEnterPC 3599 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity TYPE_PC enter-branch formula for dual-wire cross-check
// (dedicated 3599 expand residual 3024; prior dedicated 3554 / 3497 / 3431 / 3381):
//   enter
auto inlineShouldEnterPC(const bool enter) -> bool
{
    return enter;
}

// Compact dual-wire pin matching Go pinShouldEnterPC3599 /
// C++ capacity enter-identity form (formula unchanged from 1361 / 3024 / 3381 /
// 3431 / 3497 / 3554):
//   enter
auto pinShouldEnterPC(const bool enter) -> bool
{
    return enter;
}

// Prior dedicated 3554 inline/pin for independence cross-check (pin3554 chain).
auto inlineShouldEnterPC3554(const bool enter) -> bool
{
    return enter;
}

auto pinShouldEnterPC3554(const bool enter) -> bool
{
    return enter;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldEnterPC
// (enter identity; OmegaXI internal/battlefield; dedicated slice
// 3599 expand residual 3024 / prior dedicated 3554 / 3497 / 3431 / 3381 / pure 1361).
//
// Coverage:
//   - free == inline == pin residual pins
//   - free == pin3554 (prior dedicated pin chain)
//   - poles: enter-true / enter-false
//   - residual 1361 / 3024 pins still hold
//   - prior dedicated 3381 / 3431 / 3497 / 3554 poles still hold
//   - dense bool free == inline == pin residual pins
//   - host-style inject + residual independence (3198 / 3216 / 3302 / 3365 left alone)
// Formula unchanged — not registered in CMake/main.
auto runBattlefieldEnterPC3599SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 / prior dual-wire 3024 / prior dedicated 3381 / 3431 / 3497 /
    // 3554 pins still hold.
    ok = expect(ShouldEnterPC(true), "residual: enter=true takes enter path") && ok;
    ok = expect(!ShouldEnterPC(false), "residual: enter=false declines enter path") && ok;

    // --- Core poles: free == inline == pin residual pins (== pin3554) ---
    const struct
    {
        bool        enter;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: enter=true → enter path (ApplyLevelRestrictions + EnteredPlayers).
        { true, true, "enter=true → enter path" },

        // Pole: enter=false → not enter (register may own via ShouldRegisterPC).
        { false, false, "enter=false → not enter (register may own)" },

        // Residual 1361 / 3024 / prior dedicated 3381 / 3431 / 3497 / 3554 re-pins.
        { true, true, "residual 1361 enter=true takes enter path" },
        { false, false, "residual 1361 enter=false declines enter path" },
        { true, true, "residual 3024 enter=true takes enter path" },
        { false, false, "residual 3024 enter=false declines enter path" },
        { true, true, "prior 3381 enter=true takes enter path" },
        { false, false, "prior 3381 enter=false declines enter path" },
        { true, true, "prior 3431 enter=true takes enter path" },
        { false, false, "prior 3431 enter=false declines enter path" },
        { true, true, "prior 3497 enter=true takes enter path" },
        { false, false, "prior 3497 enter=false declines enter path" },
        { true, true, "prior 3554 enter=true takes enter path" },
        { false, false, "prior 3554 enter=false declines enter path" },
    };

    for (const auto& c : cases)
    {
        const bool got      = ShouldEnterPC(c.enter);
        const bool inlineF  = inlineShouldEnterPC(c.enter);
        const bool pinGot   = pinShouldEnterPC(c.enter);
        const bool pin3554  = pinShouldEnterPC3554(c.enter);
        // Identity pin composition (explicit).
        const bool wantPin = c.enter;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == pin3554, "dual-wire free==pin3554") && ok;
        ok = expect(got == wantPin, "dual-wire free==identity pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldEnterPC(true) == pinShouldEnterPC(true),
                "free==pin enter=true") &&
         ok;
    ok = expect(ShouldEnterPC(false) == pinShouldEnterPC(false),
                "free==pin enter=false") &&
         ok;

    // free == pin3554 pin chain (constraint free==inline==pin==pin3554).
    ok = expect(ShouldEnterPC(true) == pinShouldEnterPC3554(true),
                "free==pin3554 enter=true") &&
         ok;
    ok = expect(ShouldEnterPC(false) == pinShouldEnterPC3554(false),
                "free==pin3554 enter=false") &&
         ok;
    ok = expect(pinShouldEnterPC(true) == pinShouldEnterPC3554(true),
                "pin3599==pin3554 enter=true") &&
         ok;
    ok = expect(pinShouldEnterPC(false) == pinShouldEnterPC3554(false),
                "pin3599==pin3554 enter=false") &&
         ok;

    // Prior dedicated 3554 independence: free still matches prior inline/pin.
    ok = expect(ShouldEnterPC(true) == inlineShouldEnterPC3554(true) &&
                    ShouldEnterPC(true) == pinShouldEnterPC3554(true),
                "prior 3554 independence enter=true") &&
         ok;
    ok = expect(ShouldEnterPC(false) == inlineShouldEnterPC3554(false) &&
                    ShouldEnterPC(false) == pinShouldEnterPC3554(false),
                "prior 3554 independence enter=false") &&
         ok;

    // Dense compose: free == inline == pin residual pins for both bools.
    for (const bool enter : { false, true })
    {
        const bool got     = ShouldEnterPC(enter);
        const bool inlineF = inlineShouldEnterPC(enter);
        const bool pinGot  = pinShouldEnterPC(enter);
        const bool pin3554 = pinShouldEnterPC3554(enter);
        const bool want    = enter;
        ok                 = expect(got == inlineF, "compose free==inline") && ok;
        ok                 = expect(got == pinGot, "compose free==pin") && ok;
        ok                 = expect(got == pin3554, "compose free==pin3554") && ok;
        ok                 = expect(got == want, "compose free==identity pin form") && ok;
    }

    // Host-style inject poles: CBattlefield::InsertEntity TYPE_PC under
    // capacity, first branch before ShouldRegisterPC else-if.
    const struct
    {
        bool        enter;
        bool        wantEnter;
        const char* label;
    } hostCases[] = {
        { true, true, "enter: enter=true → ApplyLevelRestrictions + EnteredPlayers" },
        { false, false, "register path: enter=false declines ShouldEnterPC first" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldEnterPC(c.enter);
        const bool inlineF = inlineShouldEnterPC(c.enter);
        const bool pinGot  = pinShouldEnterPC(c.enter);
        const bool pin3554 = pinShouldEnterPC3554(c.enter);

        ok = expect(got == c.wantEnter, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
        ok = expect(got == pin3554, "host compose free==pin3554") && ok;
    }

    // Production path semantics:
    // enter=true → enter; enter=false → no enter.
    ok = expect(ShouldEnterPC(true) && pinShouldEnterPC(true) && pinShouldEnterPC3554(true),
                "enter=true → enter path") &&
         ok;
    ok = expect(!ShouldEnterPC(false) && !pinShouldEnterPC(false) && !pinShouldEnterPC3554(false),
                "enter=false → no enter path") &&
         ok;

    // Residual independence (1361 / 2994 / 3198 / 3002 / 3216 / 2930 / 3271 /
    // 3302 / 3014 / 3365 / 3024 / 3381 / 3431 / 3497 / 3554): enter is distinct
    // from null-insert, already-in, PC under-capacity, and register. Siblings
    // left alone beyond pins — do not thrash register_pc.
    ok = expect(ShouldEnterPC(true),
                "enter must accept via ShouldEnterPC when enter=true") &&
         ok;
    ok = expect(ShouldRejectNullInsert(true) && !ShouldRejectNullInsert(false),
                "null-insert residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRejectAlreadyInBattlefield(true) && !ShouldRejectAlreadyInBattlefield(false),
                "already-in residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRegisterPC(false, false) && !ShouldRegisterPC(false, true),
                "register residual still holds under dual-wire") &&
         ok;
    // Mutual exclusion with register at free-function level.
    ok = expect(!(ShouldEnterPC(true) && ShouldRegisterPC(true, false)),
                "enter=true must never also register") &&
         ok;
    ok = expect(ShouldEnterPC(true) && !ShouldRegisterPC(true, false),
                "enter=true: ShouldEnterPC only") &&
         ok;
    ok = expect(!ShouldEnterPC(false) && ShouldRegisterPC(false, false),
                "enter=false && !registered: ShouldRegisterPC only") &&
         ok;
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
