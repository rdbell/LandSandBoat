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

#include "base.h"

#include <cstddef>
#include <cstdint>

// SCENARIOITEM's entity-independent key-item effects.
namespace scenarioitemhelpers
{

constexpr std::size_t LookItemFlagWordCount = 16;
constexpr std::size_t KeyItemsPerTable      = LookItemFlagWordCount * 32;

template <typename F>
void ForEachMarkedKeyItem(const uint16_t tableIndex,
                          const uint32_t (&lookItemFlag)[LookItemFlagWordCount],
                          F&& visit)
{
    for (std::size_t wordIndex = 0; wordIndex < LookItemFlagWordCount; ++wordIndex)
    {
        const auto flags = lookItemFlag[wordIndex];
        for (uint8_t bit = 0; bit < 32; ++bit)
        {
            if (((flags >> bit) & 1) != 0)
            {
                const auto keyItemId = static_cast<uint32_t>(tableIndex) * KeyItemsPerTable + wordIndex * 32 + bit;
                visit(static_cast<uint16_t>(keyItemId));
            }
        }
    }
}

[[nodiscard]] constexpr auto ShouldSaveKeyItems() -> bool
{
    return true;
}

} // namespace scenarioitemhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0064
// This packet is sent by the client when viewing a key item that has not been viewed before.
// (This marks the key item as 'seen' by the client to remove the yellow bubble when looking at the menu in the future. ie. 'Mark as Read')
GP_CLI_PACKET(GP_CLI_COMMAND_SCENARIOITEM,
              uint32_t UniqueNo;         // PS2: UniqueNo
              uint32_t LookItemFlag[16]; // PS2: para
              uint16_t ActIndex;         // PS2: ActIndex
              uint16_t TableIndex;       // PS2: Dammy
);
