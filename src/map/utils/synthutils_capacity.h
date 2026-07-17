#pragma once

#include "common/cbasetypes.h"

// Pure synthutils helpers for dual-wire slices:
//   - 2876: CanSynthesizeHQ residual dual-wire suite (anti-HQ mod gate after
//           skill→Mod mapping)
//   - 3244: CanSynthesizeHQ prior dedicated dual-wire expand residual 2876
//           (formula unchanged: antiHQMod == 0)
//   - 3283: CanSynthesizeHQ prior dedicated dual-wire expand residual 2876
//           (prior ~3244; formula unchanged: antiHQMod == 0)
//   - 3313: CanSynthesizeHQ dedicated dual-wire expand residual 2876
//           (prior ~3283; formula unchanged: antiHQMod == 0)
//
// Dual-wire index:
//   - 2876: CanSynthesizeHQ residual pure dual-wire
//   - 3244: CanSynthesizeHQ = antiHQMod == 0
//     prior dedicated dual-wire expand residual 2876
//   - 3283: CanSynthesizeHQ = antiHQMod == 0
//     prior dedicated dual-wire expand residual 2876 (prior ~3244)
//   - 3313: CanSynthesizeHQ = antiHQMod == 0
//     dedicated dual-wire expand residual 2876 (prior ~3283)
//
// Production host: synthutils::canSynthesizeHQ in synthutils.cpp.
// Host resolves skillID → Mod::SYNTH_ANTI_HQ_* then injects getMod(ModID).
// Helpers take host-injected scalars only (no CCharEntity pointers).
// Skill→Mod catalog mapping remains host/mod-table work.
//
// Matches OmegaXI synthutils.CanSynthesizeHQ (slice 1922 / residual 2876 /
// prior dedicated 3244 / 3283 / dedicated expand 3313):
//   antiHQMod == 0
//
// Coverage: test_synthutils_can_hq_2876 (residual dual-wire),
// test_synthutils_can_synthesize_hq_3244 (prior dedicated expand residual
// 2876), test_synthutils_can_synthesize_hq_3283 (prior dedicated expand
// residual 2876), test_synthutils_can_synthesize_hq_3313 (dedicated expand
// residual 2876; not in CMake/main).

namespace synthutilshelpers
{

// ---------------------------------------------------------------------------
// Slice 2876 residual / 3244 / 3283 prior dedicated / 3313 expand residual 2876
// — canSynthesizeHQ pure half (anti-HQ mod gate)
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3313):
//   Formula unchanged from pure 1922 / residual dual-wire 2876 / prior
//   dedicated expand 3244 / 3283:
//     CanSynthesizeHQ(antiHQMod) = antiHQMod == 0
//   Go dual-wire: synthutils.CanSynthesizeHQ (internal/synthutils/can_hq.go).
//   Production host: synthutils::canSynthesizeHQ resolves skill→Mod then
//   injects PChar->getMod(ModID) into this free function (not re-inlined).
//   Skill→Mod catalog mapping remains host/mod-table work.
// Coverage: test_synthutils_can_synthesize_hq_3313 (dedicated expand residual
// 2876; not in CMake/main); residual 2876 + prior dedicated 3244 / 3283
// suites retained.

// CanSynthesizeHQ reports whether HQ is allowed given the injected anti-HQ
// mod value for the current craft skill (getMod(SYNTH_ANTI_HQ_*)).
//
// Formula (slice 3313 dedicated dual-wire expand residual 2876; prior
// dedicated 3283 / 3244 / pure 1922 — formula unchanged):
//   CanSynthesizeHQ(antiHQMod) = antiHQMod == 0
//
// true when antiHQMod == 0 (no blocking ring/mod).
// Host still owns skillID → Mod mapping and PChar->getMod.
// Dual-wire of Go synthutils.CanSynthesizeHQ (can_hq.go / slice 3313).
inline auto CanSynthesizeHQ(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

} // namespace synthutilshelpers
