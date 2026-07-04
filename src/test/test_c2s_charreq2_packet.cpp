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

#include "test_c2s_charreq2_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x017_charreq2.h"

namespace
{

using CharReq2Bytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CHARREQ2)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHARREQ2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s CHARREQ2 packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHARREQ2 packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const CharReq2Bytes& actual, const CharReq2Bytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHARREQ2 packet self-test failed: " << label << " got";
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

auto makeCharReq2(std::uint16_t actIndex, std::uint16_t padding00, std::uint32_t uniqueNo2, std::uint32_t uniqueNo3, std::uint16_t flg, std::uint16_t flg2) -> GP_CLI_COMMAND_CHARREQ2
{
    auto packet      = GP_CLI_COMMAND_CHARREQ2{};
    packet.ActIndex  = actIndex;
    packet.padding00 = padding00;
    packet.UniqueNo2 = uniqueNo2;
    packet.UniqueNo3 = uniqueNo3;
    packet.Flg       = flg;
    packet.Flg2      = flg2;
    return packet;
}

auto testDeclarationMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_CHARREQ2::name, "GP_CLI_COMMAND_CHARREQ2", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_CHARREQ2::packetId == PacketC2S::GP_CLI_COMMAND_CHARREQ2, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHARREQ2), 20, "sizeof(GP_CLI_COMMAND_CHARREQ2)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, ActIndex), sizeof(GP_CLI_HEADER), "ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, padding00), sizeof(GP_CLI_HEADER) + sizeof(std::uint16_t), "padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, UniqueNo2), 8, "UniqueNo2 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, UniqueNo3), 12, "UniqueNo3 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, Flg), 16, "Flg offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ2, Flg2), 18, "Flg2 offset") && ok;

    return ok;
}

auto testWireLayout() -> bool
{
    auto packet        = makeCharReq2(0x1122, 0x3344, 0x55667788, 0x99AABBCC, 0xDDEE, 0xF00D);
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHARREQ2);
    packet.header.size = sizeof(GP_CLI_COMMAND_CHARREQ2) / 4;
    packet.header.sync = 0xBEEF;

    auto bytes = CharReq2Bytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());

    const auto expectedBytes = CharReq2Bytes{
        0x17, 0x0A, 0xEF, 0xBE,
        0x22, 0x11,
        0x44, 0x33,
        0x88, 0x77, 0x66, 0x55,
        0xCC, 0xBB, 0xAA, 0x99,
        0xEE, 0xDD,
        0x0D, 0xF0,
    };

    return expectBytes(bytes, expectedBytes, "encoded bytes");
}

auto testValidation() -> bool
{
    bool ok = true;

    ok = expectValid(makeCharReq2(0, 0, 0, 0, 0, 0).validate(nullptr, nullptr), "zero CHARREQ2") && ok;
    ok = expectValid(makeCharReq2(1, 0, 2, 3, 4, 5).validate(nullptr, nullptr), "non-zero fields") && ok;
    ok = expectValid(makeCharReq2(0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0xFFFF).validate(nullptr, nullptr), "all bits set") && ok;

    return ok;
}

} // namespace

auto runC2SCharReq2PacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDeclarationMetadata() && ok;
    ok = testWireLayout() && ok;
    ok = testValidation() && ok;

    return ok;
}
