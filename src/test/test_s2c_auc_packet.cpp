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

#include "test_s2c_auc_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/c2s/0x04e_auc.h"
#include "map/packets/s2c/0x04c_auc.h"

namespace
{

constexpr auto aucCommandOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_AUC::PacketData, Command);
constexpr auto aucWorkIndexOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_AUC::PacketData, AucWorkIndex);
constexpr auto aucResultOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_AUC::PacketData, Result);
constexpr auto aucResultStatusOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_AUC::PacketData, ResultStatus);
constexpr auto aucParamOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_AUC::PacketData, Param);
constexpr auto aucParcelOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_AUC::PacketData, Parcel);
constexpr auto aucPacketDataSize     = sizeof(GP_SERV_COMMAND_AUC::PacketData);
constexpr auto aucFullPacketSize     = sizeof(GP_SERV_HEADER) + aucPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c AUC packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c AUC packet self-test failed: " << label << " got";
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

auto expectRepeatedByte(CBasicPacket& packet, std::size_t offset, std::size_t count, uint8 expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = 0; i < count; ++i)
    {
        if (data[offset + i] != expected)
        {
            std::cerr << "s2c AUC packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_AUC_PARAM), 12, "sizeof(GP_AUC_PARAM)") && ok;
    ok      = expectEqualUInt(sizeof(GP_AUC_BOX), 40, "sizeof(GP_AUC_BOX)") && ok;
    ok      = expectEqualUInt(aucPacketDataSize, 56, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(aucFullPacketSize, 60, "full packet size") && ok;
    ok      = expectEqualUInt(aucCommandOffset, 4, "Command offset") && ok;
    ok      = expectEqualUInt(aucWorkIndexOffset, 5, "AucWorkIndex offset") && ok;
    ok      = expectEqualUInt(aucResultOffset, 6, "Result offset") && ok;
    ok      = expectEqualUInt(aucResultStatusOffset, 7, "ResultStatus offset") && ok;
    ok      = expectEqualUInt(aucParamOffset, 8, "Param offset") && ok;
    ok      = expectEqualUInt(aucParcelOffset, 20, "Parcel offset") && ok;
    return ok;
}

auto testOpenConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND::Open);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x04C, "open type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 60, "open size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x4C, 0x1E, 0xEF, 0xBE }, "open header") && ok;
    ok      = expectBytes(packet, aucCommandOffset, std::array<uint8, 4>{ 0x02, 0xFF, 0x01, 0x00 }, "open scalar fields") && ok;
    ok      = expectRepeatedByte(packet, aucParamOffset, PACKET_SIZE - aucParamOffset, 0, "open params and tail") && ok;
    return ok;
}

auto testBidConstructorStackQuantity() -> bool
{
    auto packet = GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND::Bid, 0x07, 0x1234, 0x01020304, 0, 12);
    packet.setSequence(0xCAFE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 60, "bid stack size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x4C, 0x1E, 0xFE, 0xCA }, "bid stack header") && ok;
    ok      = expectBytes(packet, aucCommandOffset, std::array<uint8, 4>{ 0x0E, 0x00, 0x07, 0x00 }, "bid stack scalar fields") && ok;
    ok      = expectBytes(packet, aucParamOffset, std::array<uint8, 12>{ 0x04, 0x03, 0x02, 0x01, 0x34, 0x12, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00 }, "bid stack param") && ok;
    ok      = expectRepeatedByte(packet, aucParcelOffset, PACKET_SIZE - aucParcelOffset, 0, "bid stack parcel and tail") && ok;
    return ok;
}

auto testBidConstructorSingleQuantity() -> bool
{
    auto packet = GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND::Bid, 0x02, 0x5678, 0x11121314, 3, 99);

    bool ok = true;
    ok      = expectBytes(packet, aucCommandOffset, std::array<uint8, 4>{ 0x0E, 0x00, 0x02, 0x00 }, "bid single scalar fields") && ok;
    ok      = expectBytes(packet, aucParamOffset, std::array<uint8, 12>{ 0x14, 0x13, 0x12, 0x11, 0x78, 0x56, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 }, "bid single param") && ok;
    return ok;
}

} // namespace

auto runS2CAUCPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testOpenConstructor() && ok;
    ok      = testBidConstructorStackQuantity() && ok;
    ok      = testBidConstructorSingleQuantity() && ok;
    return ok;
}
