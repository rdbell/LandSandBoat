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

#include "test_c2s_event_update_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x05b_eventend.h"
#include "map/packets/c2s/0x05c_eventendxzy.h"
#include "map/packets/c2s/0x060_passwards.h"

namespace
{

using EventEndBytes    = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EVENTEND)>;
using EventEndXZYBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EVENTENDXZY)>;
using PasswardsBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_PASSWARDS)>;
using PasswardsString  = std::array<std::uint8_t, 16>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s event update packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualFloat(float actual, float expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s event update packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s event update packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s event update packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s event update packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s event update packet self-test failed: " << label << " got";
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

auto makeEventEndPacket() -> GP_CLI_COMMAND_EVENTEND
{
    auto packet         = GP_CLI_COMMAND_EVENTEND{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EVENTEND);
    packet.header.size = sizeof(GP_CLI_COMMAND_EVENTEND) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.EndPara     = 0x55667788;
    packet.ActIndex    = 0x99AA;
    packet.Mode        = static_cast<std::uint16_t>(GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending);
    packet.EventNum    = 0xBBCC;
    packet.EventPara   = 0xDDEE;
    return packet;
}

auto encodedEventEndPacket() -> EventEndBytes
{
    const auto packet = makeEventEndPacket();

    auto bytes = EventEndBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto makeEventEndXZYPacket() -> GP_CLI_COMMAND_EVENTENDXZY
{
    auto packet         = GP_CLI_COMMAND_EVENTENDXZY{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EVENTENDXZY);
    packet.header.size = sizeof(GP_CLI_COMMAND_EVENTENDXZY) / 4;
    packet.header.sync = 0xBEEF;
    packet.x           = 1.25F;
    packet.y           = -2.5F;
    packet.z           = 3.75F;
    packet.UniqueNo    = 0x11223344;
    packet.EndPara     = 0x55667788;
    packet.EventNum    = 0x99AA;
    packet.EventPara   = 0xBBCC;
    packet.ActIndex    = 0xDDEE;
    packet.Mode        = 1;
    packet.dir         = -5;
    return packet;
}

auto encodedEventEndXZYPacket() -> EventEndXZYBytes
{
    const auto packet = makeEventEndXZYPacket();

    auto bytes = EventEndXZYBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto makePasswardsPacket() -> GP_CLI_COMMAND_PASSWARDS
{
    auto packet         = GP_CLI_COMMAND_PASSWARDS{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_PASSWARDS);
    packet.header.size = sizeof(GP_CLI_COMMAND_PASSWARDS) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x5566;
    packet.padding00   = 0x7788;

    const auto stringBytes = PasswardsString{
        0x00, 0x50, 0x41, 0x53,
        0x53, 0xFF, 0x20, 0x78,
        0x00, 0x01, 0x02, 0x03,
        0x7F, 0x80, 0xFE, 0x00,
    };
    std::memcpy(packet.String, stringBytes.data(), stringBytes.size());
    return packet;
}

auto encodedPasswardsPacket() -> PasswardsBytes
{
    const auto packet = makePasswardsPacket();

    auto bytes = PasswardsBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto passwardsStringBytes(const GP_CLI_COMMAND_PASSWARDS& packet) -> PasswardsString
{
    auto bytes = PasswardsString{};
    std::memcpy(bytes.data(), packet.String, bytes.size());
    return bytes;
}

auto validateEventEndMode(std::uint16_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_EVENTEND_MODE>(mode);
    return validator;
}

auto validateEventEndXZYMode(std::uint8_t mode) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(mode, 1, "Mode not 1");
    return validator;
}

auto validatePasswardsPureScalars() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto testEventEndLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_EVENTEND::name, "GP_CLI_COMMAND_EVENTEND", "EVENTEND static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_EVENTEND::packetId == PacketC2S::GP_CLI_COMMAND_EVENTEND, "EVENTEND static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_HEADER), 4, "sizeof(GP_CLI_HEADER)") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EVENTEND), 20, "sizeof(GP_CLI_COMMAND_EVENTEND)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, header), 0, "EVENTEND header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, UniqueNo), 4, "EVENTEND UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, EndPara), 8, "EVENTEND EndPara offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, ActIndex), 12, "EVENTEND ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, Mode), 14, "EVENTEND Mode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, EventNum), 16, "EVENTEND EventNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTEND, EventPara), 18, "EVENTEND EventPara offset") && ok;
    ok = expectBytes(encodedEventEndPacket(),
                     EventEndBytes{ 0x5B, 0x0A, 0xEF, 0xBE,
                                    0x44, 0x33, 0x22, 0x11,
                                    0x88, 0x77, 0x66, 0x55,
                                    0xAA, 0x99, 0x01, 0x00,
                                    0xCC, 0xBB, 0xEE, 0xDD },
                     "EVENTEND encoded packet") &&
         ok;

    return ok;
}

auto testEventEndPayloadStorage() -> bool
{
    const auto packet = makeEventEndPacket();

    bool ok = true;
    ok      = expectEqualInt(packet.UniqueNo, 0x11223344, "EVENTEND UniqueNo") && ok;
    ok      = expectEqualInt(packet.EndPara, 0x55667788, "EVENTEND EndPara") && ok;
    ok      = expectEqualInt(packet.ActIndex, 0x99AA, "EVENTEND ActIndex") && ok;
    ok      = expectEqualInt(packet.Mode, 1, "EVENTEND Mode") && ok;
    ok      = expectEqualInt(packet.EventNum, 0xBBCC, "EVENTEND EventNum") && ok;
    ok      = expectEqualInt(packet.EventPara, 0xDDEE, "EVENTEND EventPara") && ok;
    return ok;
}

auto testEventEndModeDomain() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EVENTEND_MODE::End), 0, "EVENTEND End enum") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending), 1, "EVENTEND UpdatePending enum") && ok;
    ok = expectValid(validateEventEndMode(0), "EVENTEND End validation") && ok;
    ok = expectValid(validateEventEndMode(1), "EVENTEND UpdatePending validation") && ok;
    ok = expectInvalidError(validateEventEndMode(2), "2 not a valid GP_CLI_COMMAND_EVENTEND_MODE value.", "EVENTEND invalid mode") && ok;
    ok = expectInvalidError(validateEventEndMode(0xFFFF), "65535 not a valid GP_CLI_COMMAND_EVENTEND_MODE value.", "EVENTEND max mode") && ok;
    return ok;
}

auto testEventEndXZYLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeEventEndXZYPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_EVENTENDXZY::name, "GP_CLI_COMMAND_EVENTENDXZY", "EVENTENDXZY static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_EVENTENDXZY::packetId == PacketC2S::GP_CLI_COMMAND_EVENTENDXZY, "EVENTENDXZY static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_EVENTENDXZY), 32, "sizeof(GP_CLI_COMMAND_EVENTENDXZY)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, header), 0, "EVENTENDXZY header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, x), 4, "EVENTENDXZY x offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, y), 8, "EVENTENDXZY y offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, z), 12, "EVENTENDXZY z offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, UniqueNo), 16, "EVENTENDXZY UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, EndPara), 20, "EVENTENDXZY EndPara offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, EventNum), 24, "EVENTENDXZY EventNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, EventPara), 26, "EVENTENDXZY EventPara offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, ActIndex), 28, "EVENTENDXZY ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, Mode), 30, "EVENTENDXZY Mode offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_EVENTENDXZY, dir), 31, "EVENTENDXZY dir offset") && ok;
    ok = expectBytes(encodedEventEndXZYPacket(),
                     EventEndXZYBytes{ 0x5C, 0x10, 0xEF, 0xBE,
                                       0x00, 0x00, 0xA0, 0x3F,
                                       0x00, 0x00, 0x20, 0xC0,
                                       0x00, 0x00, 0x70, 0x40,
                                       0x44, 0x33, 0x22, 0x11,
                                       0x88, 0x77, 0x66, 0x55,
                                       0xAA, 0x99, 0xCC, 0xBB,
                                       0xEE, 0xDD, 0x01, 0xFB },
                     "EVENTENDXZY encoded packet") &&
         ok;
    ok = expectEqualFloat(packet.x, 1.25F, "EVENTENDXZY x") && ok;
    ok = expectEqualFloat(packet.y, -2.5F, "EVENTENDXZY y") && ok;
    ok = expectEqualFloat(packet.z, 3.75F, "EVENTENDXZY z") && ok;
    ok = expectEqualInt(packet.UniqueNo, 0x11223344, "EVENTENDXZY UniqueNo") && ok;
    ok = expectEqualInt(packet.EndPara, 0x55667788, "EVENTENDXZY EndPara") && ok;
    ok = expectEqualInt(packet.EventNum, 0x99AA, "EVENTENDXZY EventNum") && ok;
    ok = expectEqualInt(packet.EventPara, 0xBBCC, "EVENTENDXZY EventPara") && ok;
    ok = expectEqualInt(packet.ActIndex, 0xDDEE, "EVENTENDXZY ActIndex") && ok;
    ok = expectEqualInt(packet.Mode, 1, "EVENTENDXZY Mode") && ok;
    ok = expectEqualInt(packet.dir, -5, "EVENTENDXZY dir") && ok;
    return ok;
}

auto testEventEndXZYModeDomain() -> bool
{
    bool ok = true;
    ok      = expectValid(validateEventEndXZYMode(1), "EVENTENDXZY mode one") && ok;
    ok      = expectInvalidError(validateEventEndXZYMode(0), "Mode not 1", "EVENTENDXZY mode zero") && ok;
    ok      = expectInvalidError(validateEventEndXZYMode(2), "Mode not 1", "EVENTENDXZY mode two") && ok;
    ok      = expectInvalidError(validateEventEndXZYMode(0xFF), "Mode not 1", "EVENTENDXZY mode max") && ok;
    return ok;
}

auto testPasswardsLayoutMetadataAndPayload() -> bool
{
    const auto packet = makePasswardsPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_PASSWARDS::name, "GP_CLI_COMMAND_PASSWARDS", "PASSWARDS static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_PASSWARDS::packetId == PacketC2S::GP_CLI_COMMAND_PASSWARDS, "PASSWARDS static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_PASSWARDS), 28, "sizeof(GP_CLI_COMMAND_PASSWARDS)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_PASSWARDS, header), 0, "PASSWARDS header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_PASSWARDS, UniqueNo), 4, "PASSWARDS UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_PASSWARDS, ActIndex), 8, "PASSWARDS ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_PASSWARDS, padding00), 10, "PASSWARDS padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_PASSWARDS, String), 12, "PASSWARDS String offset") && ok;
    ok = expectBytes(encodedPasswardsPacket(),
                     PasswardsBytes{ 0x60, 0x0E, 0xEF, 0xBE,
                                     0x44, 0x33, 0x22, 0x11,
                                     0x66, 0x55, 0x88, 0x77,
                                     0x00, 0x50, 0x41, 0x53,
                                     0x53, 0xFF, 0x20, 0x78,
                                     0x00, 0x01, 0x02, 0x03,
                                     0x7F, 0x80, 0xFE, 0x00 },
                     "PASSWARDS encoded packet") &&
         ok;
    ok = expectEqualInt(packet.UniqueNo, 0x11223344, "PASSWARDS UniqueNo") && ok;
    ok = expectEqualInt(packet.ActIndex, 0x5566, "PASSWARDS ActIndex") && ok;
    ok = expectEqualInt(packet.padding00, 0x7788, "PASSWARDS padding00") && ok;
    ok = expectBytes(passwardsStringBytes(packet),
                     PasswardsString{ 0x00, 0x50, 0x41, 0x53,
                                      0x53, 0xFF, 0x20, 0x78,
                                      0x00, 0x01, 0x02, 0x03,
                                      0x7F, 0x80, 0xFE, 0x00 },
                     "PASSWARDS raw String") &&
         ok;
    ok = expectValid(validatePasswardsPureScalars(), "PASSWARDS pure scalar validation") && ok;
    return ok;
}

} // namespace

auto runC2SEventUpdatePacketSelfTests() -> bool
{
    bool ok = true;

    ok = testEventEndLayoutAndMetadata() && ok;
    ok = testEventEndPayloadStorage() && ok;
    ok = testEventEndModeDomain() && ok;
    ok = testEventEndXZYLayoutMetadataAndPayload() && ok;
    ok = testEventEndXZYModeDomain() && ok;
    ok = testPasswardsLayoutMetadataAndPayload() && ok;

    return ok;
}
