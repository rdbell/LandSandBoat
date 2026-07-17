#include "test_attackutils_ta_triple_3138.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils ShouldApplyTATripleDamage 3138 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CheckForDamageMultiplier TRIPLE type triple-damage formula for
// dual-wire cross-check (slice 3138):
//   attackType == AttackTypeTriple && rateProcs
auto inlineShouldApplyTATripleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == attackutilshelpers::AttackTypeTriple && rateProcs;
}

} // namespace

// Pure dual-wire expansion for attackutilshelpers::ShouldApplyTATripleDamage
// (attackType == TRIPLE && rateProcs; slice 3138).
auto runAttackutilsTATriple3138SelfTests() -> bool
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
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true),
                "residual: triple + rate procs") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeTriple, false),
                "residual: triple + !rate fails") &&
         ok;

    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        want;
        const char* label;
    } cases[] = {
        { AttackTypeTriple, true, true, "triple + rate → apply *3" },
        { AttackTypeTriple, false, false, "triple + !rate → skip" },
        { AttackTypeNormal, true, false, "normal + rate → skip" },
        { AttackTypeNormal, false, false, "normal + !rate → skip" },
        { AttackTypeDouble, true, false, "double + rate → skip (DA arm)" },
        { AttackTypeZanshin, true, false, "zanshin + rate → skip (zanshin arm)" },
        { AttackTypeRanged, true, false, "ranged + rate → skip" },
        { AttackTypeRapidShot, true, false, "rapid + rate → skip (rapid arm)" },
        { AttackTypeSamba, true, false, "samba + rate → skip (samba arm)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyTATripleDamage(c.attackType, c.rateProcs);
        const bool inlineF = inlineShouldApplyTATripleDamage(c.attackType, c.rateProcs);
        const bool pin     = (c.attackType == AttackTypeTriple) && c.rateProcs;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyTATripleDamage dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "free == pin formula attackType==TRIPLE && rateProcs") && ok;
    }

    // Pin composition: TRIPLE && rateProcs only.
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true),
                "triple + rate must apply triple damage") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeTriple, false),
                "triple without rate must not apply") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeNormal, true),
                "non-triple must not apply even with rate") &&
         ok;

    // Host path: attackutils::CheckForDamageMultiplier switch TRIPLE.
    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        wantApply;
        const char* label;
    } hostCases[] = {
        { AttackTypeTriple, true, true, "TRIPLE + rate roll → ApplyTypeDoubleDamage(*3)" },
        { AttackTypeTriple, false, false, "TRIPLE + failed rate → leave damage" },
        { AttackTypeNormal, true, false, "non-TRIPLE switch arm not taken" },
        { AttackTypeDouble, true, false, "DOUBLE uses DA arm, not TA" },
        { AttackTypeZanshin, true, false, "ZANSHIN uses zanshin arm, not TA" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldApplyTATripleDamage(c.attackType, c.rateProcs);
        const bool inlineF = inlineShouldApplyTATripleDamage(c.attackType, c.rateProcs);

        ok = expect(got == c.wantApply, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == ((c.attackType == AttackTypeTriple) && c.rateProcs),
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
            const bool got  = ShouldApplyTATripleDamage(at, rateProcs);
            const bool want = (at == AttackTypeTriple) && rateProcs;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplyTATripleDamage(at, rateProcs),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Residual independence (1380): TA type rate is distinct from sibling
    // type rates and null-weapon gate on CheckForDamageMultiplier.
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true),
                "TA triple must apply via ShouldApplyTATripleDamage") &&
         ok;
    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true),
                "zanshin double residual (3120) still holds under dual-wire") &&
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
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeZanshin, true),
                "zanshin must not satisfy TA triple gate") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeDouble, true),
                "double must not satisfy TA triple gate") &&
         ok;
    ok = expect(ShouldRejectNullWeapon(true) && !ShouldRejectNullWeapon(false),
                "null-weapon residual still holds under TA dual-wire") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeTriple, false),
                "failed rate must not apply even if ApplyTypeDoubleDamage would *3") &&
         ok;
    ok = expect(ApplyTypeDoubleDamage(50, 3) == 150,
                "ApplyTypeDoubleDamage residual still holds under dual-wire") &&
         ok;

    // Explicit dual-wire poles across dense type × rate space.
    for (const uint8 at : { AttackTypeNormal, AttackTypeTriple, AttackTypeDouble, AttackTypeZanshin })
    {
        for (const bool rateProcs : { false, true })
        {
            const bool got  = ShouldApplyTATripleDamage(at, rateProcs);
            const bool want = (at == AttackTypeTriple) && rateProcs;
            ok              = expect(got == want, "host inject dual-wire pin") && ok;
            ok              = expect(got == inlineShouldApplyTATripleDamage(at, rateProcs),
                        "host inject free == inline") &&
                 ok;
            ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true),
                        "zanshin residual flipped under TA compose") &&
                 ok;
            ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                        "DA double residual flipped under TA compose") &&
                 ok;
        }
    }

    // Host rate inject via RollRatePercent (TA_TRIPLE_DMG_RATE mod / roll).
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, RollRatePercent(25, 10)),
                "host inject: TRIPLE + RollRatePercent success → apply") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeTriple, RollRatePercent(25, 25)),
                "host inject: TRIPLE + RollRatePercent fail → skip") &&
         ok;
    ok = expect(!ShouldApplyTATripleDamage(AttackTypeNormal, RollRatePercent(25, 10)),
                "host inject: non-TRIPLE + successful rate still skips") &&
         ok;

    return ok;
}
