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

#include "test_c2s_rescue_buff_submap_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0f0_rescue.h"
#include "map/packets/c2s/0x0f1_buffcancel.h"
#include "map/packets/c2s/0x0f2_submapchange.h"
#include "map/status_effect.h"

namespace
{

using RescueBytes      = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_RESCUE)>;
using BuffCancelBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BUFFCANCEL)>;
using SubmapChangeBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SUBMAPCHANGE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s rescue/buff/submap packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s rescue/buff/submap packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s rescue/buff/submap packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s rescue/buff/submap packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s rescue/buff/submap packet self-test failed: " << label << " got";
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

template <typename Packet>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, sizeof(Packet)>
{
    auto bytes = std::array<std::uint8_t, sizeof(Packet)>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto validateRescuePure(std::uint32_t state) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(state, 0, "State not 0");
    return validator;
}

auto validateBuffCancelPure(std::uint16_t buffNo) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("BuffNo", buffNo, 0, MAX_EFFECTID);
    return validator;
}

auto validateSubmapChangePure(std::uint16_t state) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("State", state, GP_CLI_COMMAND_SUBMAPCHANGE_STATE::General, GP_CLI_COMMAND_SUBMAPCHANGE_STATE::Event);
    return validator;
}

auto makeRescuePacket() -> GP_CLI_COMMAND_RESCUE
{
    auto packet         = GP_CLI_COMMAND_RESCUE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_RESCUE);
    packet.header.size = sizeof(GP_CLI_COMMAND_RESCUE) / 4U;
    packet.header.sync = 0xBEEF;
    packet.State       = 0x11223344;
    return packet;
}

auto makeBuffCancelPacket() -> GP_CLI_COMMAND_BUFFCANCEL
{
    auto packet         = GP_CLI_COMMAND_BUFFCANCEL{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BUFFCANCEL);
    packet.header.size = sizeof(GP_CLI_COMMAND_BUFFCANCEL) / 4U;
    packet.header.sync = 0xBEEF;
    packet.BuffNo      = 0x0123;
    packet.padding00   = 0xABCD;
    return packet;
}

auto makeSubmapChangePacket() -> GP_CLI_COMMAND_SUBMAPCHANGE
{
    auto packet          = GP_CLI_COMMAND_SUBMAPCHANGE{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SUBMAPCHANGE);
    packet.header.size  = sizeof(GP_CLI_COMMAND_SUBMAPCHANGE) / 4U;
    packet.header.sync  = 0xBEEF;
    packet.State        = static_cast<std::uint16_t>(GP_CLI_COMMAND_SUBMAPCHANGE_STATE::Event);
    packet.SubMapNumber = 0x3344;
    return packet;
}

auto testRescueBuffSubmapLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_RESCUE::name, "GP_CLI_COMMAND_RESCUE", "RESCUE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_RESCUE::packetId), 0x0F0, "RESCUE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_RESCUE), 8, "RESCUE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_RESCUE, State), 4, "RESCUE State offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_BUFFCANCEL::name, "GP_CLI_COMMAND_BUFFCANCEL", "BUFFCANCEL name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BUFFCANCEL::packetId), 0x0F1, "BUFFCANCEL packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_BUFFCANCEL), 8, "BUFFCANCEL sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_BUFFCANCEL, BuffNo), 4, "BUFFCANCEL BuffNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_BUFFCANCEL, padding00), 6, "BUFFCANCEL padding00 offset") && ok;
    ok = expectEqualInt(MAX_EFFECTID, 814, "MAX_EFFECTID") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_SUBMAPCHANGE::name, "GP_CLI_COMMAND_SUBMAPCHANGE", "SUBMAPCHANGE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SUBMAPCHANGE::packetId), 0x0F2, "SUBMAPCHANGE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SUBMAPCHANGE), 8, "SUBMAPCHANGE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBMAPCHANGE, State), 4, "SUBMAPCHANGE State offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SUBMAPCHANGE, SubMapNumber), 6, "SUBMAPCHANGE SubMapNumber offset") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SUBMAPCHANGE_STATE::General), 1, "SUBMAPCHANGE_STATE::General") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SUBMAPCHANGE_STATE::Event), 2, "SUBMAPCHANGE_STATE::Event") && ok;
    return ok;
}

auto testRescueBuffSubmapEncodedBytesAndPayloads() -> bool
{
    bool ok = true;
    ok      = expectBytes(encodedPacketBytes(makeRescuePacket()), RescueBytes{ 0xF0, 0x04, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11 }, "RESCUE encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeBuffCancelPacket()), BuffCancelBytes{ 0xF1, 0x04, 0xEF, 0xBE, 0x23, 0x01, 0xCD, 0xAB }, "BUFFCANCEL encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeSubmapChangePacket()), SubmapChangeBytes{ 0xF2, 0x04, 0xEF, 0xBE, 0x02, 0x00, 0x44, 0x33 }, "SUBMAPCHANGE encoded packet") && ok;
    return ok;
}

auto testRescueBuffSubmapValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateRescuePure(0), "RESCUE state zero validation") && ok;
    ok      = expectInvalidError(validateRescuePure(1), "State not 0", "RESCUE invalid state validation") && ok;
    ok      = expectValid(validateBuffCancelPure(0), "BUFFCANCEL zero validation") && ok;
    ok      = expectValid(validateBuffCancelPure(MAX_EFFECTID), "BUFFCANCEL max validation") && ok;
    ok      = expectInvalidError(validateBuffCancelPure(MAX_EFFECTID + 1), "BuffNo out of range: 815 not in [0, 814]", "BUFFCANCEL above max validation") && ok;
    ok      = expectValid(validateSubmapChangePure(static_cast<std::uint16_t>(GP_CLI_COMMAND_SUBMAPCHANGE_STATE::General)), "SUBMAPCHANGE general validation") && ok;
    ok      = expectValid(validateSubmapChangePure(static_cast<std::uint16_t>(GP_CLI_COMMAND_SUBMAPCHANGE_STATE::Event)), "SUBMAPCHANGE event validation") && ok;
    ok      = expectInvalidError(validateSubmapChangePure(0), "State out of range: 0 not in [1, 2]", "SUBMAPCHANGE below range validation") && ok;
    ok      = expectInvalidError(validateSubmapChangePure(3), "State out of range: 3 not in [1, 2]", "SUBMAPCHANGE above range validation") && ok;
    return ok;
}

} // namespace

auto runC2SRescueBuffSubmapPacketSelfTests() -> bool
{
    return testRescueBuffSubmapLayoutsAndMetadata() &&
           testRescueBuffSubmapEncodedBytesAndPayloads() &&
           testRescueBuffSubmapValidation();
}
