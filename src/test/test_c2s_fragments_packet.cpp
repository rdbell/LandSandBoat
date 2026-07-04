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

#include "test_c2s_fragments_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x04b_fragments.h"

namespace
{

using PacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_FRAGMENTS)>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s FRAGMENTS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s FRAGMENTS packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s FRAGMENTS packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const PacketBytes& actual, const PacketBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s FRAGMENTS packet self-test failed: " << label << " got";
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

auto expectValid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectTrue(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), "", label + " error string") && ok;
    return ok;
}

auto encodedPacket() -> PacketBytes
{
    auto packet          = GP_CLI_COMMAND_FRAGMENTS{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_FRAGMENTS);
    packet.header.size  = sizeof(GP_CLI_COMMAND_FRAGMENTS) / 4;
    packet.header.sync  = 0xBEEF;
    packet.Command      = 1;
    packet.Result       = -2;
    packet.value1       = 3;
    packet.value2       = 4;
    packet.timestamp    = 0x11223344;
    packet.size_total   = 0x55667788;
    packet.offset       = -16;
    packet.data_size    = 0x01020304;

    auto bytes = PacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto testLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_FRAGMENTS::name, "GP_CLI_COMMAND_FRAGMENTS", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_FRAGMENTS::packetId == PacketC2S::GP_CLI_COMMAND_FRAGMENTS, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_FRAGMENTS), 24, "sizeof(GP_CLI_COMMAND_FRAGMENTS)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, Command), 4, "Command offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, Result), 5, "Result offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, value1), 6, "value1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, value2), 7, "value2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, timestamp), 8, "timestamp offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, size_total), 12, "size_total offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, offset), 16, "offset offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_FRAGMENTS, data_size), 20, "data_size offset") && ok;
    ok = expectBytes(encodedPacket(),
                     PacketBytes{ 0x4B, 0x0C, 0xEF, 0xBE,
                                  0x01, 0xFE, 0x03, 0x04,
                                  0x44, 0x33, 0x22, 0x11,
                                  0x88, 0x77, 0x66, 0x55,
                                  0xF0, 0xFF, 0xFF, 0xFF,
                                  0x04, 0x03, 0x02, 0x01 },
                     "encoded packet") &&
         ok;

    return ok;
}

auto testPayloadStorage() -> bool
{
    auto packet = GP_CLI_COMMAND_FRAGMENTS{};
    packet.Command = 1;
    packet.Result = -2;
    packet.value1 = 3;
    packet.value2 = 4;
    packet.timestamp = 0x11223344;
    packet.size_total = 0x55667788;
    packet.offset = -16;
    packet.data_size = 0x01020304;

    bool ok = true;
    ok      = expectEqualInt(packet.Command, 1, "Command") && ok;
    ok      = expectEqualInt(packet.Result, -2, "Result") && ok;
    ok      = expectEqualInt(packet.value1, 3, "value1") && ok;
    ok      = expectEqualInt(packet.value2, 4, "value2") && ok;
    ok      = expectEqualInt(packet.timestamp, 0x11223344, "timestamp") && ok;
    ok      = expectEqualInt(packet.size_total, 0x55667788, "size_total") && ok;
    ok      = expectEqualInt(packet.offset, -16, "offset") && ok;
    ok      = expectEqualInt(packet.data_size, 0x01020304, "data_size") && ok;
    return ok;
}

auto testValidation() -> bool
{
    auto packet = GP_CLI_COMMAND_FRAGMENTS{};
    packet.Command = 0xFF;
    packet.Result = -1;
    packet.value1 = 0xFF;
    packet.value2 = 0xFF;
    packet.timestamp = -1;
    packet.size_total = -2;
    packet.offset = -3;
    packet.data_size = -4;

    return expectValid(packet.validate(nullptr, nullptr), "validation");
}

} // namespace

auto runC2SFragmentsPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testLayoutAndMetadata() && ok;
    ok = testPayloadStorage() && ok;
    ok = testValidation() && ok;

    return ok;
}
