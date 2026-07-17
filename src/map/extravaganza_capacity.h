#pragma once

#include "common/cbasetypes.h"

// Pure Alter Ego Extravaganza helpers shared by dual-wire slices:
//   - 2881: CanAffordCipher (shadowEraFinish allied_notes gate)
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

namespace extravaganzahelpers
{

// Allied_notes cost of one Shadow Era cipher (shadowEraFinish).
inline constexpr int32 CipherPurchaseNotes = 1000;

// CanAffordCipher is the pure free-function form of the shadowEraFinish
// notes gate:
//
//   notes >= CipherPurchaseNotes  // 1000
//
// Future Lua host injects the currency scalar into this helper instead of
// re-inlining the comparison.
inline auto CanAffordCipher(const int32 notes) -> bool
{
    return notes >= CipherPurchaseNotes;
}

} // namespace extravaganzahelpers
