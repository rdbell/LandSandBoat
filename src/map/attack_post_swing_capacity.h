#pragma once

#include "common/cbasetypes.h"

// Pure post-swing effect-gate helpers (slice 1399 residual).
// Dual-wire residual expansions:
//   - 3046: ShouldRunEnspell (live-target gate before HandleEnspell)
//
// Production host: CBattleEntity::OnAttack (battle_entity.cpp ~3844) injects
// PTarget->GetHPP() into ShouldRunEnspell after ShouldRunEnspellAndSpikes
// admits hit/guard/block (non-Daken). When false, host skips
// battleutils::HandleEnspell while still calling HandleSpikesDamage under the
// outer gate.
// Go dual-wire: attack.ShouldRunEnspell (internal/attack/run_enspell.go).
//
// Sibling residual gates in this header:
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

// ShouldRunEnspell suppresses enspell handling after the target dies.
//
// Formula (slice 3046 dual-wire):
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
// Sibling residual gates: ShouldRunEnspellAndSpikes / ShouldRunParrySpikes.
// Coverage: test_attack_run_enspell_3046 (not in CMake/main).
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
