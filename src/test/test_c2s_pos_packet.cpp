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

#include "test_c2s_pos_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/entities/base_entity.h"
#include "map/packets/c2s/0x015_pos.h"

namespace
{

using PacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_POS)>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s POS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualFloat(float actual, float expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s POS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s POS packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s POS packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s POS packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const PacketBytes& actual, const PacketBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s POS packet self-test failed: " << label << " got";
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

auto makePacket() -> GP_CLI_COMMAND_POS
{
    auto packet         = GP_CLI_COMMAND_POS{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_POS);
    packet.header.size = sizeof(GP_CLI_COMMAND_POS) / 4;
    packet.header.sync = 0xBEEF;
    packet.x           = 1.25F;
    packet.z           = -2.5F;
    packet.y           = 3.75F;
    packet.MovTime     = 0x1122;
    packet.MoveFlame   = 0x3344;
    packet.dir         = -5;
    packet.TargetMode  = 1;
    packet.RunMode     = 0;
    packet.GroundMode  = 1;
    packet.unused      = 0x1F;
    packet.facetarget  = 0x5566;
    packet.TimeNow     = 0x778899AA;
    packet.padding00   = 0xCCDDEEFF;
    return packet;
}

auto encodedPacket() -> PacketBytes
{
    const auto packet = makePacket();

    auto bytes = PacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto validatePosStatus(STATUS_TYPE status) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .mustNotEqual(status, STATUS_TYPE::DISAPPEAR, "Character is disappearing")
        .mustNotEqual(status, STATUS_TYPE::SHUTDOWN, "Character is shutting down");
    return validator;
}

auto testLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_POS::name, "GP_CLI_COMMAND_POS", "static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_POS::packetId == PacketC2S::GP_CLI_COMMAND_POS, "static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_POS), 32, "sizeof(GP_CLI_COMMAND_POS)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, header), 0, "header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, x), 4, "x offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, z), 8, "z offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, y), 12, "y offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, MovTime), 16, "MovTime offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, MoveFlame), 18, "MoveFlame offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, dir), 20, "dir offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, facetarget), 22, "facetarget offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, TimeNow), 24, "TimeNow offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_POS, padding00), 28, "padding00 offset") && ok;
    ok = expectEqualInt(encodedPacket().at(21), 0xFD, "mode bitfield byte") && ok;
    ok = expectBytes(encodedPacket(),
                     PacketBytes{ 0x15, 0x10, 0xEF, 0xBE,
                                  0x00, 0x00, 0xA0, 0x3F,
                                  0x00, 0x00, 0x20, 0xC0,
                                  0x00, 0x00, 0x70, 0x40,
                                  0x22, 0x11, 0x44, 0x33,
                                  0xFB, 0xFD, 0x66, 0x55,
                                  0xAA, 0x99, 0x88, 0x77,
                                  0xFF, 0xEE, 0xDD, 0xCC },
                     "encoded packet") &&
         ok;

    return ok;
}

auto testPayloadStorage() -> bool
{
    const auto packet = makePacket();

    bool ok = true;
    ok      = expectEqualFloat(packet.x, 1.25F, "x") && ok;
    ok      = expectEqualFloat(packet.z, -2.5F, "z") && ok;
    ok      = expectEqualFloat(packet.y, 3.75F, "y") && ok;
    ok      = expectEqualInt(packet.MovTime, 0x1122, "MovTime") && ok;
    ok      = expectEqualInt(packet.MoveFlame, 0x3344, "MoveFlame") && ok;
    ok      = expectEqualInt(packet.dir, -5, "dir") && ok;
    ok      = expectEqualInt(packet.TargetMode, 1, "TargetMode") && ok;
    ok      = expectEqualInt(packet.RunMode, 0, "RunMode") && ok;
    ok      = expectEqualInt(packet.GroundMode, 1, "GroundMode") && ok;
    ok      = expectEqualInt(packet.unused, 0x1F, "unused") && ok;
    ok      = expectEqualInt(packet.facetarget, 0x5566, "facetarget") && ok;
    ok      = expectEqualInt(packet.TimeNow, 0x778899AA, "TimeNow") && ok;
    ok      = expectEqualInt(packet.padding00, 0xCCDDEEFF, "padding00") && ok;
    return ok;
}

auto testValidation() -> bool
{
    bool ok = true;

    ok = expectValid(validatePosStatus(STATUS_TYPE::NORMAL), "normal status") && ok;
    ok = expectValid(validatePosStatus(STATUS_TYPE::UPDATE), "update status") && ok;
    ok = expectInvalidError(validatePosStatus(STATUS_TYPE::DISAPPEAR),
                            "Character is disappearing",
                            "disappear status") &&
         ok;
    ok = expectInvalidError(validatePosStatus(STATUS_TYPE::SHUTDOWN),
                            "Character is shutting down",
                            "shutdown status") &&
         ok;

    return ok;
}

} // namespace

auto runC2SPosPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testLayoutAndMetadata() && ok;
    ok = testPayloadStorage() && ok;
    ok = testValidation() && ok;

    return ok;
}
