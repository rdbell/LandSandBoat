#include "test_ranged_use_sange_message_3040.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldUseSangeDisplayMessage 3040 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack Sange display-message gate for dual-wire
// cross-check (slice 3040):
//   return isSange
auto inlineShouldUseSangeDisplayMessage(const bool isSange) -> bool
{
    return isSange;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldUseSangeDisplayMessage
// (OnRangedAttack Barrage/Sange display-message selection; slice 3040).
auto runRangedUseSangeMessage3040SelfTests() -> bool
{
    using rangedammohelpers::ShouldUseSangeDisplayMessage;

    bool ok = true;

    const struct
    {
        bool        isSange;
        bool        want;
        const char* label;
    } cases[] = {
        // Identity poles.
        { true, true, "sange → UsesSangeTakesDamage" },
        { false, false, "no sange → UsesBarrageTakesDamage" },

        // Residual 1390 / production inject pin.
        { true, true, "residual sange msg pin" },
        { false, false, "residual no sange msg pin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUseSangeDisplayMessage(c.isSange);
        const bool inlineF = inlineShouldUseSangeDisplayMessage(c.isSange);
        const bool wantPin = c.isSange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldUseSangeDisplayMessage dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldUseSangeDisplayMessage == pin formula isSange") && ok;
    }

    // Pin composition: identity poles.
    ok = expect(ShouldUseSangeDisplayMessage(true), "sange path must select Sange display message") && ok;
    ok = expect(!ShouldUseSangeDisplayMessage(false), "no-sange path must select Barrage display message") && ok;

    // Dense compose: isSange poles only (identity).
    for (const bool isSange : { false, true })
    {
        const bool got = ShouldUseSangeDisplayMessage(isSange);
        ok             = expect(got == isSange, "compose free == pin formula isSange") && ok;
        ok             = expect(got == inlineShouldUseSangeDisplayMessage(isSange), "compose free == inline") && ok;
    }

    // Host-style compose poles: OnRangedAttack injects
    // isSange = StatusEffectContainer->HasStatusEffect(Sange, 0) captured
    // at shot start. Outer gate is (isBarrage || isSange) && hitOccured;
    // free function only selects Sange vs Barrage message id.
    const struct
    {
        bool        isSange;
        bool        wantSange;
        const char* label;
    } composeCases[] = {
        { true, true, "sange active: UsesSangeTakesDamage" },
        { false, false, "no sange (Barrage path): UsesBarrageTakesDamage" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldUseSangeDisplayMessage(c.isSange);
        ok             = expect(got == c.wantSange, c.label) && ok;
        ok             = expect(got == c.isSange, "compose free == pin formula isSange") && ok;
        ok             = expect(got == inlineShouldUseSangeDisplayMessage(c.isSange), "compose free == inline") && ok;
    }

    // Production inject pin: OnRangedAttack (~3443) calls
    // ShouldUseSangeDisplayMessage(isSange) for message override.
    ok = expect(ShouldUseSangeDisplayMessage(true), "production inject true must select Sange message") && ok;
    ok = expect(!ShouldUseSangeDisplayMessage(false), "production inject false must select Barrage message") && ok;

    return ok;
}
