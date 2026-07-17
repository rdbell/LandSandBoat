#include "test_battlefield_register_pc_3365.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldRegisterPC 3365 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity TYPE_PC register-branch formula for dual-wire cross-check
// (dedicated 3365 expand residual 3014):
//   !enter && !alreadyRegistered
// Positive form only — no De Morgan rewrite to !(enter || alreadyRegistered).
auto inlineShouldRegisterPC(const bool enter, const bool alreadyRegistered) -> bool
{
    return !enter && !alreadyRegistered;
}

// Compact dual-wire pin matching Go pinShouldRegisterPC3365 /
// C++ capacity conjunction-of-negations form (formula unchanged from 1361 /
// 3014):
//   !enter && !alreadyRegistered
// Positive form only — no De Morgan rewrite (QF1001).
auto pinShouldRegisterPC(const bool enter, const bool alreadyRegistered) -> bool
{
    return !enter && !alreadyRegistered;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldRegisterPC
// (!enter && !alreadyRegistered; OmegaXI internal/battlefield; dedicated slice
// 3365 expand residual 3014 / pure 1361).
//
// Coverage:
//   - free == inline == pin residual pins (positive form; no De Morgan QF1001)
//   - poles: register / already-reg / enter-owns / neither
//   - residual 1361 / 3014 pins still hold
//   - dense 2^2 free == inline == pin residual pins
//   - host-style inject + residual independence (3198 / 3216 / 3302 / 3024 left alone)
auto runBattlefieldRegisterPC3365SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 / prior dual-wire 3014 pins still hold under dedicated expand.
    ok = expect(ShouldRegisterPC(false, false), "residual: !enter && !registered registers") && ok;
    ok = expect(!ShouldRegisterPC(false, true), "residual: already registered does not re-register") && ok;
    ok = expect(!ShouldRegisterPC(true, false), "residual: enter path does not register") && ok;
    ok = expect(!ShouldRegisterPC(true, true), "residual: enter + alreadyRegistered must not register") && ok;

    // --- Core poles: free == inline == pin residual pins (positive form) ---
    const struct
    {
        bool        enter;
        bool        alreadyRegistered;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: !enter && !registered → register (emplace RegisteredPlayers).
        { false, false, true, "!enter && !registered → register" },

        // Pole: already registered → no re-register.
        { false, true, false, "!enter && already registered → skip" },

        // Pole: enter path owns enter=true (ShouldEnterPC first).
        { true, false, false, "enter && !registered → enter path owns" },

        // Pole: enter path first; no register even if already registered.
        { true, true, false, "enter && already registered → neither register" },

        // Residual 1361 / 3014 re-pins.
        { false, false, true, "residual 1361 !enter && !registered registers" },
        { false, true, false, "residual 1361 already registered skips" },
        { true, false, false, "residual 1361 enter path owns" },
        { false, false, true, "residual 3014 !enter && !registered registers" },
        { false, true, false, "residual 3014 already registered skips" },
        { true, false, false, "residual 3014 enter path owns" },
        { true, true, false, "residual 3014 enter + alreadyRegistered neither" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRegisterPC(c.enter, c.alreadyRegistered);
        const bool inlineF = inlineShouldRegisterPC(c.enter, c.alreadyRegistered);
        const bool pinGot  = pinShouldRegisterPC(c.enter, c.alreadyRegistered);
        // Positive pin composition (explicit; no De Morgan QF1001).
        const bool wantPin = !c.enter && !c.alreadyRegistered;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==positive pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRegisterPC(false, false) == pinShouldRegisterPC(false, false),
                "free==pin !enter && !registered") &&
         ok;
    ok = expect(ShouldRegisterPC(false, true) == pinShouldRegisterPC(false, true),
                "free==pin already registered") &&
         ok;
    ok = expect(ShouldRegisterPC(true, false) == pinShouldRegisterPC(true, false),
                "free==pin enter path") &&
         ok;
    ok = expect(ShouldRegisterPC(true, true) == pinShouldRegisterPC(true, true),
                "free==pin enter + alreadyRegistered") &&
         ok;

    // Dense compose: full 2^2 boolean space free == inline == pin residual pins.
    for (const bool enter : { false, true })
    {
        for (const bool alreadyRegistered : { false, true })
        {
            const bool got     = ShouldRegisterPC(enter, alreadyRegistered);
            const bool inlineF = inlineShouldRegisterPC(enter, alreadyRegistered);
            const bool pinGot  = pinShouldRegisterPC(enter, alreadyRegistered);
            const bool want    = !enter && !alreadyRegistered;
            ok                 = expect(got == inlineF, "compose free==inline") && ok;
            ok                 = expect(got == pinGot, "compose free==pin") && ok;
            ok                 = expect(got == want, "compose free==positive pin form") && ok;
        }
    }

    // Host-style inject poles: CBattlefield::InsertEntity TYPE_PC under
    // capacity, else-if after ShouldEnterPC(enter) declined.
    const struct
    {
        bool        enter;
        bool        alreadyRegistered;
        bool        wantRegister;
        const char* label;
    } hostCases[] = {
        { false, false, true, "register: !enter && !IsRegistered → emplace RegisteredPlayers" },
        { false, true, false, "already registered: !enter && IsRegistered → no re-register" },
        { true, false, false, "enter path: enter=true takes ShouldEnterPC branch first" },
        { true, true, false, "enter path wins even if already registered" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRegisterPC(c.enter, c.alreadyRegistered);
        const bool inlineF = inlineShouldRegisterPC(c.enter, c.alreadyRegistered);
        const bool pinGot  = pinShouldRegisterPC(c.enter, c.alreadyRegistered);

        ok = expect(got == c.wantRegister, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production path semantics:
    // !enter && !registered → register; already reg / enter → no register.
    ok = expect(ShouldRegisterPC(false, false) && pinShouldRegisterPC(false, false),
                "!enter && !registered → register path") &&
         ok;
    ok = expect(!ShouldRegisterPC(false, true) && !pinShouldRegisterPC(false, true),
                "already registered → no re-register path") &&
         ok;
    ok = expect(!ShouldRegisterPC(true, false) && !pinShouldRegisterPC(true, false),
                "enter=true → enter path owns (no register)") &&
         ok;

    // Residual independence (1361 / 2994 / 3198 / 3002 / 3216 / 2930 / 3271 /
    // 3302 / 3014 / 3024): register is distinct from null-insert, already-in,
    // PC under-capacity, and enter path. Siblings left alone beyond pins.
    ok = expect(ShouldRegisterPC(false, false),
                "register must accept via ShouldRegisterPC when !enter && !registered") &&
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
    ok = expect(ShouldEnterPC(true) && !ShouldEnterPC(false),
                "enter path residual still holds under dual-wire") &&
         ok;
    // Mutual exclusion with enter at free-function level.
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
