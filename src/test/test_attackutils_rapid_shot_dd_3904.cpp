#include "test_attackutils_rapid_shot_dd_3904.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils Rapid Shot double-damage 3904 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineShouldApplyRapidShotDoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == attackutilshelpers::AttackTypeRapidShot && rateProcs;
}

} // namespace

auto runAttackutilsRapidShotDD3904SelfTests() -> bool
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
    using attackutilshelpers::ShouldApplyRapidShotDoubleDamage;

    bool ok = true;
    const struct
    {
        uint8       attackType;
        bool        rateProcs;
        bool        want;
        const char* label;
    } cases[] = {
        { AttackTypeRapidShot, true, true, "rapid shot + rate applies" },
        { AttackTypeRapidShot, false, false, "rapid shot + failed rate skips" },
        { AttackTypeNormal, true, false, "normal is not rapid shot" },
        { AttackTypeDouble, true, false, "double uses its own arm" },
        { AttackTypeTriple, true, false, "triple uses its own arm" },
        { AttackTypeZanshin, true, false, "zanshin uses its own arm" },
        { AttackTypeRanged, true, false, "ordinary ranged is not rapid shot" },
        { AttackTypeSamba, true, false, "samba uses its own arm" },
    };

    for (const auto& c : cases)
    {
        const auto got = ShouldApplyRapidShotDoubleDamage(c.attackType, c.rateProcs);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == inlineShouldApplyRapidShotDoubleDamage(c.attackType, c.rateProcs),
                                "free helper equals inlined host formula") &&
             ok;
    }

    ok = expect(ShouldApplyRapidShotDoubleDamage(AttackTypeRapidShot, RollRatePercent(25, 24)),
                "successful host rate roll applies double damage") &&
         ok;
    ok = expect(!ShouldApplyRapidShotDoubleDamage(AttackTypeRapidShot, RollRatePercent(25, 25)),
                "boundary host rate roll skips") &&
         ok;
    ok = expect(ApplyTypeDoubleDamage(50, 2) == 100, "successful rapid shot doubles original damage") && ok;

    return ok;
}
