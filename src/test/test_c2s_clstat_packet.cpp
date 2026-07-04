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

#include "test_c2s_clstat_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x00f_clstat.h"

namespace
{

using CLStatBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CLSTAT)>;
using CLStatValues = std::array<std::uint32_t, 8>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CLSTAT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s CLSTAT packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CLSTAT packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const CLStatBytes& actual, const CLStatBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CLSTAT packet self-test failed: " << label << " got";
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

auto makeCLStat(const CLStatValues& values) -> GP_CLI_COMMAND_CLSTAT
{
    auto packet = GP_CLI_COMMAND_CLSTAT{};
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        packet.stat[i] = values[i];
    }
    return packet;
}

auto testDeclarationMetadata() -> bool
{
    auto packet = GP_CLI_COMMAND_CLSTAT{};
    bool ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_CLSTAT::name, "GP_CLI_COMMAND_CLSTAT", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_CLSTAT::packetId == PacketC2S::GP_CLI_COMMAND_CLSTAT, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CLSTAT), 36, "sizeof(GP_CLI_COMMAND_CLSTAT)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CLSTAT, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CLSTAT, stat), sizeof(GP_CLI_HEADER), "stat offset") && ok;
    ok = expectEqualInt(sizeof(packet.stat), 8 * sizeof(std::uint32_t), "stat array size") && ok;

    return ok;
}

auto testWireLayout() -> bool
{
    const auto values = CLStatValues{
        0x01020304,
        0x11223344,
        0x55667788,
        0x99AABBCC,
        0x0BADF00D,
        0x13579BDF,
        0x2468ACE0,
        0xFFFFFFFF,
    };
    auto packet        = makeCLStat(values);
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CLSTAT);
    packet.header.size = sizeof(GP_CLI_COMMAND_CLSTAT) / 4;
    packet.header.sync = 0xBEEF;

    auto bytes = CLStatBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());

    const auto expectedBytes = CLStatBytes{
        0x0F, 0x12, 0xEF, 0xBE,
        0x04, 0x03, 0x02, 0x01,
        0x44, 0x33, 0x22, 0x11,
        0x88, 0x77, 0x66, 0x55,
        0xCC, 0xBB, 0xAA, 0x99,
        0x0D, 0xF0, 0xAD, 0x0B,
        0xDF, 0x9B, 0x57, 0x13,
        0xE0, 0xAC, 0x68, 0x24,
        0xFF, 0xFF, 0xFF, 0xFF,
    };

    return expectBytes(bytes, expectedBytes, "encoded bytes");
}

auto testValidation() -> bool
{
    bool ok = true;

    ok = expectValid(makeCLStat(CLStatValues{}).validate(nullptr, nullptr), "zero CLSTAT") && ok;
    ok = expectValid(makeCLStat(CLStatValues{ 1 }).validate(nullptr, nullptr), "stat zero set") && ok;
    ok = expectValid(makeCLStat(CLStatValues{
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                         0xFFFFFFFF,
                     }).validate(nullptr, nullptr),
                     "all bits set") &&
         ok;

    return ok;
}

} // namespace

auto runC2SCLStatPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDeclarationMetadata() && ok;
    ok = testWireLayout() && ok;
    ok = testValidation() && ok;

    return ok;
}
