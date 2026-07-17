#pragma once

// Pure ranged additional-effect dispatch capacity helpers extracted so native
// tests can pin policy without battle entities, Lua item scripts, or action
// packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1392: full ranged AE dispatch policy residual pure port
//           (ShouldShortCircuitAdditionalEffects, conflict, global/item try,
//           ResolveRangedAdditionalEffectItem)
//   - 2950: ShouldShortCircuitAdditionalEffects (targetIsDead) dual-wire
//           expansion
//
// Production host: CBattleEntity::OnRangedAttack checkAddEffect lambda
// (battle_entity.cpp) injects PTarget->GetHPP() == 0 into
// ShouldShortCircuitAdditionalEffects before global / item-script AE attempts.
// Go dual-wire: ranger.ShouldShortCircuitAdditionalEffects
// (internal/ranger/ranged_additional_effect.go).

namespace rangedadditionaleffecthelpers
{

// ShouldShortCircuitAdditionalEffects reports whether a dead target prevents
// any additional-effect dispatch.
//
// Formula (slice 2950 dual-wire):
//   targetIsDead → short-circuit (true)
//
// targetIsDead — host-evaluated PTarget->GetHPP() == 0
// true  → checkAddEffect returns early (skip AE procs; treated as handled)
// false → continue global / item-script additional-effect dispatch
//
// Dual-wire of Go ranger.ShouldShortCircuitAdditionalEffects.
// Call site: OnRangedAttack checkAddEffect before GetScaledItemModifier AE flags.
// Residual pure port: slice 1392.
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
