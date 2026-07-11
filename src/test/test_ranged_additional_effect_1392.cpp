#include "test_ranged_additional_effect_1392.h"

#include "map/ranged_additional_effect_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged additional effect 1392 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runRangedAdditionalEffect1392SelfTests() -> bool
{
    using namespace rangedadditionaleffecthelpers;
    bool ok = true;

    ok = expect(ShouldShortCircuitAdditionalEffects(true) && !ShouldShortCircuitAdditionalEffects(false), "dead short circuit") && ok;
    ok = expect(HasConflictingAdditionalEffectConfig(true, true), "conflict") && ok;
    ok = expect(!HasConflictingAdditionalEffectConfig(true, false) && !HasConflictingAdditionalEffectConfig(false, true), "non-conflict") && ok;
    ok = expect(ShouldTryGlobalAdditionalEffect(true) && !ShouldTryGlobalAdditionalEffect(false), "global gate") && ok;
    ok = expect(ShouldTryItemScriptAdditionalEffect(true, false), "script fallback") && ok;
    ok = expect(!ShouldTryItemScriptAdditionalEffect(true, true) && !ShouldTryItemScriptAdditionalEffect(false, false), "script suppression") && ok;
    ok = expect(ResolveRangedAdditionalEffectItem(true, true) == RangedAdditionalEffectItem::Ammo, "ammo precedence") && ok;
    ok = expect(ResolveRangedAdditionalEffectItem(true, false) == RangedAdditionalEffectItem::Ammo, "ammo") && ok;
    ok = expect(ResolveRangedAdditionalEffectItem(false, true) == RangedAdditionalEffectItem::RangedWeapon, "weapon fallback") && ok;
    ok = expect(ResolveRangedAdditionalEffectItem(false, false) == RangedAdditionalEffectItem::None, "no item") && ok;

    return ok;
}
