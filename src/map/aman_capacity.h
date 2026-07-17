#pragma once

#include "common/cbasetypes.h"

// Pure CAMANContainer dual-wire helpers shared by OmegaXI slices:
//   - 0698: residual pure port (internal/aman mentor/mute state)
//   - 2967: CanThumbsUp (cooldown-expired identity gate)
//
// Production host: CAMANContainer::canThumbsUp in aman.cpp:
//
//   return m_player->getCharVar("[ASSIST][ThumbsUp]Cooldown") == 0;
//
// Call sites inject the result into:
//   - packets/c2s/0x0b7_assist_channel.cpp GiveThumbsUp SelectAction
//     (!muted && PChar->aman().canThumbsUp())
//   - packets/s2c/0x01b_job_info.cpp canThumbsUpMentor fact
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/aman (thumbs_up.go) so hosts call CanThumbsUp instead of
// re-inlining the identity, and so production can inject:
//
//   CanThumbsUp(getCharVar(thumbsUpCooldownVar) == 0)
//
// Hosts inject the resolved cooldown-expired bool only (no CCharEntity*).
// Charvar writeback on give, JST midnight expiry, mute gate, and packet
// emission remain host-owned.
//
// Go dual-wire: aman.CanThumbsUp (internal/aman/thumbs_up.go).

namespace amanhelpers
{

// CanThumbsUp mirrors CAMANContainer::canThumbsUp pure half (slice 2967):
//
//   CanThumbsUp(cooldownExpired) = cooldownExpired
//
// cooldownExpired — host-injected (getCharVar("[ASSIST][ThumbsUp]Cooldown") == 0)
// true  → player may give a thumbs-up to a mentor
// false → thumbs-up cooldown still active
//
// Matches Go aman.CanThumbsUp and residual 0698 pure injectable.
inline auto CanThumbsUp(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

} // namespace amanhelpers
