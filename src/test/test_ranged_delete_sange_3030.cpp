#include "test_ranged_delete_sange_3030.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldDeleteSangeStatus 3030 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack Sange delete gate for dual-wire
// cross-check (slice 3030):
//   return isSange
auto inlineShouldDeleteSangeStatus(const bool isSange) -> bool
{
    return isSange;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldDeleteSangeStatus
// (OnRangedAttack Sange status-delete gate; slice 3030).
auto runRangedDeleteSange3030SelfTests() -> bool
{
    using rangedammohelpers::ShouldDeleteSangeStatus;

    bool ok = true;

    const struct
    {
        bool        isSange;
        bool        want;
        const char* label;
    } cases[] = {
        // Identity poles.
        { true, true, "sange → delete Sange" },
        { false, false, "no sange → skip delete" },

        // Residual 1390 / production inject pin.
        { true, true, "residual del status pin" },
        { false, false, "residual no sange pin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDeleteSangeStatus(c.isSange);
        const bool inlineF = inlineShouldDeleteSangeStatus(c.isSange);
        const bool wantPin = c.isSange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDeleteSangeStatus dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDeleteSangeStatus == pin formula isSange") && ok;
    }

    // Pin composition: identity poles.
    ok = expect(ShouldDeleteSangeStatus(true), "sange path must delete Sange") && ok;
    ok = expect(!ShouldDeleteSangeStatus(false), "no-sange path must not delete Sange") && ok;

    // Dense compose: isSange poles only (identity).
    for (const bool isSange : { false, true })
    {
        const bool got = ShouldDeleteSangeStatus(isSange);
        ok             = expect(got == isSange, "compose free == pin formula isSange") && ok;
        ok             = expect(got == inlineShouldDeleteSangeStatus(isSange), "compose free == inline") && ok;
    }

    // Host-style compose poles: OnRangedAttack injects
    // isSange = StatusEffectContainer->HasStatusEffect(Sange, 0) captured
    // at shot start. On true, host DelStatusEffectSilent(Sange).
    const struct
    {
        bool        isSange;
        bool        wantDelete;
        const char* label;
    } composeCases[] = {
        { true, true, "sange active: delete Sange" },
        { false, false, "no sange: skip Sange delete" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldDeleteSangeStatus(c.isSange);
        ok             = expect(got == c.wantDelete, c.label) && ok;
        ok             = expect(got == c.isSange, "compose free == pin formula isSange") && ok;
        ok             = expect(got == inlineShouldDeleteSangeStatus(c.isSange), "compose free == inline") && ok;
    }

    // Production inject pin: OnRangedAttack (~3454) calls
    // ShouldDeleteSangeStatus(isSange) after firing.
    ok = expect(ShouldDeleteSangeStatus(true), "production inject true must delete") && ok;
    ok = expect(!ShouldDeleteSangeStatus(false), "production inject false must skip delete") && ok;

    return ok;
}
