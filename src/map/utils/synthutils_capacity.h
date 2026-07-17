#pragma once

#include "common/cbasetypes.h"

// Pure synthutils helpers for dual-wire slices:
//   - 2876: CanSynthesizeHQ (anti-HQ mod gate after skill→Mod mapping)
//
// Production host: synthutils::canSynthesizeHQ in synthutils.cpp.
// Host resolves skillID → Mod::SYNTH_ANTI_HQ_* then injects getMod(ModID).
// Helpers take host-injected scalars only (no CCharEntity pointers).
// Skill→Mod catalog mapping remains host/mod-table work.
//
// Matches OmegaXI synthutils.CanSynthesizeHQ (slice 1922 / 2876):
//   antiHQMod == 0

namespace synthutilshelpers
{

// ---------------------------------------------------------------------------
// Slice 2876 — canSynthesizeHQ pure half (anti-HQ mod gate)
// ---------------------------------------------------------------------------

// CanSynthesizeHQ reports whether HQ is allowed given the injected anti-HQ
// mod value for the current craft skill (getMod(SYNTH_ANTI_HQ_*)).
// true when antiHQMod == 0 (no blocking ring/mod).
// Host still owns skillID → Mod mapping and PChar->getMod.
inline auto CanSynthesizeHQ(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

} // namespace synthutilshelpers
