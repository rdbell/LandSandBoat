#include "test_ranged_outcome_1393.h"

#include "map/ranged_outcome_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged outcome 1393 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runRangedOutcome1393SelfTests() -> bool
{
    using namespace rangedoutcomehelpers;
    bool ok = true;

    ok = expect(NormalizeRangedDamage(352, -25) == MessageParam{ MsgRangedAttackAbsorbs, 25 }, "damage absorb") && ok;
    ok = expect(NormalizeRangedDamage(353, 25) == MessageParam{ 353, 25 }, "damage identity") && ok;
    ok = expect(NormalizeRangedDamage(352, 0) == MessageParam{ 352, 0 }, "zero damage") && ok;

    ok = expect(NormalizeRangedAdditionalEffect(MsgAddEffectDamage, -12) == MessageParam{ MsgAddEffectRecoversHP, 12 }, "additional recovery") && ok;
    ok = expect(NormalizeRangedAdditionalEffect(MsgAddEffectDamage, 12) == MessageParam{ MsgAddEffectDamage, 12 }, "additional damage") && ok;
    ok = expect(NormalizeRangedAdditionalEffect(164, -12) == MessageParam{ 164, -12 }, "other message identity") && ok;

    ok = expect(ShouldUseShadowAbsorbOutcome(false, 1), "shadow fallback") && ok;
    ok = expect(!ShouldUseShadowAbsorbOutcome(true, 1), "hit wins") && ok;
    ok = expect(!ShouldUseShadowAbsorbOutcome(false, 0), "no shadows") && ok;

    ok = expect(MsgShadowAbsorb == 31 && MsgRangedAttackAbsorbs == 382, "outcome pins") && ok;
    ok = expect(MsgAddEffectDamage == 163 && MsgAddEffectRecoversHP == 384, "additional pins") && ok;
    return ok;
}
