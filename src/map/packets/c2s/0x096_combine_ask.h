/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#pragma once

#include <algorithm>
#include <array>

#include "base.h"
#include "common/timer.h"

namespace combineaskhelpers
{

// shouldWaitForSynth mirrors the COMBINE_ASK synthesis cooldown. The boundary
// is strict: exactly fifteen seconds after the prior start is allowed.
[[nodiscard]] inline auto shouldWaitForSynth(const timer::time_point lastSynthTime, const timer::time_point now) -> bool
{
    return lastSynthTime + std::chrono::seconds(15) > now;
}

// shouldClearMatchedTradePending mirrors the resolved-peer condition before
// COMBINE_ASK's trade-container gate.
[[nodiscard]] constexpr auto shouldClearMatchedTradePending(const bool targetPresent, const uint32 pendingID, const uint32 targetID) -> bool
{
    return targetPresent && pendingID == targetID;
}

// shouldCancelPeerTrade mirrors the target-dependent half of COMBINE_ASK's
// trade-container cancellation branch.
[[nodiscard]] constexpr auto shouldCancelPeerTrade(const bool targetPresent) -> bool
{
    return targetPresent;
}

struct CrystalFact
{
    bool   present{};
    uint16 inventoryItemID{};
    uint32 quantity{};
    bool   busy{};
    bool   locked{};
};

enum class CrystalAvailability : uint8
{
    Invalid,
    Busy,
    Usable,
};

[[nodiscard]] constexpr auto ClassifyCrystal(const uint16 requestedID, const CrystalFact& crystal) -> CrystalAvailability
{
    if (!crystal.present || requestedID != crystal.inventoryItemID || crystal.quantity == 0)
    {
        return CrystalAvailability::Invalid;
    }
    if (crystal.busy || crystal.locked)
    {
        return CrystalAvailability::Busy;
    }
    return CrystalAvailability::Usable;
}

// IngredientFact captures the inventory lookup result for one requested
// synthesis ingredient. The process host remains responsible for looking up
// the slot and for starting the transaction.
struct IngredientFact
{
    uint16 requestedID{};
    uint8  inventorySlot{};
    bool   present{};
    uint16 inventoryItemID{};
    uint32 quantity{};
    bool   busy{};
    bool   locked{};
};

// IngredientPlan identifies which requested ingredients may enter the synth
// offer. COMBINE_ASK deliberately skips an invalid ingredient instead of
// rejecting the entire request.
struct IngredientPlan
{
    std::array<bool, 8> accepted{};
};

[[nodiscard]] constexpr auto BuildIngredientPlan(const std::array<IngredientFact, 8>& ingredients, const uint8 ingredientCount) -> IngredientPlan
{
    IngredientPlan             plan;
    std::array<uint8, 256> slotQty{};
    const auto                 count = std::min<std::size_t>(ingredientCount, ingredients.size());

    for (std::size_t slotId = 0; slotId < count; ++slotId)
    {
        const auto& ingredient = ingredients[slotId];
        const auto  used       = ++slotQty[ingredient.inventorySlot];

        if (!ingredient.present || ingredient.inventoryItemID != ingredient.requestedID)
        {
            continue;
        }

        if (ingredient.busy || ingredient.locked || used > ingredient.quantity)
        {
            continue;
        }

        plan.accepted[slotId] = true;
    }

    return plan;
}

} // namespace combineaskhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0096
// This packet is sent by the client when requesting to synthesize an item.
GP_CLI_PACKET(GP_CLI_COMMAND_COMBINE_ASK,
              uint8_t  HashNo;     // PS2: HashNo
              uint8_t  padding00;  // PS2: (New; did not exist.)
              uint16_t Crystal;    // PS2: Crystal
              uint8_t  CrystalIdx; // PS2: CrystalIdx
              uint8_t  Items;      // PS2: Items
              uint16_t ItemNo[8];  // PS2: ItemNo
              uint8_t  TableNo[8]; // PS2: TableNo
              uint16_t padding01;  // PS2: (New; did not exist.)
);
