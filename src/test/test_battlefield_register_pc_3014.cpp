#include "test_battlefield_register_pc_3014.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldRegisterPC 3014 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity TYPE_PC register-branch formula for dual-wire cross-check
// (slice 3014):
//   !enter && !alreadyRegistered
auto inlineShouldRegisterPC(const bool enter, const bool alreadyRegistered) -> bool
{
    return !enter && !alreadyRegistered;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldRegisterPC
// (!enter && !alreadyRegistered; slice 3014).
auto runBattlefieldRegisterPC3014SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldEnterPC;
    using battlefieldhelpers::ShouldRegisterPC;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 truth-table pins.
    ok = expect(ShouldRegisterPC(false, false), "residual: !enter && !registered registers") && ok;
    ok = expect(!ShouldRegisterPC(false, true), "residual: already registered does not re-register") && ok;
    ok = expect(!ShouldRegisterPC(true, false), "residual: enter path does not register") && ok;

    const struct
    {
        bool        enter;
        bool        alreadyRegistered;
        bool        want;
        const char* label;
    } cases[] = {
        { false, false, true, "!enter && !registered → register" },
        { false, true, false, "!enter && already registered → skip" },
        { true, false, false, "enter && !registered → enter path owns" },
        { true, true, false, "enter && already registered → neither register" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRegisterPC(c.enter, c.alreadyRegistered);
        const bool inlineF = inlineShouldRegisterPC(c.enter, c.alreadyRegistered);
        const bool wantPin = !c.enter && !c.alreadyRegistered;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRegisterPC dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "free == pin formula !enter && !alreadyRegistered") && ok;
    }

    // Pin composition: conjunction of negations only.
    ok = expect(ShouldRegisterPC(false, false), "!enter && !alreadyRegistered must register") && ok;
    ok = expect(!ShouldRegisterPC(false, true), "alreadyRegistered must block register") && ok;
    ok = expect(!ShouldRegisterPC(true, false), "enter must block register (enter path)") && ok;
    ok = expect(!ShouldRegisterPC(true, true), "enter + alreadyRegistered must not register") && ok;

    // Host path: CBattlefield::InsertEntity TYPE_PC under capacity, else-if
    // after ShouldEnterPC(enter) declined.
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

        ok = expect(got == c.wantRegister, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == (!c.enter && !c.alreadyRegistered),
                    "host compose free == !enter && !alreadyRegistered") &&
             ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool enter : { false, true })
    {
        for (const bool alreadyRegistered : { false, true })
        {
            const bool got  = ShouldRegisterPC(enter, alreadyRegistered);
            const bool want = !enter && !alreadyRegistered;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRegisterPC(enter, alreadyRegistered),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Residual independence (1361 / 2994 / 3002 / 2930): register is distinct
    // from null-insert, already-in, PC under-capacity, and enter path.
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
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
