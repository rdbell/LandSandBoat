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

#include "test_c2s_tracking_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0f4_tracking_list.h"
#include "map/packets/c2s/0x0f5_tracking_start.h"
#include "map/packets/c2s/0x0f6_tracking_end.h"

namespace
{

using TrackingListBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRACKING_LIST)>;
using TrackingStartBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRACKING_START)>;
using TrackingEndBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRACKING_END)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s tracking packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s tracking packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s tracking packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s tracking packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s tracking packet self-test failed: " << label << " got";
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

auto validateTrackingListPure(std::uint32_t sendFlg) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(sendFlg, 1, "SendFlg not equal to 1");
    return validator;
}

auto validateTrackingStartPure(std::uint32_t actIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("ActIndex", actIndex, 0x1, 0x1000);
    return validator;
}

auto validateTrackingEndPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto makeTrackingListPacket() -> GP_CLI_COMMAND_TRACKING_LIST
{
    auto packet         = GP_CLI_COMMAND_TRACKING_LIST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRACKING_LIST);
    packet.header.size = sizeof(GP_CLI_COMMAND_TRACKING_LIST) / 4U;
    packet.header.sync = 0xBEEF;
    packet.SendFlg     = 1;
    return packet;
}

auto makeTrackingStartPacket() -> GP_CLI_COMMAND_TRACKING_START
{
    auto packet         = GP_CLI_COMMAND_TRACKING_START{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRACKING_START);
    packet.header.size = sizeof(GP_CLI_COMMAND_TRACKING_START) / 4U;
    packet.header.sync = 0xBEEF;
    packet.ActIndex    = 0x1000;
    return packet;
}

auto makeTrackingEndPacket() -> GP_CLI_COMMAND_TRACKING_END
{
    auto packet         = GP_CLI_COMMAND_TRACKING_END{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRACKING_END);
    packet.header.size = sizeof(GP_CLI_COMMAND_TRACKING_END) / 4U;
    packet.header.sync = 0xBEEF;
    packet.padding00   = 0xAABBCCDD;
    return packet;
}

auto testTrackingLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_TRACKING_LIST::name, "GP_CLI_COMMAND_TRACKING_LIST", "TRACKING_LIST name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_TRACKING_LIST::packetId), 0x0F4, "TRACKING_LIST packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRACKING_LIST), 8, "TRACKING_LIST sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRACKING_LIST, SendFlg), 4, "TRACKING_LIST SendFlg offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_TRACKING_START::name, "GP_CLI_COMMAND_TRACKING_START", "TRACKING_START name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_TRACKING_START::packetId), 0x0F5, "TRACKING_START packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRACKING_START), 8, "TRACKING_START sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRACKING_START, ActIndex), 4, "TRACKING_START ActIndex offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_TRACKING_END::name, "GP_CLI_COMMAND_TRACKING_END", "TRACKING_END name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_TRACKING_END::packetId), 0x0F6, "TRACKING_END packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRACKING_END), 8, "TRACKING_END sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRACKING_END, padding00), 4, "TRACKING_END padding00 offset") && ok;
    return ok;
}

auto testTrackingEncodedBytesAndPayloads() -> bool
{
    bool ok = true;
    ok      = expectBytes(encodedPacketBytes(makeTrackingListPacket()), TrackingListBytes{ 0xF4, 0x04, 0xEF, 0xBE, 0x01, 0x00, 0x00, 0x00 }, "TRACKING_LIST encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeTrackingStartPacket()), TrackingStartBytes{ 0xF5, 0x04, 0xEF, 0xBE, 0x00, 0x10, 0x00, 0x00 }, "TRACKING_START encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeTrackingEndPacket()), TrackingEndBytes{ 0xF6, 0x04, 0xEF, 0xBE, 0xDD, 0xCC, 0xBB, 0xAA }, "TRACKING_END encoded packet") && ok;
    return ok;
}

auto testTrackingValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateTrackingListPure(1), "TRACKING_LIST SendFlg validation") && ok;
    ok      = expectInvalidError(validateTrackingListPure(0), "SendFlg not equal to 1", "TRACKING_LIST zero validation") && ok;
    ok      = expectInvalidError(validateTrackingListPure(2), "SendFlg not equal to 1", "TRACKING_LIST two validation") && ok;
    ok      = expectValid(validateTrackingStartPure(1), "TRACKING_START min validation") && ok;
    ok      = expectValid(validateTrackingStartPure(0x1000), "TRACKING_START max validation") && ok;
    ok      = expectInvalidError(validateTrackingStartPure(0), "ActIndex out of range: 0 not in [1, 4096]", "TRACKING_START below range validation") && ok;
    ok      = expectInvalidError(validateTrackingStartPure(0x1001), "ActIndex out of range: 4097 not in [1, 4096]", "TRACKING_START above range validation") && ok;
    ok      = expectValid(validateTrackingEndPure(), "TRACKING_END pure validation") && ok;
    return ok;
}

} // namespace

auto runC2STrackingPacketSelfTests() -> bool
{
    return testTrackingLayoutsAndMetadata() &&
           testTrackingEncodedBytesAndPayloads() &&
           testTrackingValidation();
}
