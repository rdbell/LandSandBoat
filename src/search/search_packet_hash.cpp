/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "search_packet_hash.h"

#include "common/logging.h"
#include "common/md52.h"
#include "common/utils.h"

auto ValidateSearchPacketHash(uint8* packetData, uint16 length) -> bool
{
    // Check if packet is valid
    uint8 PacketHash[16]{};

    int32 toHash = length; // whole packet

    toHash -= 0x08; // -headersize
    toHash -= 0x10; // -hashsize
    toHash -= 0x04; // -keysize

    md5(reinterpret_cast<uint8*>(&packetData[8]), PacketHash, toHash);

    for (uint8 i = 0; i < 16; ++i)
    {
        if (packetData[length - 0x14 + i] != PacketHash[i])
        {
            ShowErrorFmt("Search hash wrong byte {}: {} should be {}", i, hex8ToString(PacketHash[i]), hex8ToString(packetData[length - 0x14 + i]));
            return false;
        }
    }

    return true;
}
