#pragma once

#include "common/cbasetypes.h"

// Pure CAMANContainer dual-wire helpers shared by OmegaXI slices:
//   - 0698: residual pure port (internal/aman mentor/mute state)
//   - 2967: CanThumbsUp residual dual-wire suite (thumbs_up)
//   - 3150: CanThumbsUp prior dedicated dual-wire (retained)
//   - 3229: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3278: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3308: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3419: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3473: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3541: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3585: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3630: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3675: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3720: CanThumbsUp dedicated dual-wire expand residual 2967 (thumbs_up.go)
//
// Dual-wire index:
//   - 2967: CanThumbsUp residual dual-wire suite
//   - 3150: CanThumbsUp prior dedicated dual-wire (retained)
//   - 3229: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3278: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3308: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3419: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3473: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3541: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3585: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3630: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3675: CanThumbsUp prior dedicated dual-wire expand residual 2967 (retained)
//   - 3720: CanThumbsUp = cooldownExpired (identity; dedicated expand residual 2967)
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
// Residual dual-wire suite: 2967.
// Prior dedicated dual-wire suites: 3150 / 3229 / 3278 / 3308 / 3419 / 3473 / 3541 / 3585 / 3630 / 3675 (retained).
// Dedicated dual-wire suite: 3720.
// Prior pure port: OmegaXI slice 0698 (internal/aman).

namespace amanhelpers
{

// CanThumbsUp mirrors CAMANContainer::canThumbsUp pure half
// (slice 3720 dedicated dual-wire expand residual 2967; prior dedicated
// 3675 / 3630 / 3585 / 3541 / 3473 / 3419 / 3308 / 3278 / 3229 / 3150 / pure 0698 — formula unchanged):
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
