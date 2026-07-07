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

#include "search_packet_crypto.h"

#include "common/md52.h"
#include "common/utils.h"

#include <cstring>

void DecryptSearchPacket(uint8* packetData, uint16 length, uint8* key, blowfish_t& blowfish)
{
    // Get key from packet
    ref<uint32>(key, 16) = ref<uint32>(packetData, length - 4);

    // Decrypt packet
    md5(reinterpret_cast<uint8*>(key), blowfish.hash, 20);

    blowfish_init(reinterpret_cast<int8*>(blowfish.hash), 16, blowfish.P, blowfish.S[0]);

    uint16 tmp = (length - 12) / 4;
    tmp -= tmp % 2;

    for (uint16 i = 0; i < tmp; i += 2)
    {
        blowfish_decipher(reinterpret_cast<uint32*>(packetData) + i + 2, reinterpret_cast<uint32*>(packetData) + i + 3, blowfish.P, blowfish.S[0]);
    }

    ref<uint32>(key, 20) = ref<uint32>(packetData, length - 0x18);
}

void EncryptSearchPacket(uint8* packetData, uint16 length, uint8* key, blowfish_t& blowfish)
{
    ref<uint16>(packetData, 0x00) = length;     // packet size
    ref<uint32>(packetData, 0x04) = 0x46465849; // "IXFF"

    md5(reinterpret_cast<uint8*>(key), blowfish.hash, 24);

    blowfish_init(reinterpret_cast<int8*>(blowfish.hash), 16, blowfish.P, blowfish.S[0]);

    md5(packetData + 8, packetData + length - 0x18 + 0x04, length - 0x18 - 0x04);

    uint8 tmp = (length - 12) / 4;
    tmp -= tmp % 2;

    for (uint8 i = 0; i < tmp; i += 2)
    {
        blowfish_encipher(reinterpret_cast<uint32*>(packetData) + i + 2, reinterpret_cast<uint32*>(packetData) + i + 3, blowfish.P, blowfish.S[0]);
    }

    std::memcpy(&packetData[length] - 0x04, key + 16, 4);
}
