#pragma once

namespace rangedadditionaleffecthelpers
{

inline auto ShouldShortCircuitAdditionalEffects(const bool targetIsDead) -> bool
{
    return targetIsDead;
}

inline auto HasConflictingAdditionalEffectConfig(const bool hasGlobal, const bool hasItemScript) -> bool
{
    return hasGlobal && hasItemScript;
}

inline auto ShouldTryGlobalAdditionalEffect(const bool hasGlobal) -> bool
{
    return hasGlobal;
}

inline auto ShouldTryItemScriptAdditionalEffect(const bool hasItemScript, const bool globalHandled) -> bool
{
    return hasItemScript && !globalHandled;
}

enum class RangedAdditionalEffectItem
{
    None,
    Ammo,
    RangedWeapon,
};

inline auto ResolveRangedAdditionalEffectItem(const bool hasAmmo, const bool hasRangedWeapon) -> RangedAdditionalEffectItem
{
    if (hasAmmo)
    {
        return RangedAdditionalEffectItem::Ammo;
    }
    if (hasRangedWeapon)
    {
        return RangedAdditionalEffectItem::RangedWeapon;
    }
    return RangedAdditionalEffectItem::None;
}

} // namespace rangedadditionaleffecthelpers
