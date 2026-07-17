#pragma once

#include "common/cbasetypes.h"

// Pure post-swing effect-gate helpers (slice 1399 residual).
// Dual-wire residual expansions:
//   - 3046: ShouldRunEnspell residual dual-wire suite
//   - 3390: ShouldRunEnspell dedicated dual-wire (run_enspell.go)
//
// Dual-wire index:
//   - 3046: ShouldRunEnspell residual dual-wire suite
//   - 3390: ShouldRunEnspell = targetHPP > 0
//
// Production host: CBattleEntity::OnAttack (battle_entity.cpp ~3844) injects
// PTarget->GetHPP() into ShouldRunEnspell after ShouldRunEnspellAndSpikes
// admits hit/guard/block (non-Daken). When false, host skips
// battleutils::HandleEnspell while still calling HandleSpikesDamage under the
// outer gate.
// Go dual-wire: attack.ShouldRunEnspell (internal/attack/run_enspell.go).
// Residual dual-wire suite: 3046 (test_attack_run_enspell_3046).
// Dedicated dual-wire suite: 3390 (test_attack_run_enspell_3390).
//
// Sibling residual gates in this header (not re-expanded under 3390):
//   - ShouldRunEnspellAndSpikes (resolution + Daken outer gate)
//   - ShouldRunParrySpikes (parry + lazy Battuta)

namespace attackpostswinghelpers
{

constexpr uint8  ResolutionMiss  = 1;
constexpr uint8  ResolutionParry = 3;
constexpr uint8  AttackTypeDaken = 9;
constexpr uint16 MsgShadowAbsorb = 31;

inline auto NormalizeMissParam(const uint8 resolution, const uint16 message, const int32 param) -> int32
{
    if (resolution == ResolutionMiss && message != MsgShadowAbsorb)
    {
        return 0;
    }
    return param;
}

inline auto ShouldRunEnspellAndSpikes(const uint8 resolution, const uint8 attackType) -> bool
{
    return resolution != ResolutionMiss && resolution != ResolutionParry && attackType != AttackTypeDaken;
}

// ---------------------------------------------------------------------------
// Slice 3390 — OnAttack live-target enspell gate (dedicated expand residual 3046)
// ---------------------------------------------------------------------------

// ShouldRunEnspell suppresses enspell handling after the target dies.
//
// Formula (slice 3390 dedicated dual-wire; residual expand 3046 / pure 1399 —
// formula unchanged):
//   targetHPP > 0
//   // suppresses enspell after target dies
//
// targetHPP — host-evaluated PTarget->GetHPP()
// true  → host may call battleutils::HandleEnspell
// false → host skips HandleEnspell (ordinary spikes still run under outer gate)
//
// Dual-wire of Go attack.ShouldRunEnspell.
// Call site: CBattleEntity::OnAttack host inject (targetHPP) ~3844:
//   if (ShouldRunEnspellAndSpikes(resolution, attackType))
//   {
//       if (ShouldRunEnspell(PTarget->GetHPP()))
//       {
//           battleutils::HandleEnspell(...);
//       }
//       battleutils::HandleSpikesDamage(...);
//   }
// Prior pure port: slice 1399 (melee post-swing effect gates residual).
// Residual dual-wire suite: 3046 / test_attack_run_enspell_3046.
// Dedicated dual-wire suite: 3390 / test_attack_run_enspell_3390.
// Sibling residual gates (not re-expanded under 3390):
// ShouldRunEnspellAndSpikes / ShouldRunParrySpikes.
// Coverage: test_attack_run_enspell_3390 (not in CMake/main).
// Edges: 0 (dead/skip), 1 / 50 / 100 / 255 (live/run).
inline auto ShouldRunEnspell(const uint8 targetHPP) -> bool
{
    return targetHPP > 0;
}

template <typename HasBattuta>
inline auto ShouldRunParrySpikes(const uint8 resolution, HasBattuta&& hasBattuta) -> bool
{
    return resolution == ResolutionParry && hasBattuta();
}

} // namespace attackpostswinghelpers
