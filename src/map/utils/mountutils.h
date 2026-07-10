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

#include <cstdint>

class CCharEntity;
class CStatusEffect;

struct MountPacketDefinition
{
    uint8_t  ChocoboIndex;
    uint32_t CustomProperties[2];
};

enum class MountAnimation : uint8_t
{
    None,
    Chocobo,
    Mount,
};

struct MountStateResolution
{
    bool           mounted;
    uint16_t       mount;
    uint16_t       subPower;
    MountAnimation animation;
};

// Odd values work but will force short tail, even if the bit is set in traits.
enum class ChocoboColor : uint8_t
{
    Yellow = 0,
    Black  = 2,
    Blue   = 4,
    Red    = 6,
    Green  = 8,
};

struct ChocoboPhysicalTraits
{
    uint8_t largeBeak : 1;
    uint8_t unknown1 : 1;
    uint8_t unknown2 : 1;
    uint8_t largeTalons : 1;
    uint8_t unknown4 : 1;
    uint8_t unknown5 : 1;
    uint8_t fullTail : 1;
    uint8_t unknown7 : 1; // Gives an unknown purple tail variant on yellow chocobos
};

struct ChocoboCustomProperties
{
    union
    {
        uint32_t properties;

        struct
        {
            ChocoboPhysicalTraits traits;
            ChocoboColor          color;

            uint8_t unknown8; // Might be personal chocobo statistics.
            uint8_t unknown9; // Might be personal chocobo statistics.
        };
    };
};

namespace mountutils
{

auto resolveState(const CStatusEffect* effect) -> MountStateResolution;

auto packetDefinition(const CCharEntity* PChar) -> MountPacketDefinition;

}; // namespace mountutils
