#pragma once

#include "common/cbasetypes.h"

// Pure Alter Ego Extravaganza helpers shared by dual-wire slices:
//   - 2881: CanAffordCipher residual dual-wire suite (shadowEraFinish notes)
//   - 3153: CanAffordCipher prior dedicated dual-wire (retained)
//   - 3262: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3292: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3322: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3408: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3462: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3504: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3559: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3604: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3649: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3694: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3739: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3784: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3829: CanAffordCipher dedicated dual-wire (afford_cipher.go;
//           expand residual 2881)
//
// Dual-wire index:
//   - 2881: CanAffordCipher residual dual-wire suite
//   - 3153: CanAffordCipher prior dedicated dual-wire (retained)
//   - 3262: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3292: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3322: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3408: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3462: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3504: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3559: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3604: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3649: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3694: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3739: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3784: CanAffordCipher prior dedicated dual-wire expand residual 2881
//           (retained)
//   - 3829: CanAffordCipher = notes >= CipherPurchaseNotes  // 1000
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
// Prior dedicated dual-wire expand residual 2881: 3262
//   (test_extravaganza_can_afford_cipher_3262; retained).
// Prior dedicated dual-wire expand residual 2881: 3292
//   (test_extravaganza_can_afford_cipher_3292; retained).
// Prior dedicated dual-wire expand residual 2881: 3322
//   (test_extravaganza_can_afford_cipher_3322; retained).
// Prior dedicated dual-wire expand residual 2881: 3408
//   (test_extravaganza_can_afford_cipher_3408; retained).
// Prior dedicated dual-wire expand residual 2881: 3462
//   (test_extravaganza_can_afford_cipher_3462; retained).
// Prior dedicated dual-wire expand residual 2881: 3504
//   (test_extravaganza_can_afford_cipher_3504; retained).
// Prior dedicated dual-wire expand residual 2881: 3559
//   (test_extravaganza_can_afford_cipher_3559; retained).
// Prior dedicated dual-wire expand residual 2881: 3604
//   (test_extravaganza_can_afford_cipher_3604; retained).
// Prior dedicated dual-wire expand residual 2881: 3649
//   (test_extravaganza_can_afford_cipher_3649; retained).
// Prior dedicated dual-wire expand residual 2881: 3694
//   (test_extravaganza_can_afford_cipher_3694; retained).
// Prior dedicated dual-wire expand residual 2881: 3739
//   (test_extravaganza_can_afford_cipher_3739; retained).
// Prior dedicated dual-wire expand residual 2881: 3784
//   (test_extravaganza_can_afford_cipher_3784; retained).
// Dedicated dual-wire expand residual 2881: 3829
//   (test_extravaganza_can_afford_cipher_3829).
// Go dual-wire: extravaganza.CanAffordCipher (internal/extravaganza/afford_cipher.go).
// Future Lua host injects free function then grant/debit/message writeback.

namespace extravaganzahelpers
{

// ---------------------------------------------------------------------------
// Slice 2881 residual / 3153 prior dedicated / 3262 prior expand residual /
// 3292 prior expand residual / 3322 prior expand residual / 3408 prior expand
// residual / 3462 prior expand residual / 3504 prior expand residual /
// 3559 prior expand residual / 3604 prior expand residual /
// 3649 prior expand residual / 3694 prior expand residual /
// 3739 prior expand residual / 3784 prior expand residual /
// 3829 dedicated — shadowEraFinish allied_notes gate
// ---------------------------------------------------------------------------

// Allied_notes cost of one Shadow Era cipher (shadowEraFinish).
// Same value used for delCurrency debit on success.
inline constexpr int32 CipherPurchaseNotes = 1000;

// CanAffordCipher is the pure free-function form of the shadowEraFinish
// notes gate:
//
//   notes >= CipherPurchaseNotes  // 1000
//
// Formula (slice 3829 dedicated dual-wire expand residual 2881; prior
// dedicated 3784 / 3739 / 3694 / 3649 / 3604 / 3559 / 3504 / 3462 / 3408 / 3322 / 3292 / 3262 / 3153 / pure 0949 — formula unchanged):
//
//   CanAffordCipher(notes) = notes >= CipherPurchaseNotes
//
// Future Lua host injects the currency scalar into this helper instead of
// re-inlining the comparison. Residual dual-wire suite: 2881 /
// test_extravaganza_afford_cipher_2881. Prior dedicated dual-wire suites:
// 3153 / test_extravaganza_can_afford_cipher_3153 (retained);
// 3262 / test_extravaganza_can_afford_cipher_3262 (retained);
// 3292 / test_extravaganza_can_afford_cipher_3292 (retained);
// 3322 / test_extravaganza_can_afford_cipher_3322 (retained);
// 3408 / test_extravaganza_can_afford_cipher_3408 (retained);
// 3462 / test_extravaganza_can_afford_cipher_3462 (retained);
// 3504 / test_extravaganza_can_afford_cipher_3504 (retained);
// 3559 / test_extravaganza_can_afford_cipher_3559 (retained);
// 3604 / test_extravaganza_can_afford_cipher_3604 (retained);
// 3649 / test_extravaganza_can_afford_cipher_3649 (retained);
// 3694 / test_extravaganza_can_afford_cipher_3694 (retained);
// 3739 / test_extravaganza_can_afford_cipher_3739 (retained);
// 3784 / test_extravaganza_can_afford_cipher_3784 (retained). Dedicated
// dual-wire suite is test_extravaganza_can_afford_cipher_3829.
inline auto CanAffordCipher(const int32 notes) -> bool
{
    return notes >= CipherPurchaseNotes;
}

} // namespace extravaganzahelpers
