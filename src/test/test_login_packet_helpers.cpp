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

#include "test_login_packet_helpers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "login/login_packets.h"

namespace
{

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login packet helper self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "login packet helper self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto packetHeaderFieldsUnchanged(const packet_t& packet) -> bool
{
    bool ok = true;
    ok      = expectEqualInt(packet.packet_size, 0x01020304, "packet_size unchanged") && ok;
    ok      = expectEqualInt(packet.terminator, 0x11223344, "terminator unchanged") && ok;
    ok      = expectEqualInt(packet.command, 0x55667788, "command unchanged") && ok;
    return ok;
}

auto makePacketWithHeader() -> packet_t
{
    auto packet        = packet_t{};
    packet.packet_size = 0x01020304;
    packet.terminator  = 0x11223344;
    packet.command     = 0x55667788;
    std::memset(packet.identifer, 0xAA, sizeof(packet.identifer));
    return packet;
}

auto testPacketHeaderLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(sizeof(packet_t), 28, "packet_t size") && ok;
    ok      = expectEqualInt(offsetof(packet_t, identifer), 12, "packet_t identifier offset") && ok;
    ok      = expectEqualInt(sizeof(packet_t{}.identifer), 16, "packet_t identifier length") && ok;
    return ok;
}

auto testTerminator() -> bool
{
    const auto terminator = loginPackets::getTerminator();

    bool ok = true;
    ok      = expectEqualInt(terminator, 0x46465849, "terminator value") && ok;
    ok      = expectEqualInt((terminator >> 0) & 0xFF, 'I', "terminator byte 0") && ok;
    ok      = expectEqualInt((terminator >> 8) & 0xFF, 'X', "terminator byte 1") && ok;
    ok      = expectEqualInt((terminator >> 16) & 0xFF, 'F', "terminator byte 2") && ok;
    ok      = expectEqualInt((terminator >> 24) & 0xFF, 'F', "terminator byte 3") && ok;
    return ok;
}

auto testCopyHashIntoPacket() -> bool
{
    auto packet = makePacketWithHeader();
    auto hash   = std::array<uint8_t, 16>{
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF,
    };

    loginPackets::copyHashIntoPacket(packet, hash.data());

    bool ok = true;
    ok      = expectTrue(std::memcmp(packet.identifer, hash.data(), hash.size()) == 0, "identifier contains copied hash") && ok;
    ok      = packetHeaderFieldsUnchanged(packet) && ok;
    return ok;
}

auto testClearIdentifier() -> bool
{
    auto packet = makePacketWithHeader();
    loginPackets::clearIdentifier(packet);

    bool ok = true;
    for (const auto value : packet.identifer)
    {
        ok = expectEqualInt(value, 0, "identifier byte cleared") && ok;
    }
    ok = packetHeaderFieldsUnchanged(packet) && ok;
    return ok;
}

auto testCharacterInfoLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(sizeof(TC_OPERATION_MAKE), 96, "TC_OPERATION_MAKE size") && ok;
    ok      = expectEqualInt(sizeof(lpkt_chr_info_sub2), 140, "lpkt_chr_info_sub2 size") && ok;
    ok      = expectEqualInt(sizeof(lpkt_chr_info2), 2272, "lpkt_chr_info2 size") && ok;
    ok      = expectEqualInt(offsetof(lpkt_chr_info_sub2, character_name), 12, "character name offset") && ok;
    ok      = expectEqualInt(offsetof(lpkt_chr_info_sub2, world_name), 28, "world name offset") && ok;
    ok      = expectEqualInt(offsetof(lpkt_chr_info_sub2, character_info), 44, "character operation offset") && ok;
    ok      = expectEqualInt(offsetof(TC_OPERATION_MAKE, GrapIDTbl), 12, "equipment offset") && ok;
    ok      = expectEqualInt(offsetof(TC_OPERATION_MAKE, zone_no), 28, "zone low offset") && ok;
    ok      = expectEqualInt(offsetof(TC_OPERATION_MAKE, zone_no2), 35, "zone high offset") && ok;
    ok      = expectEqualInt(offsetof(TC_OPERATION_MAKE, job_lev), 56, "job levels offset") && ok;
    return ok;
}

auto testCharacterInfoFixedSlotMutations() -> bool
{
    auto response       = lpkt_chr_info2{};
    response.characters = 4;
    for (std::size_t i = 0; i < std::size(response.character_info); ++i)
    {
        auto& slot                  = response.character_info[i];
        slot.ffxi_id                = static_cast<uint32_t>(100 + i);
        slot.status                 = 0xBEEF;
        slot.character_info.Gold    = static_cast<uint32_t>(900 + i);
        slot.character_name[0]      = 'X';
        slot.character_name[1]      = 'Y';
        slot.character_name[2]      = 'Z';
    }
    response.character_info[1].ffxi_id = 777;
    response.character_info[7].ffxi_id = 777;

    // Pure extraction of data_session::deleteCharFromCharInfo's fixed-array loop.
    for (auto& slot : response.character_info)
    {
        if (slot.ffxi_id == 777)
        {
            slot.status            = 0x01;
            slot.character_name[0] = 0x20;
            slot.character_name[1] = 0x00;
        }
    }

    bool ok = true;
    for (const auto index : { 1U, 7U })
    {
        const auto& slot = response.character_info[index];
        ok               = expectEqualInt(slot.status, 1, "deleted slot available") && ok;
        ok               = expectEqualInt(static_cast<uint8_t>(slot.character_name[0]), 0x20, "deleted slot space marker") && ok;
        ok               = expectEqualInt(static_cast<uint8_t>(slot.character_name[1]), 0, "deleted slot name terminator") && ok;
        ok               = expectEqualInt(static_cast<uint8_t>(slot.character_name[2]), 'Z', "deleted slot name tail preserved") && ok;
        ok               = expectEqualInt(slot.character_info.Gold, 900 + index, "deleted slot payload preserved") && ok;
    }
    ok = expectEqualInt(response.characters, 4, "delete preserves character count") && ok;
    ok = expectEqualInt(response.character_info[2].status, 0xBEEF, "delete preserves unrelated slot") && ok;

    response.character_info[2].character_name[0] = 0x20;
    response.character_info[3].character_name[0] = 0x20;
    auto replacement                               = lpkt_chr_info_sub2{};
    replacement.ffxi_id                            = 0x12345678;
    std::memcpy(replacement.character_name, "Added", 6);

    // Pure extraction of data_session::addCharIntoCharInfo's first-space loop.
    for (auto& slot : response.character_info)
    {
        if (slot.character_name[0] == 0x20)
        {
            slot = replacement;
            break;
        }
    }
    ok = expectEqualInt(response.character_info[1].ffxi_id, replacement.ffxi_id, "add reuses first deleted space slot") && ok;
    ok = expectEqualInt(response.character_info[2].ffxi_id, 102, "add leaves later space slot") && ok;
    ok = expectEqualInt(response.character_info[3].ffxi_id, 103, "add leaves final space slot") && ok;
    ok = expectEqualInt(response.characters, 4, "add preserves character count") && ok;
    return ok;
}

auto testCharacterIDAndZoneShaping() -> bool
{
    constexpr uint32_t characterID = 0xAB123456;
    constexpr uint16_t zone        = 0x03AB;
    constexpr auto     lowID       = static_cast<uint16_t>(characterID & 0xFFFF);
    constexpr auto     highID      = static_cast<uint8_t>((characterID >> 16) & 0xFF);
    constexpr auto     lowZone     = static_cast<uint8_t>(zone);
    constexpr auto     highZone    = static_cast<uint8_t>((zone >> 8) & 1);

    bool ok = true;
    ok      = expectEqualInt(lowID, 0x3456, "character ID low split") && ok;
    ok      = expectEqualInt(highID, 0x12, "character ID high split") && ok;
    ok      = expectEqualInt(lowZone, 0xAB, "zone low byte") && ok;
    ok      = expectEqualInt(highZone, 1, "zone high bit") && ok;
    return ok;
}

} // namespace

auto runLoginPacketHelperSelfTests() -> bool
{
    return testPacketHeaderLayout() &&
           testTerminator() &&
           testCopyHashIntoPacket() &&
           testClearIdentifier() &&
           testCharacterInfoLayout() &&
           testCharacterInfoFixedSlotMutations() &&
           testCharacterIDAndZoneShaping();
}
