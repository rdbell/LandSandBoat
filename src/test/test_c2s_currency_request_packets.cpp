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

#include "test_c2s_currency_request_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x10f_currencies_1.h"
#include "map/packets/c2s/0x115_currencies_2.h"

namespace
{

using Currencies1Bytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CURRENCIES_1)>;
using Currencies2Bytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CURRENCIES_2)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s currency request packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s currency request packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s currency request packet self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
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

template <typename Packet>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, sizeof(Packet)>
{
    auto bytes = std::array<std::uint8_t, sizeof(Packet)>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

template <typename Packet>
auto makeCurrencyPacket(PacketC2S packetId) -> Packet
{
    auto packet         = Packet{};
    packet.header.id   = static_cast<std::uint16_t>(packetId);
    packet.header.size = sizeof(Packet) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto testCurrencyRequestLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_CURRENCIES_1::name, "GP_CLI_COMMAND_CURRENCIES_1", "CURRENCIES_1 name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_CURRENCIES_2::name, "GP_CLI_COMMAND_CURRENCIES_2", "CURRENCIES_2 name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CURRENCIES_1::packetId), 0x10F, "CURRENCIES_1 packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CURRENCIES_2::packetId), 0x115, "CURRENCIES_2 packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_CURRENCIES_1), 4, "CURRENCIES_1 sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_CURRENCIES_2), 4, "CURRENCIES_2 sizeof") && ok;
    return ok;
}

auto testCurrencyRequestEncodedBytes() -> bool
{
    bool ok = true;

    auto currencies1Expected = Currencies1Bytes{ 0x0F, 0x03, 0xEF, 0xBE };
    ok                       = expectBytes(encodedPacketBytes(makeCurrencyPacket<GP_CLI_COMMAND_CURRENCIES_1>(PacketC2S::GP_CLI_COMMAND_CURRENCIES_1)), currencies1Expected, "CURRENCIES_1 encoded packet") && ok;

    auto currencies2Expected = Currencies2Bytes{ 0x15, 0x03, 0xEF, 0xBE };
    ok                       = expectBytes(encodedPacketBytes(makeCurrencyPacket<GP_CLI_COMMAND_CURRENCIES_2>(PacketC2S::GP_CLI_COMMAND_CURRENCIES_2)), currencies2Expected, "CURRENCIES_2 encoded packet") && ok;

    return ok;
}

} // namespace

auto runC2SCurrencyRequestPacketSelfTests() -> bool
{
    return testCurrencyRequestLayoutsAndMetadata() &&
           testCurrencyRequestEncodedBytes();
}
