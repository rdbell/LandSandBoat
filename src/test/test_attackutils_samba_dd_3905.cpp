#include "test_attackutils_samba_dd_3905.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils Samba double-damage 3905 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineShouldApplySambaDoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == attackutilshelpers::AttackTypeSamba && rateProcs;
}
} // namespace

auto runAttackutilsSambaDD3905SelfTests() -> bool
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
    using attackutilshelpers::ShouldApplySambaDoubleDamage;

    bool ok = true;
    const struct { uint8 attackType; bool rateProcs; bool want; const char* label; } cases[] = {
        { AttackTypeSamba, true, true, "samba + rate applies" },
        { AttackTypeSamba, false, false, "samba + failed rate skips" },
        { AttackTypeNormal, true, false, "normal is not samba" },
        { AttackTypeDouble, true, false, "double uses its own arm" },
        { AttackTypeTriple, true, false, "triple uses its own arm" },
        { AttackTypeZanshin, true, false, "zanshin uses its own arm" },
        { AttackTypeRanged, true, false, "ordinary ranged is not samba" },
        { AttackTypeRapidShot, true, false, "rapid shot uses its own arm" },
    };
    for (const auto& c : cases)
    {
        const auto got = ShouldApplySambaDoubleDamage(c.attackType, c.rateProcs);
        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineShouldApplySambaDoubleDamage(c.attackType, c.rateProcs),
                    "free helper equals inlined host formula") && ok;
    }
    ok = expect(ShouldApplySambaDoubleDamage(AttackTypeSamba, RollRatePercent(25, 24)),
                "successful host rate roll applies") && ok;
    ok = expect(!ShouldApplySambaDoubleDamage(AttackTypeSamba, RollRatePercent(25, 25)),
                "boundary host rate roll skips") && ok;
    ok = expect(ApplyTypeDoubleDamage(50, 2) == 100, "successful samba doubles original damage") && ok;
    return ok;
}
