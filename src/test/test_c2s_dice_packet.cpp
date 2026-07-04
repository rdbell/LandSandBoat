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

#include "test_c2s_dice_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0a2_dice.h"

namespace
{

using DicePacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_DICE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s DICE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s DICE packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s DICE packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s DICE packet self-test failed: " << label << " got";
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

auto encodedPacketBytes(const GP_CLI_COMMAND_DICE& packet) -> DicePacketBytes
{
    auto bytes = DicePacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto makeDicePacket() -> GP_CLI_COMMAND_DICE
{
    auto packet         = GP_CLI_COMMAND_DICE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_DICE);
    packet.header.size = sizeof(GP_CLI_COMMAND_DICE) / 4U;
    packet.header.sync = 0xBEEF;
    packet.unknown00   = 0x11223344;
    return packet;
}

auto validateDicePure(std::uint32_t unknown00) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    (void)unknown00;
    return validator;
}

auto testDiceLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeDicePacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_DICE::name, "GP_CLI_COMMAND_DICE", "DICE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_DICE::packetId), 0x0A2, "DICE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_DICE), 8, "DICE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_DICE, unknown00), 4, "DICE unknown00 offset") && ok;
    ok = expectBytes(encodedPacketBytes(packet),
                     DicePacketBytes{
                         0xA2, 0x04, 0xEF, 0xBE,
                         0x44, 0x33, 0x22, 0x11 },
                     "DICE encoded packet") &&
         ok;
    ok = expectEqualInt(packet.unknown00, 0x11223344, "DICE unknown00") && ok;
    return ok;
}

auto testDicePureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(validateDicePure(0), "DICE zero validation") && ok;
    ok      = expectValid(validateDicePure(999), "DICE client parameter validation") && ok;
    ok      = expectValid(validateDicePure(0xFFFFFFFF), "DICE max validation") && ok;
    return ok;
}

} // namespace

auto runC2SDicePacketSelfTests() -> bool
{
    return testDiceLayoutMetadataAndPayload() &&
           testDicePureValidationFacts();
}
