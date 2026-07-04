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

#include "test_c2s_chocobo_race_req_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x09b_chocobo_race_req.h"

namespace
{

using ChocoboRaceReqPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CHOCOBO_RACE_REQ)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHOCOBO_RACE_REQ packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s CHOCOBO_RACE_REQ packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s CHOCOBO_RACE_REQ packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHOCOBO_RACE_REQ packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s CHOCOBO_RACE_REQ packet self-test failed: " << label << " got";
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

auto encodedPacketBytes(const GP_CLI_COMMAND_CHOCOBO_RACE_REQ& packet) -> ChocoboRaceReqPacketBytes
{
    auto bytes = ChocoboRaceReqPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto makeChocoboRaceReqPacket() -> GP_CLI_COMMAND_CHOCOBO_RACE_REQ
{
    auto packet         = GP_CLI_COMMAND_CHOCOBO_RACE_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CHOCOBO_RACE_REQ);
    packet.header.size = sizeof(GP_CLI_COMMAND_CHOCOBO_RACE_REQ) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Param       = static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM::RacingWindowOpen);
    packet.Kind        = static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_KIND::ChocoboList);
    return packet;
}

auto validateChocoboRaceReqPure(std::uint32_t param, std::uint32_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM>(param)
        .oneOf<GP_CLI_COMMAND_CHOCOBO_RACE_REQ_KIND>(kind);
    return validator;
}

auto testChocoboRaceReqLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeChocoboRaceReqPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_CHOCOBO_RACE_REQ::name, "GP_CLI_COMMAND_CHOCOBO_RACE_REQ", "CHOCOBO_RACE_REQ name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ::packetId), 0x09B, "CHOCOBO_RACE_REQ packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CHOCOBO_RACE_REQ), 12, "CHOCOBO_RACE_REQ sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHOCOBO_RACE_REQ, Param), 4, "CHOCOBO_RACE_REQ Param offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CHOCOBO_RACE_REQ, Kind), 8, "CHOCOBO_RACE_REQ Kind offset") && ok;
    ok = expectBytes(encodedPacketBytes(packet),
                     ChocoboRaceReqPacketBytes{
                         0x9B, 0x06, 0xEF, 0xBE,
                         0x0F, 0x00, 0x00, 0x00,
                         0x02, 0x00, 0x00, 0x00 },
                     "CHOCOBO_RACE_REQ encoded packet") &&
         ok;
    ok = expectEqualInt(packet.Param, 0x0F, "CHOCOBO_RACE_REQ Param") && ok;
    ok = expectEqualInt(packet.Kind, 0x02, "CHOCOBO_RACE_REQ Kind") && ok;
    return ok;
}

auto testChocoboRaceReqEnumConstants() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM::None), 0x00, "Param::None") && ok;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM::RacingWindowOpen), 0x0F, "Param::RacingWindowOpen") && ok;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM::RacingWindowClose), 0x10, "Param::RacingWindowClose") && ok;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM::ChocoboListOpen), 0x11, "Param::ChocoboListOpen") && ok;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM::ChocoboListClose), 0x12, "Param::ChocoboListClose") && ok;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_KIND::Toteboard), 1, "Kind::Toteboard") && ok;
    ok      = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CHOCOBO_RACE_REQ_KIND::ChocoboList), 2, "Kind::ChocoboList") && ok;
    return ok;
}

auto testChocoboRaceReqPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(validateChocoboRaceReqPure(0x00, 1), "CHOCOBO_RACE_REQ none toteboard validation") && ok;
    ok      = expectValid(validateChocoboRaceReqPure(0x0F, 2), "CHOCOBO_RACE_REQ racing open list validation") && ok;
    ok      = expectValid(validateChocoboRaceReqPure(0x10, 1), "CHOCOBO_RACE_REQ racing close validation") && ok;
    ok      = expectValid(validateChocoboRaceReqPure(0x11, 2), "CHOCOBO_RACE_REQ list open validation") && ok;
    ok      = expectValid(validateChocoboRaceReqPure(0x12, 1), "CHOCOBO_RACE_REQ list close validation") && ok;
    ok      = expectInvalidError(validateChocoboRaceReqPure(1, 1), "1 not a valid GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM value.", "CHOCOBO_RACE_REQ invalid param validation") && ok;
    ok      = expectInvalidError(validateChocoboRaceReqPure(0, 0), "0 not a valid GP_CLI_COMMAND_CHOCOBO_RACE_REQ_KIND value.", "CHOCOBO_RACE_REQ invalid kind validation") && ok;
    ok      = expectInvalidError(validateChocoboRaceReqPure(1, 0), "1 not a valid GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM value.", "CHOCOBO_RACE_REQ validation order") && ok;
    ok      = expectInvalidError(validateChocoboRaceReqPure(0xFFFFFFFF, 1), "4294967295 not a valid GP_CLI_COMMAND_CHOCOBO_RACE_REQ_PARAM value.", "CHOCOBO_RACE_REQ max param validation") && ok;
    return ok;
}

} // namespace

auto runC2SChocoboRaceReqPacketSelfTests() -> bool
{
    return testChocoboRaceReqLayoutMetadataAndPayload() &&
           testChocoboRaceReqEnumConstants() &&
           testChocoboRaceReqPureValidationFacts();
}
