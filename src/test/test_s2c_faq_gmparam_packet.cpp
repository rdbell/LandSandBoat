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

#include "test_s2c_faq_gmparam_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0b5_faq_gmparam.h"

namespace
{

constexpr auto faqGMParamRescueCountOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData, RescueCount);
constexpr auto faqGMParamParamsOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData, params);
constexpr auto faqGMParamIDOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData, Id);
constexpr auto faqGMParamOptionOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData, Option);
constexpr auto faqGMParamStatusOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData, Status);
constexpr auto faqGMParamRescueTimeOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData, RescueTime);
constexpr auto faqGMParamPacketDefaultSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c FAQ_GMPARAM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 32>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c FAQ_GMPARAM packet self-test failed: " << label << " got";
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
            std::cerr << "s2c FAQ_GMPARAM packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData), 28, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(faqGMParamPacketDefaultSize, 32, "packet default size") && ok;
    ok      = expectEqualUInt(faqGMParamRescueCountOffset, 4, "RescueCount offset") && ok;
    ok      = expectEqualUInt(faqGMParamParamsOffset, 8, "params offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_FAQ_GMPARAM::PacketData::params), 16, "sizeof(params)") && ok;
    ok      = expectEqualUInt(faqGMParamIDOffset, 24, "Id offset") && ok;
    ok      = expectEqualUInt(faqGMParamOptionOffset, 26, "Option offset") && ok;
    ok      = expectEqualUInt(faqGMParamStatusOffset, 28, "Status offset") && ok;
    ok      = expectEqualUInt(faqGMParamRescueTimeOffset, 30, "RescueTime offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FAQ_GMPARAM(0x1234);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 32>{
        0xB5, 0x10, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x34, 0x12, 0x00, 0x00,
        0x00, 0x00, 0x08, 0x07,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0B5, "FAQ_GMPARAM type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "FAQ_GMPARAM size") && ok;
    ok      = expectBytes(packet, expected, "encoded FAQ_GMPARAM prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "FAQ_GMPARAM tail") && ok;
    return ok;
}

} // namespace

auto runS2CFAQGMParamPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
