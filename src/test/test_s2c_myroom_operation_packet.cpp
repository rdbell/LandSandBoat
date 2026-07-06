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

#include "test_s2c_myroom_operation_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/item_container.h"
#include "map/items/item.h"
#include "map/packets/s2c/0x0fa_myroom_operation.h"

namespace
{

constexpr auto myRoomOperationItemNoOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData, MyroomItemNo);
constexpr auto myRoomOperationResultOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData, Result);
constexpr auto myRoomOperationUnknown00Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData, unknown00);
constexpr auto myRoomOperationItemIndexOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData, MyroomItemIndex);
constexpr auto myRoomOperationCategoryOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData, MyroomCategory);
constexpr auto myRoomOperationUnknown01Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData, unknown01);
constexpr auto myRoomOperationPacketDataSize      = sizeof(GP_SERV_COMMAND_MYROOM_OPERATION::PacketData);
constexpr auto myRoomOperationUnroundedPacketSize = sizeof(GP_SERV_HEADER) + myRoomOperationPacketDataSize;
constexpr auto myRoomOperationPacketSize          = 16U;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MYROOM_OPERATION packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MYROOM_OPERATION packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c MYROOM_OPERATION packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT), 1, "sizeof(Result)") && ok;
    ok      = expectEqualUInt(myRoomOperationPacketDataSize, 10, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(myRoomOperationUnroundedPacketSize, 14, "unrounded packet size") && ok;
    ok      = expectEqualUInt(myRoomOperationItemNoOffset, 4, "MyroomItemNo offset") && ok;
    ok      = expectEqualUInt(myRoomOperationResultOffset, 6, "Result offset") && ok;
    ok      = expectEqualUInt(myRoomOperationUnknown00Offset, 8, "unknown00 offset") && ok;
    ok      = expectEqualUInt(myRoomOperationItemIndexOffset, 10, "MyroomItemIndex offset") && ok;
    ok      = expectEqualUInt(myRoomOperationCategoryOffset, 11, "MyroomCategory offset") && ok;
    ok      = expectEqualUInt(myRoomOperationUnknown01Offset, 12, "unknown01 offset") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_MYROOM_OPERATION), 0x0FA, "packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::Ok), 0, "Ok enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::PlantAdd), 1, "PlantAdd enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::PlantCheck), 2, "PlantCheck enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::PlantCorp), 3, "PlantCorp enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::PlantStop), 4, "PlantStop enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::Layout), 5, "Layout enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::Bankin), 6, "Bankin enum") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_MYROOM_OPERATION_RESULT::End), 7, "End enum") && ok;
    ok      = expectEqualUInt(LOC_MOGSAFE2, 9, "LOC_MOGSAFE2") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto item   = CItem(0x2345);
    auto packet = GP_SERV_COMMAND_MYROOM_OPERATION(&item, LOC_MOGSAFE2, 0x56);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 16>{
        0xFA, 0x08, 0xEF, 0xBE,
        0x45, 0x23, 0x00, 0x00,
        0x00, 0x00, 0x56, 0x09,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0FA, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), myRoomOperationPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, expected, "encoded packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "tail") && ok;
    return ok;
}

} // namespace

auto runS2CMyRoomOperationPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
