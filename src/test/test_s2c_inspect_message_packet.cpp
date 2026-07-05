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

#include "test_s2c_inspect_message_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0ca_inspect_message.h"

namespace
{

using InspectMessagePacket = GP_SERV_COMMAND_INSPECT_MESSAGE;

constexpr auto inspectMessageTextOffset        = sizeof(GP_SERV_HEADER) + offsetof(InspectMessagePacket::PacketData, sInspectMessage);
constexpr auto inspectMessageTextSize          = sizeof(InspectMessagePacket::PacketData{}.sInspectMessage);
constexpr auto inspectMessageFlagsOffset       = inspectMessageTextOffset + inspectMessageTextSize;
constexpr auto inspectMessageNameOffset        = sizeof(GP_SERV_HEADER) + offsetof(InspectMessagePacket::PacketData, sName);
constexpr auto inspectMessageDesignationOffset = sizeof(GP_SERV_HEADER) + offsetof(InspectMessagePacket::PacketData, DesignationNo);
constexpr auto inspectMessageNameSize          = sizeof(InspectMessagePacket::PacketData{}.sName);
constexpr auto inspectMessagePacketDataSize    = sizeof(InspectMessagePacket::PacketData);
constexpr auto inspectMessagePacketSize        = sizeof(GP_SERV_HEADER) + inspectMessagePacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c INSPECT_MESSAGE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c INSPECT_MESSAGE packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_INSPECT_MESSAGE), 0x0CA, "INSPECT_MESSAGE packet id") && ok;
    ok      = expectEqualUInt(inspectMessageTextSize, 123, "sInspectMessage size") && ok;
    ok      = expectEqualUInt(inspectMessageNameSize, 16, "sName size") && ok;
    ok      = expectEqualUInt(inspectMessagePacketDataSize, 144, "sizeof(INSPECT_MESSAGE::PacketData)") && ok;
    ok      = expectEqualUInt(inspectMessagePacketSize, 148, "INSPECT_MESSAGE packet size") && ok;
    ok      = expectEqualUInt(inspectMessageTextOffset, 4, "sInspectMessage offset") && ok;
    ok      = expectEqualUInt(inspectMessageFlagsOffset, 127, "flags offset") && ok;
    ok      = expectEqualUInt(inspectMessageNameOffset, 128, "sName offset") && ok;
    ok      = expectEqualUInt(inspectMessageDesignationOffset, 144, "DesignationNo offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data                 = InspectMessagePacket::PacketData{};
    data.sInspectMessage[0]   = 0x11;
    data.sInspectMessage[122] = 0x22;
    data.BazaarFlag           = 1;
    data.MyFlag               = 1;
    data.Race                 = 0x15;
    data.sName[0]             = 0x33;
    data.sName[15]            = 0x44;
    data.DesignationNo        = 0x11223344;

    auto expected = std::array<uint8, 144>{};
    expected[0]   = 0x11;
    expected[122] = 0x22;
    expected[123] = 0x57;
    expected[124] = 0x33;
    expected[139] = 0x44;
    expected[140] = 0x44;
    expected[141] = 0x33;
    expected[142] = 0x22;
    expected[143] = 0x11;

    return expectStructBytes(data, expected, "INSPECT_MESSAGE PacketData bytes");
}

} // namespace

auto runS2CInspectMessagePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
