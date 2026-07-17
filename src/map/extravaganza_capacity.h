#pragma once

#include "common/cbasetypes.h"

// Pure Alter Ego Extravaganza helpers shared by dual-wire slices:
//   - 2881: CanAffordCipher residual dual-wire suite (shadowEraFinish notes)
//   - 3153: CanAffordCipher prior dedicated dual-wire (retained)
//   - 3262: CanAffordCipher dedicated dual-wire (afford_cipher.go;
//           expand residual 2881)
//
// Dual-wire index:
//   - 2881: CanAffordCipher residual dual-wire suite
//   - 3153: CanAffordCipher prior dedicated dual-wire (retained)
//   - 3262: CanAffordCipher = notes >= CipherPurchaseNotes  // 1000
//
// Lua production host: scripts/globals/extravaganza.lua
//   xi.extravaganza.shadowEraFinish (~82–105):
//
//   local notes = player:getCurrency('allied_notes')
//   if option == 65537 or option == 131073 then
//     if notes >= 1000 then
//       if npcUtil.giveItem(player, cipherid[choice]) then
//         player:delCurrency('allied_notes', 1000)
//       end
//     elseif notes < 1000 then
//       player:messageSpecial(ID.text.NOT_ENOUGH_NOTES)
//     end
//   end
//
// Host injects scalars only (no player / entity pointers):
//   notes — player:getCurrency('allied_notes')
//
// giveItem / delCurrency / messageSpecial writeback remains host-owned.
// Prior pure port: OmegaXI slice 0949 (internal/extravaganza).
// Residual dual-wire suite: 2881 (test_extravaganza_afford_cipher_2881).
// Prior dedicated dual-wire: 3153 (test_extravaganza_can_afford_cipher_3153).
// Dedicated dual-wire expand residual 2881: 3262
//   (test_extravaganza_can_afford_cipher_3262).
// Go dual-wire: extravaganza.CanAffordCipher (internal/extravaganza/afford_cipher.go).
// Future Lua host injects free function then grant/debit/message writeback.

namespace extravaganzahelpers
{

// ---------------------------------------------------------------------------
// Slice 2881 residual / 3153 prior dedicated / 3262 dedicated —
// shadowEraFinish allied_notes gate
// ---------------------------------------------------------------------------

// Allied_notes cost of one Shadow Era cipher (shadowEraFinish).
// Same value used for delCurrency debit on success.
inline constexpr int32 CipherPurchaseNotes = 1000;

// CanAffordCipher is the pure free-function form of the shadowEraFinish
// notes gate:
//
//   notes >= CipherPurchaseNotes  // 1000
//
// Formula (slice 3262 dedicated dual-wire expand residual 2881; prior
// dedicated 3153 / pure 0949 — formula unchanged):
//
//   CanAffordCipher(notes) = notes >= CipherPurchaseNotes
//
// Future Lua host injects the currency scalar into this helper instead of
// re-inlining the comparison. Residual dual-wire suite: 2881 /
// test_extravaganza_afford_cipher_2881. Prior dedicated dual-wire suite:
// 3153 / test_extravaganza_can_afford_cipher_3153 (retained). Dedicated
// dual-wire suite is test_extravaganza_can_afford_cipher_3262.
inline auto CanAffordCipher(const int32 notes) -> bool
{
    return notes >= CipherPurchaseNotes;
}

} // namespace extravaganzahelpers
