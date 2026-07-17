#include "test_attackutils_zanshin_dd_3120.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils ShouldApplyZanshinDoubleDamage 3120 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CheckForDamageMultiplier ZANSHIN type double-damage formula for
// dual-wire cross-check (slice 3120):
//   attackType == AttackTypeZanshin && rateProcs
auto inlineShouldApplyZanshinDoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == attackutilshelpers::AttackTypeZanshin && rateProcs;
}

} // namespace

// Pure dual-wire expansion for attackutilshelpers::ShouldApplyZanshinDoubleDamage
// (attackType == ZANSHIN && rateProcs; slice 3120).
auto runAttackutilsZanshinDD3120SelfTests() -> bool
{
    using attackutilshelpers::ApplyTypeDoubleDamage;
    using attackutilshelpers::AttackTypeDouble;
    using attackutilshelpers::AttackTypeNormal;
    using attackutilshelpers::AttackTypeRapidShot;
    using attackutilshelpers::AttackTypeRanged;
    using attackutilshelpers::AttackTypeSamba;
    using attackutilshelpers::AttackTypeTriple;
    using attackutilshelpers::AttackTypeZanshin;
    using attackutilshelpers::RollRatePercent;
    using attackutilshelpers::ShouldApplyDADoubleDamage;
    using attackutilshelpers::ShouldApplyRapidShotDoubleDamage;
    using attackutilshelpers::ShouldApplySambaDoubleDamage;
    using attackutilshelpers::ShouldApplyTATripleDamage;
    using attackutilshelpers::ShouldApplyZanshinDoubleDamage;
    using attackutilshelpers::ShouldRejectNullWeapon;

    bool ok = true;

    // Residual 1380 truth-table pins.
    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true),
                "residual: zanshin + rate procs") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, false),
                "residual: zanshin + !rate fails") &&
         ok;

    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        want;
        const char* label;
    } cases[] = {
        { AttackTypeZanshin, true, true, "zanshin + rate → apply *2" },
        { AttackTypeZanshin, false, false, "zanshin + !rate → skip" },
        { AttackTypeNormal, true, false, "normal + rate → skip" },
        { AttackTypeNormal, false, false, "normal + !rate → skip" },
        { AttackTypeDouble, true, false, "double + rate → skip (DA arm)" },
        { AttackTypeTriple, true, false, "triple + rate → skip (TA arm)" },
        { AttackTypeRanged, true, false, "ranged + rate → skip" },
        { AttackTypeRapidShot, true, false, "rapid + rate → skip (rapid arm)" },
        { AttackTypeSamba, true, false, "samba + rate → skip (samba arm)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyZanshinDoubleDamage(c.attackType, c.rateProcs);
        const bool inlineF = inlineShouldApplyZanshinDoubleDamage(c.attackType, c.rateProcs);
        const bool pin     = (c.attackType == AttackTypeZanshin) && c.rateProcs;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyZanshinDoubleDamage dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "free == pin formula attackType==ZANSHIN && rateProcs") && ok;
    }

    // Pin composition: ZANSHIN && rateProcs only.
    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true),
                "zanshin + rate must apply double damage") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, false),
                "zanshin without rate must not apply") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeNormal, true),
                "non-zanshin must not apply even with rate") &&
         ok;

    // Host path: attackutils::CheckForDamageMultiplier switch ZANSHIN.
    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        wantApply;
        const char* label;
    } hostCases[] = {
        { AttackTypeZanshin, true, true, "ZANSHIN + rate roll → ApplyTypeDoubleDamage(*2)" },
        { AttackTypeZanshin, false, false, "ZANSHIN + failed rate → leave damage" },
        { AttackTypeNormal, true, false, "non-ZANSHIN switch arm not taken" },
        { AttackTypeDouble, true, false, "DOUBLE uses DA arm, not zanshin" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldApplyZanshinDoubleDamage(c.attackType, c.rateProcs);
        const bool inlineF = inlineShouldApplyZanshinDoubleDamage(c.attackType, c.rateProcs);

        ok = expect(got == c.wantApply, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == ((c.attackType == AttackTypeZanshin) && c.rateProcs),
                    "host compose free == pin formula") &&
             ok;
    }

    // Dense compose over capacity attack-type pins × rateProcs.
    const uint8 types[] = {
        AttackTypeNormal,
        AttackTypeDouble,
        AttackTypeTriple,
        AttackTypeZanshin,
        AttackTypeRanged,
        AttackTypeRapidShot,
        AttackTypeSamba,
    };
    for (const uint8 at : types)
    {
        for (const bool rateProcs : { false, true })
        {
            const bool got  = ShouldApplyZanshinDoubleDamage(at, rateProcs);
            const bool want = (at == AttackTypeZanshin) && rateProcs;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplyZanshinDoubleDamage(at, rateProcs),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Residual independence (1380): zanshin type rate is distinct from sibling
    // type rates and null-weapon gate on CheckForDamageMultiplier.
    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true),
                "zanshin double must apply via ShouldApplyZanshinDoubleDamage") &&
         ok;
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true),
                "TA triple residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                "DA double residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldApplyRapidShotDoubleDamage(AttackTypeRapidShot, true),
                "rapid residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldApplySambaDoubleDamage(AttackTypeSamba, true),
                "samba residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeTriple, true),
                "triple must not satisfy zanshin double gate") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeDouble, true),
                "double must not satisfy zanshin double gate") &&
         ok;
    ok = expect(ShouldRejectNullWeapon(true) && !ShouldRejectNullWeapon(false),
                "null-weapon residual still holds under zanshin dual-wire") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, false),
                "failed rate must not apply even if ApplyTypeDoubleDamage would *2") &&
         ok;
    ok = expect(ApplyTypeDoubleDamage(50, 2) == 100,
                "ApplyTypeDoubleDamage residual still holds under dual-wire") &&
         ok;

    // Explicit dual-wire poles across dense type × rate space.
    for (const uint8 at : { AttackTypeNormal, AttackTypeZanshin, AttackTypeDouble, AttackTypeTriple })
    {
        for (const bool rateProcs : { false, true })
        {
            const bool got  = ShouldApplyZanshinDoubleDamage(at, rateProcs);
            const bool want = (at == AttackTypeZanshin) && rateProcs;
            ok              = expect(got == want, "host inject dual-wire pin") && ok;
            ok              = expect(got == inlineShouldApplyZanshinDoubleDamage(at, rateProcs),
                        "host inject free == inline") &&
                 ok;
            ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true),
                        "TA triple residual flipped under zanshin compose") &&
                 ok;
            ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                        "DA double residual flipped under zanshin compose") &&
                 ok;
        }
    }

    // Host rate inject via RollRatePercent (ZANSHIN_DOUBLE_DAMAGE mod / roll).
    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, RollRatePercent(25, 10)),
                "host inject: ZANSHIN + RollRatePercent success → apply") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, RollRatePercent(25, 25)),
                "host inject: ZANSHIN + RollRatePercent fail → skip") &&
         ok;
    ok = expect(!ShouldApplyZanshinDoubleDamage(AttackTypeNormal, RollRatePercent(25, 10)),
                "host inject: non-ZANSHIN + successful rate still skips") &&
         ok;

    return ok;
}
