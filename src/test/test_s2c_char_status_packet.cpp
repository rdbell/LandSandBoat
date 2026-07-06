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

#include "test_s2c_char_status_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "common/earth_time.h"
#include "entities/char_entity.h"
#include "map/packets/char_status.h"

namespace
{

constexpr auto charStatusPacketSize             = 0x60;
constexpr auto charStatusIconsOffset            = 0x04;
constexpr auto charStatusUniqueNoOffset         = 0x24;
constexpr auto charStatusFlags0Offset           = 0x28;
constexpr auto charStatusFlags1Offset           = 0x2C;
constexpr auto charStatusServerStatusOffset     = 0x30;
constexpr auto charStatusLinkshellColorOffset   = 0x31;
constexpr auto charStatusFlags2Offset           = 0x34;
constexpr auto charStatusFlags3Offset           = 0x38;
constexpr auto charStatusDeadCounter1Offset     = 0x3C;
constexpr auto charStatusDeadCounter2Offset     = 0x40;
constexpr auto charStatusCostumeIDOffset        = 0x44;
constexpr auto charStatusWarpTargetIndexOffset  = 0x46;
constexpr auto charStatusFellowTargetIndexOffset = 0x48;
constexpr auto charStatusFishingTimerOffset     = 0x4A;
constexpr auto charStatusPadding00Offset        = 0x4B;
constexpr auto charStatusBitsOffset             = 0x4C;
constexpr auto charStatusMonstrosityInfoOffset  = 0x54;
constexpr auto charStatusMonstrosityName1Offset = 0x56;
constexpr auto charStatusMonstrosityName2Offset = 0x57;
constexpr auto charStatusFlags4Offset           = 0x58;
constexpr auto charStatusModelHitboxSizeOffset  = 0x59;
constexpr auto charStatusFlags5Offset           = 0x5A;
constexpr auto charStatusMountIDOffset          = 0x5B;
constexpr auto charStatusFlags6Offset           = 0x5C;
constexpr auto charStatusDefaultDeadSeconds     = 66 * 60;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

class EmptySelectDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>&) -> std::unique_ptr<db::ResultSet> override
    {
        auto schema   = std::make_shared<db::ColumnSchema>();
        schema->names = { "mentor", "days_since_logout", "muted" };
        for (std::size_t i = 0; i < schema->names.size(); ++i)
        {
            schema->index[schema->names[i]] = i;
        }
        return std::make_unique<db::LibMariaDBResultSet>(query, schema, std::vector<db::LibMariaDBResultSet::Row>{});
    }

    auto getSchema() -> std::string override
    {
        return "xidb";
    }

    auto getVersion() -> std::string override
    {
        return "test";
    }

    auto getDriverVersion() -> std::string override
    {
        return "test";
    }
};

void populateCharacter(CCharEntity& entity)
{
    entity.id             = 0x11223344;
    entity.animation      = ANIMATION_FISHING_START;
    entity.hookDelay      = 17;
    entity.health.hp      = 750;
    entity.health.modhp   = 1000;
    entity.look.size      = 2;
    entity.look.race      = 5;
    entity.baseSpeed      = 40;
    entity.animationSpeed = 22;
    entity.m_Costume      = 0x3344;
    entity.modelHitboxSize = 2.5f;
    entity.visibleGmLevel  = 5;
    entity.wallhackEnabled = true;
    entity.isFrozenFlagged = true;
    entity.isLinkDead      = true;
    entity.isCharmed       = true;
    entity.priorityRender  = true;
    entity.m_isGMHidden    = true;
    entity.allegiance      = ALLEGIANCE_TYPE::GRIFFONS;

    entity.playerConfig.InviteFlg    = 1;
    entity.playerConfig.AwayFlg      = 1;
    entity.playerConfig.AnonymityFlg = 1;
    entity.playerConfig.MentorFlg    = 1;
}

auto readLE32(CBasicPacket& packet, std::size_t offset) -> std::uint32_t
{
    std::uint32_t value = 0;
    std::memcpy(&value, packetData(packet) + offset, sizeof(value));
    return value;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CHAR_STATUS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectRangeUInt(std::uint64_t actual, std::uint64_t min, std::uint64_t max, const std::string& label) -> bool
{
    if (actual < min || actual > max)
    {
        std::cerr << "s2c CHAR_STATUS packet self-test failed: " << label << " got " << actual << " expected range [" << min << ", " << max << "]\n";
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c CHAR_STATUS packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
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

auto expectFilledRange(CBasicPacket& packet, std::size_t offset, std::size_t end, uint8 expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != expected)
        {
            std::cerr << "s2c CHAR_STATUS packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    return expectFilledRange(packet, offset, end, 0, label);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(charStatusPacketSize, 96, "packet size") && ok;
    ok      = expectEqualUInt(charStatusPacketSize - sizeof(GP_SERV_HEADER), 92, "packet data size") && ok;
    ok      = expectEqualUInt(charStatusIconsOffset, 4, "icons offset") && ok;
    ok      = expectEqualUInt(charStatusUniqueNoOffset, 36, "unique no offset") && ok;
    ok      = expectEqualUInt(charStatusFlags0Offset, 40, "flags0 offset") && ok;
    ok      = expectEqualUInt(charStatusFlags1Offset, 44, "flags1 offset") && ok;
    ok      = expectEqualUInt(charStatusServerStatusOffset, 48, "server status offset") && ok;
    ok      = expectEqualUInt(charStatusLinkshellColorOffset, 49, "linkshell color offset") && ok;
    ok      = expectEqualUInt(charStatusFlags2Offset, 52, "flags2 offset") && ok;
    ok      = expectEqualUInt(charStatusFlags3Offset, 56, "flags3 offset") && ok;
    ok      = expectEqualUInt(charStatusDeadCounter1Offset, 60, "dead counter1 offset") && ok;
    ok      = expectEqualUInt(charStatusDeadCounter2Offset, 64, "dead counter2 offset") && ok;
    ok      = expectEqualUInt(charStatusCostumeIDOffset, 68, "costume offset") && ok;
    ok      = expectEqualUInt(charStatusWarpTargetIndexOffset, 70, "warp target offset") && ok;
    ok      = expectEqualUInt(charStatusFellowTargetIndexOffset, 72, "fellow target offset") && ok;
    ok      = expectEqualUInt(charStatusFishingTimerOffset, 74, "fishing timer offset") && ok;
    ok      = expectEqualUInt(charStatusPadding00Offset, 75, "padding offset") && ok;
    ok      = expectEqualUInt(charStatusBitsOffset, 76, "status bits offset") && ok;
    ok      = expectEqualUInt(charStatusMonstrosityInfoOffset, 84, "monstrosity info offset") && ok;
    ok      = expectEqualUInt(charStatusMonstrosityName1Offset, 86, "monstrosity name1 offset") && ok;
    ok      = expectEqualUInt(charStatusMonstrosityName2Offset, 87, "monstrosity name2 offset") && ok;
    ok      = expectEqualUInt(charStatusFlags4Offset, 88, "flags4 offset") && ok;
    ok      = expectEqualUInt(charStatusModelHitboxSizeOffset, 89, "model hitbox offset") && ok;
    ok      = expectEqualUInt(charStatusFlags5Offset, 90, "flags5 offset") && ok;
    ok      = expectEqualUInt(charStatusMountIDOffset, 91, "mount id offset") && ok;
    ok      = expectEqualUInt(charStatusFlags6Offset, 92, "flags6 offset") && ok;
    return ok;
}

auto testConstructorFields() -> bool
{
    auto entity = CCharEntity{};
    populateCharacter(entity);

    EmptySelectDatabase fakeDatabase;
    db::setDatabase(&fakeDatabase);
    const auto timestampBefore = earth_time::vanadiel_timestamp();
    auto       packet          = CCharStatusPacket(&entity);
    const auto timestampAfter  = earth_time::vanadiel_timestamp();
    db::setDatabase(nullptr);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x037, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), charStatusPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x37, 0x30, 0xEF, 0xBE }, "header") && ok;
    ok      = expectFilledRange(packet, charStatusIconsOffset, charStatusUniqueNoOffset, 0xFF, "default status icons") && ok;
    ok      = expectBytes(packet, charStatusUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "unique no") && ok;
    ok      = expectBytes(packet, charStatusFlags0Offset, std::array<uint8, 4>{ 0xB0, 0x11, 0x4B, 0xA4 }, "flags0") && ok;
    ok      = expectBytes(packet, charStatusFlags1Offset, std::array<uint8, 4>{ 0x28, 0xB0, 0x2C, 0x40 }, "flags1") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusServerStatusOffset], ANIMATION_FISHING_START, "server status") && ok;
    ok      = expectBytes(packet, charStatusLinkshellColorOffset, std::array<uint8, 3>{ 0x00, 0x00, 0x00 }, "linkshell color") && ok;
    ok      = expectBytes(packet, charStatusFlags2Offset, std::array<uint8, 4>{ 0x00, 0x00, 0xD0, 0x00 }, "flags2") && ok;
    ok      = expectBytes(packet, charStatusFlags3Offset, std::array<uint8, 4>{ 0x1C, 0x06, 0x00, 0x00 }, "flags3") && ok;
    ok      = expectBytes(packet, charStatusDeadCounter1Offset, std::array<uint8, 4>{ 0x20, 0xA0, 0x03, 0x00 }, "dead counter1") && ok;
    ok      = expectRangeUInt(readLE32(packet, charStatusDeadCounter2Offset), timestampBefore + charStatusDefaultDeadSeconds, timestampAfter + charStatusDefaultDeadSeconds, "dead counter2") && ok;
    ok      = expectBytes(packet, charStatusCostumeIDOffset, std::array<uint8, 2>{ 0x44, 0x33 }, "costume id") && ok;
    ok      = expectZeroRange(packet, charStatusWarpTargetIndexOffset, charStatusFishingTimerOffset, "warp and fellow targets") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusFishingTimerOffset], 17, "fishing timer") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusPadding00Offset], 0, "padding00") && ok;
    ok      = expectZeroRange(packet, charStatusBitsOffset, charStatusMonstrosityInfoOffset, "status bits") && ok;
    ok      = expectZeroRange(packet, charStatusMonstrosityInfoOffset, charStatusFlags4Offset, "monstrosity fields") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusFlags4Offset], 0x10, "flags4") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusModelHitboxSizeOffset], 25, "model hitbox size") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusFlags5Offset], 0, "flags5") && ok;
    ok      = expectEqualUInt(packetData(packet)[charStatusMountIDOffset], 0, "mount id") && ok;
    ok      = expectBytes(packet, charStatusFlags6Offset, std::array<uint8, 4>{ 0x7B, 0x00, 0x00, 0x00 }, "flags6") && ok;
    ok      = expectZeroRange(packet, charStatusPacketSize, PACKET_SIZE, "packet tail") && ok;
    return ok;
}

} // namespace

auto runS2CCharStatusPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructorFields() && ok;
    return ok;
}
