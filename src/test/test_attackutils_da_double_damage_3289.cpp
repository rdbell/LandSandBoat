#include "test_attackutils_da_double_damage_3289.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils ShouldApplyDADoubleDamage 3289 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CheckForDamageMultiplier DOUBLE type double-damage formula for
// dual-wire cross-check (dedicated 3289):
//   attackType == AttackTypeDouble && rateProcs
// Direct return (QF1001/SA1008 safe).
auto inlineShouldApplyDADoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == attackutilshelpers::AttackTypeDouble && rateProcs;
}

// Compact dual-wire pin matching Go pinShouldApplyDADoubleDamage3289 / C++
// capacity:
//   attackType == AttackTypeDouble && rateProcs
// Direct return (QF1001/SA1008 safe).
auto pinShouldApplyDADoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == attackutilshelpers::AttackTypeDouble && rateProcs;
}

} // namespace

// Pure dual-wire expansion for attackutilshelpers::ShouldApplyDADoubleDamage
// (DOUBLE type double-damage gate; OmegaXI internal/attackutils;
// dedicated slice 3289 expand residual 3120 / prior 3249).
//
// Coverage:
//   - free == inline == pin == (attackType == DOUBLE && rateProcs)
//   - residual poles (1380 / prior 3249) still hold
//   - dense type×rate free == inline == pin
// QF1001/SA1008 safe: free / inline / pin are direct returns.
auto runAttackutilsDADoubleDamage3289SelfTests() -> bool
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

    // Residual 1380 / prior 3249 pins still hold under dual-wire.
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                "residual: double + rate procs (1380/3249)") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeDouble, false),
                "residual: double + !rate fails (1380/3249)") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeNormal, true),
                "residual: normal + rate skips (1380/3249)") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeZanshin, true),
                "residual: zanshin + rate skips DA arm (1380/3249)") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeTriple, true),
                "residual: triple + rate skips DA arm (1380/3249)") &&
         ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 1380+prior 3249 surface.
        { AttackTypeDouble, true, true, "double + rate → apply *2" },
        { AttackTypeDouble, false, false, "double + !rate → skip" },
        { AttackTypeNormal, true, false, "normal + rate → skip" },
        { AttackTypeNormal, false, false, "normal + !rate → skip" },
        { AttackTypeTriple, true, false, "triple + rate → skip (TA arm)" },
        { AttackTypeZanshin, true, false, "zanshin + rate → skip (zanshin arm)" },
        { AttackTypeRanged, true, false, "ranged + rate → skip" },
        { AttackTypeRapidShot, true, false, "rapid + rate → skip (rapid arm)" },
        { AttackTypeSamba, true, false, "samba + rate → skip (samba arm)" },

        // Residual 1380 / prior 3249 pins (labeled).
        { AttackTypeDouble, true, true, "residual double apply" },
        { AttackTypeDouble, false, false, "residual double fail" },
        { AttackTypeNormal, true, false, "residual normal skip" },
        { AttackTypeZanshin, true, false, "residual zanshin skip" },
        { AttackTypeTriple, true, false, "residual triple skip" },
        { AttackTypeRapidShot, true, false, "residual rapid skip" },
        { AttackTypeSamba, true, false, "residual samba skip" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyDADoubleDamage(c.attackType, c.rateProcs);
        const bool inlineF = inlineShouldApplyDADoubleDamage(c.attackType, c.rateProcs);
        const bool pin     = pinShouldApplyDADoubleDamage(c.attackType, c.rateProcs);
        const bool wantPin = (c.attackType == AttackTypeDouble) && c.rateProcs;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula attackType==DOUBLE && rateProcs") && ok;
    }

    // Direct return pin: only DOUBLE && rateProcs admits (QF1001/SA1008).
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                "double + rate must apply double damage") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeDouble, false),
                "double without rate must not apply") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeNormal, true),
                "non-double must not apply even with rate") &&
         ok;
    ok = expect(inlineShouldApplyDADoubleDamage(AttackTypeDouble, true) &&
                    pinShouldApplyDADoubleDamage(AttackTypeDouble, true),
                "inline/pin direct return must apply DOUBLE+rate") &&
         ok;
    ok = expect(!inlineShouldApplyDADoubleDamage(AttackTypeDouble, false) &&
                    !pinShouldApplyDADoubleDamage(AttackTypeDouble, false),
                "inline/pin direct return must skip DOUBLE+!rate") &&
         ok;
    ok = expect(!inlineShouldApplyDADoubleDamage(AttackTypeNormal, true) &&
                    !pinShouldApplyDADoubleDamage(AttackTypeNormal, true),
                "inline/pin direct return must skip non-DOUBLE+rate") &&
         ok;

    // Dense compose over capacity attack-type pins × rateProcs free == inline == pin.
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
            const bool got     = ShouldApplyDADoubleDamage(at, rateProcs);
            const bool inlineF = inlineShouldApplyDADoubleDamage(at, rateProcs);
            const bool pin     = pinShouldApplyDADoubleDamage(at, rateProcs);
            const bool want    = (at == AttackTypeDouble) && rateProcs;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Host-style inject poles: free == inline == pin for CheckForDamageMultiplier.
    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        wantApply;
        const char* label;
    } hostPoles[] = {
        { AttackTypeDouble, true, true, "DOUBLE + rate roll → ApplyTypeDoubleDamage(*2)" },
        { AttackTypeDouble, false, false, "DOUBLE + failed rate → leave damage" },
        { AttackTypeNormal, true, false, "non-DOUBLE switch arm not taken" },
        { AttackTypeTriple, true, false, "TRIPLE uses TA arm, not DA" },
        { AttackTypeZanshin, true, false, "ZANSHIN uses zanshin arm, not DA" },
        { AttackTypeRapidShot, true, false, "RAPID_SHOT uses rapid arm, not DA" },
        { AttackTypeSamba, true, false, "SAMBA uses samba arm, not DA" },
    };

    for (const auto& pole : hostPoles)
    {
        const bool got     = ShouldApplyDADoubleDamage(pole.attackType, pole.rateProcs);
        const bool inlineF = inlineShouldApplyDADoubleDamage(pole.attackType, pole.rateProcs);
        const bool pin     = pinShouldApplyDADoubleDamage(pole.attackType, pole.rateProcs);
        const bool inject  = (pole.attackType == AttackTypeDouble) && pole.rateProcs;

        ok = expect(got == pole.wantApply && got == inject, pole.label) && ok;
        ok = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production CheckForDamageMultiplier path semantics still hold.
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                "CheckForDamageMultiplier DOUBLE+rate → apply *2 path") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeDouble, false),
                "CheckForDamageMultiplier DOUBLE+!rate → leave path") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeNormal, true),
                "CheckForDamageMultiplier non-DOUBLE → leave path") &&
         ok;

    // Residual independence: DA type rate is distinct from sibling type rates
    // and null-weapon gate. Residual sibling dual-wire 3120 left alone.
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, true),
                "DA double must apply via ShouldApplyDADoubleDamage") &&
         ok;
    ok = expect(ShouldApplyZanshinDoubleDamage(AttackTypeZanshin, true),
                "zanshin double residual (3120) still holds under dual-wire") &&
         ok;
    ok = expect(ShouldApplyTATripleDamage(AttackTypeTriple, true),
                "TA triple residual (3138) still holds under dual-wire") &&
         ok;
    ok = expect(ShouldApplyRapidShotDoubleDamage(AttackTypeRapidShot, true),
                "rapid residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldApplySambaDoubleDamage(AttackTypeSamba, true),
                "samba residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeZanshin, true),
                "zanshin must not satisfy DA double gate") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeTriple, true),
                "triple must not satisfy DA double gate") &&
         ok;
    ok = expect(ShouldRejectNullWeapon(true) && !ShouldRejectNullWeapon(false),
                "null-weapon residual still holds under DA dual-wire") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeDouble, false),
                "failed rate must not apply even if ApplyTypeDoubleDamage would *2") &&
         ok;
    ok = expect(ApplyTypeDoubleDamage(50, 2) == 100,
                "ApplyTypeDoubleDamage residual still holds under dual-wire") &&
         ok;

    // Host rate inject via RollRatePercent (DA_DOUBLE_DMG_RATE mod / roll).
    ok = expect(ShouldApplyDADoubleDamage(AttackTypeDouble, RollRatePercent(25, 10)),
                "host inject: DOUBLE + RollRatePercent success → apply") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeDouble, RollRatePercent(25, 25)),
                "host inject: DOUBLE + RollRatePercent fail → skip") &&
         ok;
    ok = expect(!ShouldApplyDADoubleDamage(AttackTypeNormal, RollRatePercent(25, 10)),
                "host inject: non-DOUBLE + successful rate still skips") &&
         ok;

    // free == inline == pin under RollRatePercent host inject.
    const struct
    {
        int16 rateMod;
        int   roll0to99;
    } rolls[] = {
        { 25, 10 },
        { 25, 25 },
        { 0, 0 },
        { 100, 99 },
    };
    for (const uint8 at : { AttackTypeDouble, AttackTypeNormal, AttackTypeZanshin })
    {
        for (const auto& roll : rolls)
        {
            const bool rateProcs = RollRatePercent(roll.rateMod, roll.roll0to99);
            const bool got       = ShouldApplyDADoubleDamage(at, rateProcs);
            const bool inlineF   = inlineShouldApplyDADoubleDamage(at, rateProcs);
            const bool pin       = pinShouldApplyDADoubleDamage(at, rateProcs);
            ok                   = expect(got == inlineF && got == pin,
                        "rate inject free == inline == pin") &&
                 ok;
        }
    }

    return ok;
}
