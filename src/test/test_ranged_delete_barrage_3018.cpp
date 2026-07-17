#include "test_ranged_delete_barrage_3018.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldDeleteBarrageStatus 3018 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack Barrage delete gate for dual-wire
// cross-check (slice 3018):
//   return isBarrage
auto inlineShouldDeleteBarrageStatus(const bool isBarrage) -> bool
{
    return isBarrage;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldDeleteBarrageStatus
// (OnRangedAttack Barrage status-delete gate; slice 3018).
auto runRangedDeleteBarrage3018SelfTests() -> bool
{
    using rangedammohelpers::ShouldDeleteBarrageStatus;

    bool ok = true;

    const struct
    {
        bool        isBarrage;
        bool        want;
        const char* label;
    } cases[] = {
        // Identity poles.
        { true, true, "barrage → delete Barrage" },
        { false, false, "no barrage → skip delete" },

        // Residual 1390 / production inject pin.
        { true, true, "residual del status pin" },
        { false, false, "residual no barrage pin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDeleteBarrageStatus(c.isBarrage);
        const bool inlineF = inlineShouldDeleteBarrageStatus(c.isBarrage);
        const bool wantPin = c.isBarrage;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDeleteBarrageStatus dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDeleteBarrageStatus == pin formula isBarrage") && ok;
    }

    // Pin composition: identity poles.
    ok = expect(ShouldDeleteBarrageStatus(true), "barrage path must delete Barrage") && ok;
    ok = expect(!ShouldDeleteBarrageStatus(false), "no-barrage path must not delete Barrage") && ok;

    // Dense compose: isBarrage poles only (identity).
    for (const bool isBarrage : { false, true })
    {
        const bool got = ShouldDeleteBarrageStatus(isBarrage);
        ok             = expect(got == isBarrage, "compose free == pin formula isBarrage") && ok;
        ok             = expect(got == inlineShouldDeleteBarrageStatus(isBarrage), "compose free == inline") && ok;
    }

    // Host-style compose poles: OnRangedAttack injects
    // isBarrage = StatusEffectContainer->HasStatusEffect(Barrage, 0) captured
    // at shot start. On true, host DelStatusEffectSilent(Barrage).
    const struct
    {
        bool        isBarrage;
        bool        wantDelete;
        const char* label;
    } composeCases[] = {
        { true, true, "barrage active: delete Barrage" },
        { false, false, "no barrage: skip Barrage delete" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldDeleteBarrageStatus(c.isBarrage);
        ok             = expect(got == c.wantDelete, c.label) && ok;
        ok             = expect(got == c.isBarrage, "compose free == pin formula isBarrage") && ok;
        ok             = expect(got == inlineShouldDeleteBarrageStatus(c.isBarrage), "compose free == inline") && ok;
    }

    // Production inject pin: OnRangedAttack (~3449) calls
    // ShouldDeleteBarrageStatus(isBarrage) after firing.
    ok = expect(ShouldDeleteBarrageStatus(true), "production inject true must delete") && ok;
    ok = expect(!ShouldDeleteBarrageStatus(false), "production inject false must skip delete") && ok;

    return ok;
}
