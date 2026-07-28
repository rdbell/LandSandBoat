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

#include "test_s2c_char_sync_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/job_points.h"
#include "map/merit.h"
#include "map/packets/char_sync.h"
#include "status_effect_container.h"

namespace
{

constexpr auto charSyncPacketSize                   = 0x28;
constexpr auto charSyncControlOffset                = 0x04;
constexpr auto charSyncControl2Offset               = 0x05;
constexpr auto charSyncTargIDOffset                 = 0x06;
constexpr auto charSyncEntityIDOffset               = 0x08;
constexpr auto charSyncFellowTargIDOffset           = 0x0C;
constexpr auto charSyncSyncFlagsOffset              = 0x10;
constexpr auto charSyncMountedSubPowerOffset        = 0x13;
constexpr auto charSyncMountedCustomProperty0Offset = 0x18;
constexpr auto charSyncMountedCustomProperty1Offset = 0x1C;
constexpr auto charSyncMainJobLevelOffset           = 0x25;
constexpr auto charSyncLevelRestrictionOffset       = 0x26;
constexpr auto charSyncMogExpansionOffset           = 0x27;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void populateChar(CCharEntity& entity, std::uint32_t id, std::uint16_t targid, std::uint8_t mainJobLevel)
{
    if (!entity.PJobPoints)
    {
        entity.PJobPoints = std::make_unique<CJobPoints>(&entity);
    }
    if (!entity.PMeritPoints)
    {
        entity.PMeritPoints = std::make_unique<CMeritPoints>(&entity);
    }
    entity.id                         = id;
    entity.targid                     = targid;
    entity.jobs.job[entity.GetMJob()] = mainJobLevel;
}

void addStatus(CCharEntity& entity, xi::StatusEffect effect)
{
    entity.StatusEffectContainer->AddStatusEffectSilent(effect, static_cast<uint16>(effect), 0, timer::duration::zero(), timer::duration::zero());
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CHAR_SYNC packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CHAR_SYNC packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c CHAR_SYNC packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(charSyncPacketSize, 40, "packet size") && ok;
    ok      = expectEqualUInt(charSyncControlOffset, 4, "control offset") && ok;
    ok      = expectEqualUInt(charSyncControl2Offset, 5, "control2 offset") && ok;
    ok      = expectEqualUInt(charSyncTargIDOffset, 6, "targid offset") && ok;
    ok      = expectEqualUInt(charSyncEntityIDOffset, 8, "entity id offset") && ok;
    ok      = expectEqualUInt(charSyncFellowTargIDOffset, 12, "fellow targid offset") && ok;
    ok      = expectEqualUInt(charSyncSyncFlagsOffset, 16, "sync flags offset") && ok;
    ok      = expectEqualUInt(charSyncMountedSubPowerOffset, 19, "mounted subpower offset") && ok;
    ok      = expectEqualUInt(charSyncMountedCustomProperty0Offset, 24, "mounted custom0 offset") && ok;
    ok      = expectEqualUInt(charSyncMountedCustomProperty1Offset, 28, "mounted custom1 offset") && ok;
    ok      = expectEqualUInt(charSyncMainJobLevelOffset, 37, "main job level offset") && ok;
    ok      = expectEqualUInt(charSyncLevelRestrictionOffset, 38, "level restriction offset") && ok;
    ok      = expectEqualUInt(charSyncMogExpansionOffset, 39, "mog expansion offset") && ok;
    return ok;
}

auto testConstructorBaseFields() -> bool
{
    auto entity = CCharEntity{};
    populateChar(entity, 0x11223344, 0x5566, 75);

    auto packet = CCharSyncPacket(&entity);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x067, "base type") && ok;
    ok      = expectEqualUInt(packet.getSize(), charSyncPacketSize, "base size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x67, 0x14, 0xEF, 0xBE }, "base header") && ok;
    ok      = expectBytes(packet, charSyncControlOffset, std::array<uint8, 2>{ 0x02, 0x09 }, "base controls") && ok;
    ok      = expectBytes(packet, charSyncTargIDOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "base targid") && ok;
    ok      = expectBytes(packet, charSyncEntityIDOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "base entity id") && ok;
    ok      = expectBytes(packet, charSyncFellowTargIDOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "base fellow slot") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncSyncFlagsOffset], 0, "base sync flags") && ok;
    ok      = expectZeroRange(packet, charSyncMountedSubPowerOffset, charSyncMainJobLevelOffset, "base mounted fields") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncMainJobLevelOffset], 75, "base main job level") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncLevelRestrictionOffset], 0, "base level restriction") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncMogExpansionOffset], 0, "base mog expansion") && ok;
    ok      = expectZeroRange(packet, charSyncPacketSize, PACKET_SIZE, "base packet tail") && ok;
    return ok;
}

auto testConstructorSyncFlags() -> bool
{
    auto entity = CCharEntity{};
    populateChar(entity, 0x01020304, 0x7788, 99);
    addStatus(entity, xi::StatusEffect::AlliedTags);
    addStatus(entity, xi::StatusEffect::LevelSync);
    entity.m_LevelRestriction = 30;
    entity.profile.mhflag     = 0x20;

    auto packet = CCharSyncPacket(&entity);

    bool ok = true;
    ok      = expectBytes(packet, charSyncTargIDOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "sync targid") && ok;
    ok      = expectBytes(packet, charSyncEntityIDOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "sync entity id") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncSyncFlagsOffset], 0x06, "sync flags") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncMainJobLevelOffset], 99, "sync main job level") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncLevelRestrictionOffset], 30, "sync level restriction") && ok;
    ok      = expectEqualUInt(packetData(packet)[charSyncMogExpansionOffset], 1, "sync mog expansion") && ok;
    return ok;
}

} // namespace

auto runS2CCharSyncPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructorBaseFields() && ok;
    ok      = testConstructorSyncFlags() && ok;
    return ok;
}
