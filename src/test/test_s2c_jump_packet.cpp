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

#include "test_s2c_jump_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x11e_jump.h"

namespace
{

using JumpPacket = GP_SERV_COMMAND_JUMP;

constexpr auto jumpPacketDataSize    = sizeof(JumpPacket::PacketData);
constexpr auto jumpPacketSize        = sizeof(GP_SERV_HEADER) + jumpPacketDataSize;
constexpr auto jumpActIndexOffset    = sizeof(GP_SERV_HEADER) + offsetof(JumpPacket::PacketData, ActIndex);
constexpr auto jumpPadding00Offset   = sizeof(GP_SERV_HEADER) + offsetof(JumpPacket::PacketData, padding00);
constexpr auto jumpPadding00Size     = sizeof(JumpPacket::PacketData::padding00);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c JUMP packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c JUMP packet self-test failed: " << label << " got";
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

template <std::size_t Size>
void putLE16(std::array<uint8, Size>& buffer, std::size_t offset, std::uint16_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_JUMP), 0x11E, "JUMP packet id") && ok;
    ok      = expectEqualUInt(jumpPacketDataSize, 4, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(jumpPacketSize, 8, "packet size") && ok;
    ok      = expectEqualUInt(jumpActIndexOffset, 4, "ActIndex offset") && ok;
    ok      = expectEqualUInt(jumpPadding00Offset, 6, "padding00 offset") && ok;
    ok      = expectEqualUInt(jumpPadding00Size, 2, "padding00 size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data             = JumpPacket::PacketData{};
    data.ActIndex         = 0x1234;
    data.padding00[0]     = 0x56;
    data.padding00[1]     = 0x78;

    auto expected = std::array<uint8, jumpPacketDataSize>{};
    putLE16(expected, 0, 0x1234);
    expected[2] = 0x56;
    expected[3] = 0x78;

    return expectStructBytes(data, expected, "JUMP PacketData bytes");
}

} // namespace

auto runS2CJumpPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
