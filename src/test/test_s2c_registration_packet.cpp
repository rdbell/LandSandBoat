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

#include "test_s2c_registration_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/base_entity.h"
#include "map/packets/s2c/0x0bf_registration.h"

namespace
{

constexpr auto registrationUnknown04Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_REGISTRATION::PacketData, unknown04);
constexpr auto registrationResultOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_REGISTRATION::PacketData, Result);
constexpr auto registrationUnknown08Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_REGISTRATION::PacketData, unknown08);
constexpr auto registrationActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_REGISTRATION::PacketData, ActIndex);
constexpr auto registrationPadding10Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_REGISTRATION::PacketData, padding10);
constexpr auto registrationPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_REGISTRATION::PacketData);
constexpr auto baseEntityProbeStartOffset  = sizeof(uint32);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c REGISTRATION packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(CBasicPacket& packet, const std::array<uint8, N>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c REGISTRATION packet self-test failed: " << label << " got";
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
            std::cerr << "s2c REGISTRATION packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_REGISTRATION::PacketData), 24, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(registrationPacketSize, 28, "packet size") && ok;
    ok      = expectEqualUInt(registrationUnknown04Offset, 4, "unknown04 offset") && ok;
    ok      = expectEqualUInt(registrationResultOffset, 6, "Result offset") && ok;
    ok      = expectEqualUInt(registrationUnknown08Offset, 8, "unknown08 offset") && ok;
    ok      = expectEqualUInt(registrationActIndexOffset, 12, "ActIndex offset") && ok;
    ok      = expectEqualUInt(registrationPadding10Offset, 16, "padding10 offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_REGISTRATION::PacketData::padding10), 12, "sizeof(padding10)") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    alignas(CBaseEntity) std::array<uint8, sizeof(CBaseEntity)> entranceStorage{};
    const auto targid = uint16_t{ 0xBEEF };
    for (std::size_t offset = baseEntityProbeStartOffset; offset < 64; offset += sizeof(targid))
    {
        std::memcpy(entranceStorage.data() + offset, &targid, sizeof(targid));
    }
    const auto* entrance = reinterpret_cast<const CBaseEntity*>(entranceStorage.data());

    auto packet = GP_SERV_COMMAND_REGISTRATION(entrance, 0x12345678);
    packet.setSequence(0xCAFE);

    const auto expected = std::array<uint8, 28>{
        0xBF, 0x0E, 0xFE, 0xCA,
        0x00, 0x00, 0x78, 0x56,
        0x00, 0x00, 0x00, 0x00,
        0xEF, 0xBE, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0BF, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "size") && ok;
    ok      = expectBytes(packet, expected, "encoded prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "tail") && ok;
    return ok;
}

} // namespace

auto runS2CRegistrationPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
