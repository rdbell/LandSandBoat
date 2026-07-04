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

#include "test_c2s_gameok_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x00c_gameok.h"

namespace
{

using GameOKBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GAMEOK)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s GAMEOK packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s GAMEOK packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s GAMEOK packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s GAMEOK packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const GameOKBytes& actual, const GameOKBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s GAMEOK packet self-test failed: " << label << " got";
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

auto expectValidation(const PacketValidationResult& result, bool expectedValid, std::string_view expectedError, const std::string& label) -> bool
{
    bool ok = true;
    if (expectedValid)
    {
        ok = expectTrue(result.valid(), label + " valid") && ok;
    }
    else
    {
        ok = expectFalse(result.valid(), label + " valid") && ok;
    }
    ok = expectEqualString(result.errorString(), expectedError, label + " error string") && ok;
    return ok;
}

auto makeGameOK(std::uint32_t clientState, std::uint32_t debugClientFlg, std::uint32_t unused) -> GP_CLI_COMMAND_GAMEOK
{
    auto packet           = GP_CLI_COMMAND_GAMEOK{};
    packet.ClientState   = clientState;
    packet.DebugClientFlg = debugClientFlg;
    packet.unused         = unused;
    return packet;
}

auto validateGameOK(std::uint32_t clientState, std::uint32_t debugClientFlg, std::uint32_t unused) -> PacketValidationResult
{
    const auto packet = makeGameOK(clientState, debugClientFlg, unused);
    return packet.validate(nullptr, nullptr);
}

auto testDeclarationMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_GAMEOK::name, "GP_CLI_COMMAND_GAMEOK", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_GAMEOK::packetId == PacketC2S::GP_CLI_COMMAND_GAMEOK, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GAMEOK), 12, "sizeof(GP_CLI_COMMAND_GAMEOK)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GAMEOK, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GAMEOK, ClientState), sizeof(GP_CLI_HEADER), "ClientState offset") && ok;

    return ok;
}

auto testWireLayout() -> bool
{
    auto packet           = GP_CLI_COMMAND_GAMEOK{};
    packet.header.id      = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GAMEOK);
    packet.header.size    = sizeof(GP_CLI_COMMAND_GAMEOK) / 4;
    packet.header.sync    = 0xBEEF;
    packet.ClientState    = 0x11223344;
    packet.DebugClientFlg = 1;
    packet.unused         = 0;

    auto bytes = GameOKBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());

    const auto expectedBytes = GameOKBytes{
        0x0C, 0x06, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x01, 0x00, 0x00, 0x00,
    };

    return expectBytes(bytes, expectedBytes, "encoded bytes");
}

auto testValidation() -> bool
{
    bool ok = true;

    ok = expectValidation(validateGameOK(0, 0, 0), true, "", "zero GAMEOK") && ok;
    ok = expectValidation(validateGameOK(1, 0, 0), false, "ClientState not 0", "client state") && ok;
    ok = expectValidation(validateGameOK(0, 1, 0), false, "DebugClientFlg not 0", "debug flag") && ok;
    ok = expectValidation(validateGameOK(1, 1, 0), false, "ClientState not 0", "short-circuit order") && ok;
    ok = expectValidation(validateGameOK(0, 0, 0x1ABCDE), true, "", "unused bitfield ignored") && ok;

    return ok;
}

auto testUnusedBitsShareDebugStorageWord() -> bool
{
    const auto packet = makeGameOK(0, 0, 0x1ABCDE);

    auto bytes = GameOKBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());

    std::uint32_t debugStorage = 0;
    std::memcpy(&debugStorage, bytes.data() + 8, sizeof(debugStorage));

    return expectEqualInt(debugStorage & 1U, 0, "DebugClientFlg low bit") &&
           expectEqualInt(debugStorage >> 1U, 0x1ABCDE, "unused high bits");
}

} // namespace

auto runC2SGameOKPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDeclarationMetadata() && ok;
    ok = testWireLayout() && ok;
    ok = testValidation() && ok;
    ok = testUnusedBitsShareDebugStorageWord() && ok;

    return ok;
}
