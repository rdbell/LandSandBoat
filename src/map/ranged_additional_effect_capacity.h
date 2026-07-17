#pragma once

// Pure ranged additional-effect dispatch capacity helpers extracted so native
// tests can pin policy without battle entities, Lua item scripts, or action
// packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1392: full ranged AE dispatch policy residual pure port
//           (ShouldShortCircuitAdditionalEffects, conflict, global/item try,
//           ResolveRangedAdditionalEffectItem)
//   - 2950: ShouldShortCircuitAdditionalEffects residual dual-wire expand
//           (targetIsDead identity)
//   - 3203: ShouldShortCircuitAdditionalEffects dedicated dual-wire
//           (targetIsDead; residual expand 2950 / pure 1392)
//
// Production host: CBattleEntity::OnRangedAttack checkAddEffect lambda
// (battle_entity.cpp) injects PTarget->GetHPP() == 0 into
// ShouldShortCircuitAdditionalEffects before global / item-script AE attempts.
// Go dual-wire: ranger.ShouldShortCircuitAdditionalEffects
// (internal/ranger/ranged_additional_effect.go).

namespace rangedadditionaleffecthelpers
{

// --- Slice 3203: ShouldShortCircuitAdditionalEffects dedicated pure dual-wire ---
// Residual dual-wire expand: slice 2950.
// Residual pure port: slice 1392 (OnRangedAttack checkAddEffect AE policy suite).
// Production host: CBattleEntity::OnRangedAttack checkAddEffect injects
// PTarget->GetHPP() == 0 into ShouldShortCircuitAdditionalEffects before
// global / item-script AE attempts. When true, checkAddEffect returns early
// (treated as handled / skipped).
// Go dual-wire: ranger.ShouldShortCircuitAdditionalEffects
// (internal/ranger/ranged_additional_effect.go).
// Residual dual-wire suite: 2950 / test_ranger_short_circuit_ae_2950.
// Dedicated dual-wire suite: 3203 /
// test_ranger_short_circuit_additional_effects_3203.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite. Sibling residual AE gates remain on this header
// (conflict / global try / item-script try / ammo-weapon resolve).
// Siblings left alone: camouflage retain suite (3174 etc.).

// ShouldShortCircuitAdditionalEffects reports whether a dead target prevents
// any additional-effect dispatch.
//
// Formula (slice 3203 dual-wire; residual 2950 / pure 1392):
//   targetIsDead → short-circuit (true)  // identity
//
// targetIsDead — host-evaluated PTarget->GetHPP() == 0
// true  → checkAddEffect returns early (skip AE procs; treated as handled)
// false → continue global / item-script additional-effect dispatch
//
// Dual-wire of Go ranger.ShouldShortCircuitAdditionalEffects.
// Call site: OnRangedAttack checkAddEffect before GetScaledItemModifier AE flags.
// Residual pure port: slice 1392. Residual dual-wire expand: slice 2950.
// Dedicated dual-wire suite: slice 3203.
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
