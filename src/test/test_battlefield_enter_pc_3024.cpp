#include "test_battlefield_enter_pc_3024.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldEnterPC 3024 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity TYPE_PC enter-branch formula for dual-wire cross-check
// (slice 3024):
//   enter
auto inlineShouldEnterPC(const bool enter) -> bool
{
    return enter;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldEnterPC
// (enter identity; slice 3024).
auto runBattlefieldEnterPC3024SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 truth-table pins.
    ok = expect(ShouldEnterPC(true), "residual: enter=true takes enter path") && ok;
    ok = expect(!ShouldEnterPC(false), "residual: enter=false declines enter path") && ok;

    const struct
    {
        bool        enter;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "enter=true → enter path" },
        { false, false, "enter=false → not enter (register may own)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEnterPC(c.enter);
        const bool inlineF = inlineShouldEnterPC(c.enter);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldEnterPC dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.enter, "free == identity of enter") && ok;
    }

    // Pin composition: identity of enter only.
    ok = expect(ShouldEnterPC(true), "enter must accept enter path") && ok;
    ok = expect(!ShouldEnterPC(false), "!enter must not take enter path") && ok;

    // Host path: CBattlefield::InsertEntity TYPE_PC under capacity, first
    // branch before ShouldRegisterPC else-if.
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

        ok = expect(got == c.wantEnter, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.enter, "host compose free == identity of enter") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool enter : { false, true })
    {
        const bool got = ShouldEnterPC(enter);
        ok             = expect(got == enter, "compose free == pin formula") && ok;
        ok             = expect(got == inlineShouldEnterPC(enter), "compose free == inline") && ok;
    }

    // Mutual exclusion with register (3014): enter=true never registers;
    // enter=false may register when !alreadyRegistered.
    ok = expect(!(ShouldEnterPC(true) && ShouldRegisterPC(true, false)),
                "enter path and register path must not both accept for enter=true") &&
         ok;
    ok = expect(ShouldEnterPC(true) && !ShouldRegisterPC(true, false),
                "enter=true: ShouldEnterPC only") &&
         ok;
    ok = expect(!ShouldEnterPC(false) && ShouldRegisterPC(false, false),
                "enter=false && !registered: ShouldRegisterPC only") &&
         ok;

    // Residual independence (1361 / 2994 / 3002 / 2930 / 3014): enter is
    // distinct from null-insert, already-in, PC under-capacity, and register.
    ok = expect(ShouldEnterPC(true), "enter must accept via ShouldEnterPC when enter=true") && ok;
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
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
