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

#include "0x063_miscdata.h"
#include "base.h"

class CCharEntity;

// MISCDATA MERITS derives its payload from character, merit, job-point, and
// server-setting facts. Entity and service lookups remain in the constructor.
namespace miscdatameritshelpers
{

struct Facts
{
    uint16 limitPoints{};
    uint8  meritPoints{};
    bool   mainJobIsBlueMage{};
    uint8  mainJobLevel{};
    bool   hasLimitBreaker{};
    bool   meritMode{};
    uint8  blueAssimilationBonus{};
    uint8  blueJobPointBonus{};
    uint8  currentJobLevel{};
    uint8  levelLimit{};
    uint32 currentExperience{};
    uint32 experienceForNextLevel{};
    uint8  configuredMaxMeritPoints{};
    uint8  maxMeritBonus{};
};

struct Plan
{
    uint16 limitPoints{};
    uint8  meritPoints{};
    uint8  bluBonus{};
    bool   canUseMeritMode{};
    bool   xpCappedOrMeritMode{};
    bool   meritModeEnabled{};
    uint8  maxMeritPoints{};
};

[[nodiscard]] constexpr auto PlanFor(const Facts facts) -> Plan
{
    uint8 bluBonus = 0;
    if (facts.mainJobIsBlueMage)
    {
        if (facts.mainJobLevel >= 75)
        {
            bluBonus += facts.blueAssimilationBonus;
        }

        if (facts.mainJobLevel >= 99)
        {
            bluBonus += facts.blueJobPointBonus;
        }
    }

    const bool canUseMeritMode = facts.currentJobLevel >= 75 && facts.hasLimitBreaker;
    const bool atLevelLimit    = facts.currentJobLevel >= facts.levelLimit;
    const bool hasCappedXp     = facts.currentExperience == facts.experienceForNextLevel - 1;

    return {
        facts.limitPoints,
        facts.meritPoints,
        bluBonus,
        canUseMeritMode,
        (atLevelLimit && hasCappedXp) || facts.meritMode,
        canUseMeritMode && facts.meritMode,
        static_cast<uint8>(facts.configuredMaxMeritPoints + facts.maxMeritBonus),
    };
}

} // namespace miscdatameritshelpers

// https://github.com/atom0s/XiPackets/tree/main/world/server/0x0063
// This packet is sent by the server to inform the client of multiple different kinds of information.
namespace GP_SERV_COMMAND_MISCDATA
{

// Type 0x02: Merit/Limit Points (data: 12 bytes, total: 16 bytes)
class MERITS final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_MISCDATA, MERITS>
{
public:
    struct PacketData
    {
        GP_SERV_COMMAND_MISCDATA_TYPE type;                    // PS2: type
        uint16_t                      unknown06;               // PS2: (New; did not exist.)
        uint16_t                      limitPoints;             // Limit points available
        uint16_t                      meritPoints : 7;         // Merit points value (0-127)
        uint16_t                      bluBonus : 6;            // BLU spell point bonus (0-63)
        uint16_t                      canUseMeritMode : 1;     // Level >= 75 and has Limit Breaker KI
        uint16_t                      xpCappedOrMeritMode : 1; // XP is capped or player is in merit mode
        uint16_t                      meritModeEnabled : 1;    // Merit mode enabled and current job is eligible
        uint8_t                       maxMeritPoints;          // Maximum merit points
        uint8_t                       padding[3];
    };

    MERITS(CCharEntity* PChar);
};

} // namespace GP_SERV_COMMAND_MISCDATA
