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

#include "test_c2s_charreq_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x016_charreq.h"

namespace
{

using CharReqBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CHARREQ)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHARREQ packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s CHARREQ packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHARREQ packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const CharReqBytes& actual, const CharReqBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHARREQ packet self-test failed: " << label << " got";
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

auto makeCharReq(std::uint16_t actIndex, std::uint16_t padding00) -> GP_CLI_COMMAND_CHARREQ
{
    auto packet      = GP_CLI_COMMAND_CHARREQ{};
    packet.ActIndex  = actIndex;
    packet.padding00 = padding00;
    return packet;
}

auto testDeclarationMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_CHARREQ::name, "GP_CLI_COMMAND_CHARREQ", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_CHARREQ::packetId == PacketC2S::GP_CLI_COMMAND_CHARREQ, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHARREQ), 8, "sizeof(GP_CLI_COMMAND_CHARREQ)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ, ActIndex), sizeof(GP_CLI_HEADER), "ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHARREQ, padding00), sizeof(GP_CLI_HEADER) + sizeof(std::uint16_t), "padding00 offset") && ok;

    return ok;
}

auto testWireLayout() -> bool
{
    auto packet        = makeCharReq(0x1122, 0x3344);
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHARREQ);
    packet.header.size = sizeof(GP_CLI_COMMAND_CHARREQ) / 4;
    packet.header.sync = 0xBEEF;

    auto bytes = CharReqBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());

    const auto expectedBytes = CharReqBytes{
        0x16, 0x04, 0xEF, 0xBE,
        0x22, 0x11, 0x44, 0x33,
    };

    return expectBytes(bytes, expectedBytes, "encoded bytes");
}

auto testValidation() -> bool
{
    bool ok = true;

    ok = expectValid(makeCharReq(0, 0).validate(nullptr, nullptr), "zero CHARREQ") && ok;
    ok = expectValid(makeCharReq(1, 0).validate(nullptr, nullptr), "non-zero ActIndex") && ok;
    ok = expectValid(makeCharReq(0xFFFF, 0xFFFF).validate(nullptr, nullptr), "all bits set") && ok;

    return ok;
}

} // namespace

auto runC2SCharReqPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDeclarationMetadata() && ok;
    ok = testWireLayout() && ok;
    ok = testValidation() && ok;

    return ok;
}
