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

#include "test_c2s_fishing_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x066_fishing.h"
#include "map/packets/c2s/0x110_fishing_2.h"

namespace
{

constexpr auto fishingUniqueNoOffset  = 4U;
constexpr auto fishingParaOffset      = 8U;
constexpr auto fishingActIndexOffset  = 12U;
constexpr auto fishingModeOffset      = 14U;
constexpr auto fishingUnknown00Offset = 15U;
constexpr auto fishingPara2Offset     = 16U;

using FishingBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_FISHING_2)>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s fishing packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s fishing packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s fishing packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s fishing packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s fishing packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s fishing packet self-test failed: " << label << " got";
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

auto makeFishingPacket(PacketC2S packetId) -> GP_CLI_COMMAND_FISHING_2
{
    auto packet         = GP_CLI_COMMAND_FISHING_2{};
    packet.header.id   = static_cast<std::uint16_t>(packetId);
    packet.header.size = sizeof(packet) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.para        = 0x01020304;
    packet.ActIndex    = 0x5566;
    packet.mode        = static_cast<std::int8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame);
    packet.unknown00   = 0x77;
    packet.para2       = 0x10203040;
    return packet;
}

auto makeExpectedFishingBytes(std::uint8_t typeByte, std::uint8_t sizeByte) -> FishingBytes
{
    auto bytes = FishingBytes{};
    bytes[0] = typeByte;
    bytes[1] = sizeByte;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[fishingUniqueNoOffset] = 0x44;
    bytes[fishingUniqueNoOffset + 1U] = 0x33;
    bytes[fishingUniqueNoOffset + 2U] = 0x22;
    bytes[fishingUniqueNoOffset + 3U] = 0x11;
    bytes[fishingParaOffset] = 0x04;
    bytes[fishingParaOffset + 1U] = 0x03;
    bytes[fishingParaOffset + 2U] = 0x02;
    bytes[fishingParaOffset + 3U] = 0x01;
    bytes[fishingActIndexOffset] = 0x66;
    bytes[fishingActIndexOffset + 1U] = 0x55;
    bytes[fishingModeOffset] = static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame);
    bytes[fishingUnknown00Offset] = 0x77;
    bytes[fishingPara2Offset] = 0x40;
    bytes[fishingPara2Offset + 1U] = 0x30;
    bytes[fishingPara2Offset + 2U] = 0x20;
    bytes[fishingPara2Offset + 3U] = 0x10;
    return bytes;
}

auto validateFishingPure(std::int32_t para, std::uint8_t mode, std::int32_t para2) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf<GP_CLI_COMMAND_FISHING_2_MODE>(mode)
        .custom([&](PacketValidator& v)
                {
                    switch (static_cast<GP_CLI_COMMAND_FISHING_2_MODE>(mode))
                    {
                        case GP_CLI_COMMAND_FISHING_2_MODE::RequestCheckHook:
                            v.mustEqual(para, 0, "para must be 0")
                                .mustEqual(para2, 0, "para2 must be 0");
                            break;
                        case GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame:
                            v.range("para", para, 0, 300);
                            break;
                        case GP_CLI_COMMAND_FISHING_2_MODE::RequestRelease:
                            v.mustEqual(para, 0, "para must be 0")
                                .mustEqual(para2, 0, "para2 must be 0");
                            break;
                        case GP_CLI_COMMAND_FISHING_2_MODE::RequestPotentialTimeout:
                            v.range("para", para, 0, 10)
                                .mustEqual(para2, 0, "para2 must be 0");
                            break;
                    }
                });
    return validator;
}

auto testFishingLayoutsAndMetadata() -> bool
{
    bool ok = true;

    const auto packet = GP_CLI_COMMAND_FISHING_2{};

    ok = expectEqualString(GP_CLI_COMMAND_FISHING_2::name, "GP_CLI_COMMAND_FISHING_2", "FISHING_2 name") && ok;
    ok = expectEqualString(GP_CLI_COMMAND_FISHING::name, "GP_CLI_COMMAND_FISHING_2", "FISHING alias inherited name") && ok;
    ok = expectEqualUInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FISHING_2::packetId), 0x110, "FISHING_2 packet id") && ok;
    ok = expectEqualUInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_FISHING::packetId), 0x110, "FISHING alias inherited packet id") && ok;
    ok = expectEqualUInt(static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_FISHING), 0x066, "FISHING enum value") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_COMMAND_FISHING_2), 20, "FISHING_2 sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_COMMAND_FISHING), sizeof(GP_CLI_COMMAND_FISHING_2), "FISHING alias sizeof") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_FISHING_2, UniqueNo), fishingUniqueNoOffset, "UniqueNo offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_FISHING_2, para), fishingParaOffset, "para offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_FISHING_2, ActIndex), fishingActIndexOffset, "ActIndex offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_FISHING_2, mode), fishingModeOffset, "mode offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_FISHING_2, unknown00), fishingUnknown00Offset, "unknown00 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_FISHING_2, para2), fishingPara2Offset, "para2 offset") && ok;
    ok = expectEqualUInt(sizeof(packet.mode), 1, "mode size") && ok;

    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestCheckHook), 2, "RequestCheckHook") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame), 3, "RequestEndMiniGame") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestRelease), 4, "RequestRelease") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestPotentialTimeout), 5, "RequestPotentialTimeout") && ok;

    return ok;
}

auto testFishingEncodedBytes() -> bool
{
    bool ok = true;
    ok      = expectBytes(encodedPacketBytes(makeFishingPacket(PacketC2S::GP_CLI_COMMAND_FISHING)),
                          makeExpectedFishingBytes(0x66, 0x0A),
                          "FISHING legacy packet bytes") &&
         ok;
    ok = expectBytes(encodedPacketBytes(makeFishingPacket(PacketC2S::GP_CLI_COMMAND_FISHING_2)),
                     makeExpectedFishingBytes(0x10, 0x0B),
                     "FISHING_2 packet bytes") &&
         ok;
    return ok;
}

auto testFishingValidation() -> bool
{
    bool ok = true;

    ok = expectValid(validateFishingPure(0, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestCheckHook), 0), "check hook") && ok;
    ok = expectInvalidError(validateFishingPure(1, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestCheckHook), 0), "para must be 0", "check hook para") && ok;
    ok = expectInvalidError(validateFishingPure(0, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestCheckHook), 1), "para2 must be 0", "check hook para2") && ok;
    ok = expectValid(validateFishingPure(300, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame), 1234), "end mini game max") && ok;
    ok = expectInvalidError(validateFishingPure(-1, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame), 0), "para out of range: -1 not in [0, 300]", "end mini game para below") && ok;
    ok = expectInvalidError(validateFishingPure(301, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestEndMiniGame), 0), "para out of range: 301 not in [0, 300]", "end mini game para above") && ok;
    ok = expectValid(validateFishingPure(0, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestRelease), 0), "release") && ok;
    ok = expectInvalidError(validateFishingPure(1, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestRelease), 0), "para must be 0", "release para") && ok;
    ok = expectInvalidError(validateFishingPure(0, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestRelease), 1), "para2 must be 0", "release para2") && ok;
    ok = expectValid(validateFishingPure(10, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestPotentialTimeout), 0), "potential timeout") && ok;
    ok = expectInvalidError(validateFishingPure(11, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestPotentialTimeout), 0), "para out of range: 11 not in [0, 10]", "potential timeout para") && ok;
    ok = expectInvalidError(validateFishingPure(10, static_cast<std::uint8_t>(GP_CLI_COMMAND_FISHING_2_MODE::RequestPotentialTimeout), 1), "para2 must be 0", "potential timeout para2") && ok;
    ok = expectInvalidError(validateFishingPure(11, 1, 1), "1 not a valid GP_CLI_COMMAND_FISHING_2_MODE value.", "invalid mode first") && ok;

    return ok;
}

} // namespace

auto runC2SFishingPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testFishingLayoutsAndMetadata() && ok;
    ok      = testFishingEncodedBytes() && ok;
    ok      = testFishingValidation() && ok;
    return ok;
}
