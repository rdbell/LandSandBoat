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

enum class GP_CLI_COMMAND_SITCHAIR_MODE : uint8
{
    Toggle = 0,
    On     = 1,
    Off    = 2,
};

namespace sitchairhelpers
{
constexpr uint8_t AnimationNone      = 0;
constexpr uint8_t AnimationSitchair0 = 63;
constexpr uint16_t ChairKeyItemBase  = 0xACA;

struct Transition
{
    uint8_t animation;
    bool    removeHealingSilently;
    bool    setUpdateHP;
};

inline auto RequiresChairKeyItem(const uint8_t chairAnimation) -> bool
{
    // SITCHAIR_0 is always available. IDs 64 through 83 are key-item chairs,
    // including the reserved chair slots accepted by packet validation.
    return chairAnimation != AnimationSitchair0;
}

inline auto ChairKeyItemID(const uint8_t chairAnimation) -> uint16_t
{
    return static_cast<uint16_t>(chairAnimation + ChairKeyItemBase);
}

// SelectTransition mirrors process after its live status-effect and key-item
// lookups. Mode On intentionally has the same toggle behavior as Toggle.
inline auto SelectTransition(const uint8_t mode, uint8_t chairAnimation, const uint8_t currentAnimation, const bool hasRequiredChairKeyItem) -> Transition
{
    constexpr auto Off = static_cast<uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::Off);
    if (mode == Off)
    {
        return { AnimationNone, true, true };
    }

    if (RequiresChairKeyItem(chairAnimation) && !hasRequiredChairKeyItem)
    {
        chairAnimation = AnimationSitchair0;
    }

    return { currentAnimation == chairAnimation ? AnimationNone : chairAnimation, true, true };
}
} // namespace sitchairhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0113
// This packet is sent by the client when requesting to sit in a chair.
GP_CLI_PACKET(GP_CLI_COMMAND_SITCHAIR,
              uint32_t Mode;    // The packet mode.
              uint32_t ChairId; // The id of the chair the client wishes to sit on.
);
