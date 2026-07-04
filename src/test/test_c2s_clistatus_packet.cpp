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

#include "test_c2s_clistatus_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x061_clistatus.h"

namespace
{

using PacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CLISTATUS)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CLISTATUS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s CLISTATUS packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s CLISTATUS packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CLISTATUS packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const PacketBytes& actual, const PacketBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CLISTATUS packet self-test failed: " << label << " got";
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

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

auto encodedPacket(std::uint8_t unknown00, std::uint8_t padding00, std::uint16_t padding01) -> PacketBytes
{
    auto packet          = GP_CLI_COMMAND_CLISTATUS{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CLISTATUS);
    packet.header.size  = sizeof(GP_CLI_COMMAND_CLISTATUS) / 4;
    packet.header.sync  = 0xBEEF;
    packet.unknown00    = unknown00;
    packet.padding00    = padding00;
    packet.padding01    = padding01;

    auto bytes = PacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto testLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_CLISTATUS::name, "GP_CLI_COMMAND_CLISTATUS", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_CLISTATUS::packetId == PacketC2S::GP_CLI_COMMAND_CLISTATUS, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CLISTATUS), 8, "sizeof(GP_CLI_COMMAND_CLISTATUS)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CLISTATUS, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CLISTATUS, unknown00), 4, "unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CLISTATUS, padding00), 5, "padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CLISTATUS, padding01), 6, "padding01 offset") && ok;
    ok = expectBytes(encodedPacket(1, 0x22, 0x3344),
                     PacketBytes{ 0x61, 0x04, 0xEF, 0xBE, 0x01, 0x22, 0x44, 0x33 },
                     "encoded packet") &&
         ok;

    return ok;
}

auto testValidation() -> bool
{
    auto zero = GP_CLI_COMMAND_CLISTATUS{};
    zero.unknown00 = 0;

    auto one = GP_CLI_COMMAND_CLISTATUS{};
    one.unknown00 = 1;
    one.padding00 = 0xFF;
    one.padding01 = 0xFFFF;

    auto two = GP_CLI_COMMAND_CLISTATUS{};
    two.unknown00 = 2;

    bool ok = true;
    ok      = expectValid(zero.validate(nullptr, nullptr), "zero validation") && ok;
    ok      = expectValid(one.validate(nullptr, nullptr), "one validation") && ok;
    ok      = expectInvalidError(two.validate(nullptr, nullptr), "unknown00 out of range: 2 not in [0, 1]", "two validation") && ok;
    return ok;
}

} // namespace

auto runC2SCLIStatusPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testLayoutAndMetadata() && ok;
    ok = testValidation() && ok;

    return ok;
}
