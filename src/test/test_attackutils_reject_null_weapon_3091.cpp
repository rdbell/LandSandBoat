#include "test_attackutils_reject_null_weapon_3091.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils ShouldRejectNullWeapon 3091 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CheckForDamageMultiplier null-weapon formula for dual-wire cross-check
// (slice 3091):
//   weaponNull
auto inlineShouldRejectNullWeapon(const bool weaponNull) -> bool
{
    return weaponNull;
}

} // namespace

// Pure dual-wire expansion for attackutilshelpers::ShouldRejectNullWeapon
// (weaponNull identity; slice 3091).
auto runAttackutilsRejectNullWeapon3091SelfTests() -> bool
{
    using attackutilshelpers::AttackTypeNormal;
    using attackutilshelpers::AttackTypeRanged;
    using attackutilshelpers::ShouldRejectNullWeapon;
    using attackutilshelpers::ShouldUseMainHandRemOcc;
    using attackutilshelpers::ShouldUseRangedRemOcc;
    using attackutilshelpers::SlotMain;

    bool ok = true;

    // Residual 1380 truth-table pins.
    ok = expect(ShouldRejectNullWeapon(true), "residual: null weapon rejects") && ok;
    ok = expect(!ShouldRejectNullWeapon(false), "residual: non-null weapon proceeds") && ok;

    const struct
    {
        bool        weaponNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null weapon rejects" },
        { false, false, "non-null weapon proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullWeapon(c.weaponNull);
        const bool inlineF = inlineShouldRejectNullWeapon(c.weaponNull);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullWeapon dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.weaponNull, "free == weaponNull (identity)") && ok;
    }

    // Pin composition: identity of weaponNull only.
    ok = expect(ShouldRejectNullWeapon(true), "weaponNull must reject") && ok;
    ok = expect(!ShouldRejectNullWeapon(false), "!weaponNull must not reject") && ok;

    // Host path: attackutils::CheckForDamageMultiplier before other gates.
    const struct
    {
        bool        weaponNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PWeapon == nullptr → return original damage before other gates" },
        { false, false, "PWeapon non-null → proceed to REM occ / allowProc ladder" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullWeapon(c.weaponNull);
        const bool inlineF = inlineShouldRejectNullWeapon(c.weaponNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.weaponNull, "host compose free == weaponNull (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool weaponNull : { false, true })
    {
        const bool got  = ShouldRejectNullWeapon(weaponNull);
        const bool want = weaponNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullWeapon(weaponNull),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (1380): null reject is distinct from rem occ
    // selection gates on the same CheckForDamageMultiplier path.
    ok = expect(ShouldRejectNullWeapon(true),
                "null weapon must reject via ShouldRejectNullWeapon") &&
         ok;
    ok = expect(ShouldUseRangedRemOcc(AttackTypeRanged),
                "ranged rem residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldUseRangedRemOcc(AttackTypeNormal),
                "normal attack must not use ranged rem") &&
         ok;
    ok = expect(ShouldUseMainHandRemOcc(AttackTypeNormal, SlotMain),
                "main-hand rem residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldUseMainHandRemOcc(AttackTypeNormal, 1),
                "non-main slot must not use main-hand rem") &&
         ok;
    ok = expect(!ShouldRejectNullWeapon(false),
                "non-null must proceed even if later rem / ladder fails") &&
         ok;

    // Explicit dual-wire poles across dense bool space.
    for (const bool weaponNull : { false, true })
    {
        const bool got = ShouldRejectNullWeapon(weaponNull);
        ok             = expect(got == weaponNull, "host inject dual-wire pin") && ok;
        ok             = expect(got == inlineShouldRejectNullWeapon(weaponNull),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldUseRangedRemOcc(AttackTypeRanged),
                    "ranged rem residual flipped under null-weapon compose") &&
             ok;
        ok = expect(ShouldUseMainHandRemOcc(AttackTypeNormal, SlotMain),
                    "main rem residual flipped under null-weapon compose") &&
             ok;
    }

    return ok;
}
