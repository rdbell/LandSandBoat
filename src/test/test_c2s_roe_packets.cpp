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

#include "test_c2s_roe_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x10c_roe_start.h"
#include "map/packets/c2s/0x10d_roe_remove.h"
#include "map/packets/c2s/0x10e_roe_claim.h"

namespace
{

using ROEStartBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ROE_START)>;
using ROERemoveBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ROE_REMOVE)>;
using ROEClaimBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_ROE_CLAIM)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s roe packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s roe packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s roe packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s roe packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s roe packet self-test failed: " << label << " got";
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

auto validateROEObjectiveIdPure(std::uint16_t objectiveId) -> PacketValidationResult
{
    // The full packet validators require live CCharEntity, settings, and RoE state.
    // This helper pins the portable scalar subset shared by the three packet validators.
    auto validator = PacketValidator(nullptr);
    validator.range("ObjectiveId", objectiveId, 0, 4096);
    return validator;
}

template <typename Packet>
auto makeROEPacket(PacketC2S packetId) -> Packet
{
    auto packet         = Packet{};
    packet.header.id   = static_cast<std::uint16_t>(packetId);
    packet.header.size = sizeof(Packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.ObjectiveId = 0x1122;
    packet.padding00   = 0x3344;
    return packet;
}

auto testROELayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_ROE_START::name, "GP_CLI_COMMAND_ROE_START", "ROE_START name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_ROE_REMOVE::name, "GP_CLI_COMMAND_ROE_REMOVE", "ROE_REMOVE name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_ROE_CLAIM::name, "GP_CLI_COMMAND_ROE_CLAIM", "ROE_CLAIM name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_ROE_START::packetId), 0x10C, "ROE_START packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_ROE_REMOVE::packetId), 0x10D, "ROE_REMOVE packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_ROE_CLAIM::packetId), 0x10E, "ROE_CLAIM packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_ROE_START), 8, "ROE_START sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_ROE_REMOVE), 8, "ROE_REMOVE sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_ROE_CLAIM), 8, "ROE_CLAIM sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_ROE_START, ObjectiveId), 4, "ROE_START ObjectiveId offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_ROE_START, padding00), 6, "ROE_START padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_ROE_REMOVE, ObjectiveId), 4, "ROE_REMOVE ObjectiveId offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_ROE_REMOVE, padding00), 6, "ROE_REMOVE padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_ROE_CLAIM, ObjectiveId), 4, "ROE_CLAIM ObjectiveId offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_ROE_CLAIM, padding00), 6, "ROE_CLAIM padding00 offset") && ok;
    return ok;
}

auto testROEEncodedBytes() -> bool
{
    bool ok = true;

    auto startExpected = ROEStartBytes{ 0x0C, 0x05, 0xEF, 0xBE, 0x22, 0x11, 0x44, 0x33 };
    ok                 = expectBytes(encodedPacketBytes(makeROEPacket<GP_CLI_COMMAND_ROE_START>(PacketC2S::GP_CLI_COMMAND_ROE_START)), startExpected, "ROE_START encoded packet") && ok;

    auto removeExpected = ROERemoveBytes{ 0x0D, 0x05, 0xEF, 0xBE, 0x22, 0x11, 0x44, 0x33 };
    ok                  = expectBytes(encodedPacketBytes(makeROEPacket<GP_CLI_COMMAND_ROE_REMOVE>(PacketC2S::GP_CLI_COMMAND_ROE_REMOVE)), removeExpected, "ROE_REMOVE encoded packet") && ok;

    auto claimExpected = ROEClaimBytes{ 0x0E, 0x05, 0xEF, 0xBE, 0x22, 0x11, 0x44, 0x33 };
    ok                 = expectBytes(encodedPacketBytes(makeROEPacket<GP_CLI_COMMAND_ROE_CLAIM>(PacketC2S::GP_CLI_COMMAND_ROE_CLAIM)), claimExpected, "ROE_CLAIM encoded packet") && ok;

    return ok;
}

auto testROEValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateROEObjectiveIdPure(0), "ROE ObjectiveId min validation") && ok;
    ok      = expectValid(validateROEObjectiveIdPure(4096), "ROE ObjectiveId max validation") && ok;
    ok      = expectInvalidError(validateROEObjectiveIdPure(4097), "ObjectiveId out of range: 4097 not in [0, 4096]", "ROE ObjectiveId above validation") && ok;
    return ok;
}

} // namespace

auto runC2SROEPacketSelfTests() -> bool
{
    return testROELayoutsAndMetadata() &&
           testROEEncodedBytes() &&
           testROEValidation();
}
