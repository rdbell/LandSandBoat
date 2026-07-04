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

#include "test_c2s_zone_transition_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x011_zone_transition.h"

namespace
{

using ZoneTransitionBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ZONE_TRANSITION)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ZONE_TRANSITION packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s ZONE_TRANSITION packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s ZONE_TRANSITION packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ZONE_TRANSITION packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const ZoneTransitionBytes& actual, const ZoneTransitionBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s ZONE_TRANSITION packet self-test failed: " << label << " got";
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

auto makeZoneTransition(std::uint8_t unknown00, std::uint8_t unknown01, std::uint16_t padding00) -> GP_CLI_COMMAND_ZONE_TRANSITION
{
    auto packet       = GP_CLI_COMMAND_ZONE_TRANSITION{};
    packet.unknown00  = unknown00;
    packet.unknown01  = unknown01;
    packet.padding00  = padding00;
    return packet;
}

auto validateZoneTransition(std::uint8_t unknown00, std::uint8_t unknown01, std::uint16_t padding00) -> PacketValidationResult
{
    const auto packet = makeZoneTransition(unknown00, unknown01, padding00);
    return packet.validate(nullptr, nullptr);
}

auto testDeclarationMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_ZONE_TRANSITION::name, "GP_CLI_COMMAND_ZONE_TRANSITION", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_ZONE_TRANSITION::packetId == PacketC2S::GP_CLI_COMMAND_ZONE_TRANSITION, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_ZONE_TRANSITION), 8, "sizeof(GP_CLI_COMMAND_ZONE_TRANSITION)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ZONE_TRANSITION, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ZONE_TRANSITION, unknown00), sizeof(GP_CLI_HEADER), "unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ZONE_TRANSITION, unknown01), sizeof(GP_CLI_HEADER) + sizeof(std::uint8_t), "unknown01 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_ZONE_TRANSITION, padding00), sizeof(GP_CLI_HEADER) + 2 * sizeof(std::uint8_t), "padding00 offset") && ok;

    return ok;
}

auto testWireLayout() -> bool
{
    auto packet        = makeZoneTransition(2, 0, 0x3344);
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_ZONE_TRANSITION);
    packet.header.size = sizeof(GP_CLI_COMMAND_ZONE_TRANSITION) / 4;
    packet.header.sync = 0xBEEF;

    auto bytes = ZoneTransitionBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());

    const auto expectedBytes = ZoneTransitionBytes{
        0x11, 0x04, 0xEF, 0xBE,
        0x02, 0x00, 0x44, 0x33,
    };

    return expectBytes(bytes, expectedBytes, "encoded bytes");
}

auto testValidation() -> bool
{
    bool ok = true;

    ok = expectValidation(validateZoneTransition(2, 0, 0xFFFF), true, "", "valid transition") && ok;
    ok = expectValidation(validateZoneTransition(1, 0, 0), false, "unknown00 not 2", "unknown00") && ok;
    ok = expectValidation(validateZoneTransition(2, 1, 0), false, "unknown01 not 0", "unknown01") && ok;
    ok = expectValidation(validateZoneTransition(1, 1, 0), false, "unknown00 not 2", "short-circuit order") && ok;

    return ok;
}

} // namespace

auto runC2SZoneTransitionPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testDeclarationMetadata() && ok;
    ok = testWireLayout() && ok;
    ok = testValidation() && ok;

    return ok;
}
