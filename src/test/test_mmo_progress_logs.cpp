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

#include "test_mmo_progress_logs.h"

#include "common/mmo.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo progress logs self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo progress logs self-test failed: " << label << " got";
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

template <typename T>
auto bytesOf(const T& value) -> std::array<std::uint8_t, sizeof(T)>
{
    std::array<std::uint8_t, sizeof(T)> bytes{};
    std::memcpy(bytes.data(), &value, bytes.size());
    return bytes;
}

template <typename Struct, typename Field>
auto fieldOffset(const Struct& value, const Field& field) -> std::uint64_t
{
    return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&field) - reinterpret_cast<std::uintptr_t>(&value));
}

auto questStatus(const questlog_t& log, std::uint16_t questID) -> std::uint8_t
{
    const auto current  = log.current[questID / 8] & (1 << (questID % 8));
    const auto complete = log.complete[questID / 8] & (1 << (questID % 8));
    return complete != 0 ? 2 : (current != 0 ? 1 : 0);
}

auto testQuestLogLayoutAndBytes() -> bool
{
    bool ok = true;

    questlog_t log{};
    ok = expectEqualUInt(sizeof(questlog_t), 64, "questlog_t sizeof") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.current), 0, "questlog_t current offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.complete), 32, "questlog_t complete offset") && ok;

    log.current[0]    = 0x01;
    log.current[31]   = 0x80;
    log.complete[1]   = 0x04;
    log.complete[31]  = 0x80;

    std::array<std::uint8_t, sizeof(questlog_t)> expected{};
    expected[0]       = 0x01;
    expected[31]      = 0x80;
    expected[32 + 1]  = 0x04;
    expected[32 + 31] = 0x80;

    ok = expectBytes(bytesOf(log), expected, "questlog_t golden bytes") && ok;
    ok = expectEqualUInt(questStatus(log, 0), 1, "quest status current") && ok;
    ok = expectEqualUInt(questStatus(log, 10), 2, "quest status complete") && ok;
    ok = expectEqualUInt(questStatus(log, 255), 2, "quest status complete overrides current") && ok;
    ok = expectEqualUInt(questStatus(log, 12), 0, "quest status absent") && ok;
    return ok;
}

auto testMissionLogLayoutAndBytes() -> bool
{
    bool ok = true;

    missionlog_t log{};
    ok = expectEqualUInt(sizeof(missionlog_t), 70, "missionlog_t sizeof") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.current), 0, "missionlog_t current offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.statusUpper), 2, "missionlog_t statusUpper offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.statusLower), 4, "missionlog_t statusLower offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.complete), 6, "missionlog_t complete offset") && ok;

    log.current     = 0x1234;
    log.statusUpper = 0xABCD;
    log.statusLower = 0x5678;
    log.complete[0] = true;
    log.complete[63] = true;

    std::array<std::uint8_t, sizeof(missionlog_t)> expected{};
    expected[0]     = 0x34;
    expected[1]     = 0x12;
    expected[2]     = 0xCD;
    expected[3]     = 0xAB;
    expected[4]     = 0x78;
    expected[5]     = 0x56;
    expected[6]     = 0x01;
    expected[6 + 63] = 0x01;

    const auto status = (static_cast<std::uint32_t>(log.statusUpper) << 16) | log.statusLower;
    ok = expectEqualUInt(status, 0xABCD5678, "missionlog_t combined status") && ok;
    ok = expectBytes(bytesOf(log), expected, "missionlog_t golden bytes") && ok;
    return ok;
}

auto testAssaultLogLayoutAndBytes() -> bool
{
    bool ok = true;

    assaultlog_t log{};
    ok = expectEqualUInt(sizeof(assaultlog_t), 130, "assaultlog_t sizeof") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.current), 0, "assaultlog_t current offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.complete), 2, "assaultlog_t complete offset") && ok;

    log.current       = 0x00FE;
    log.complete[0]   = true;
    log.complete[127] = true;

    std::array<std::uint8_t, sizeof(assaultlog_t)> expected{};
    expected[0]       = 0xFE;
    expected[1]       = 0x00;
    expected[2]       = 0x01;
    expected[2 + 127] = 0x01;

    ok = expectBytes(bytesOf(log), expected, "assaultlog_t golden bytes") && ok;
    return ok;
}

auto testCampaignLogLayoutAndBytes() -> bool
{
    bool ok = true;

    campaignlog_t log{};
    ok = expectEqualUInt(sizeof(campaignlog_t), 514, "campaignlog_t sizeof") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.current), 0, "campaignlog_t current offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.complete), 2, "campaignlog_t complete offset") && ok;

    log.current       = 0x0201;
    log.complete[0]   = true;
    log.complete[256] = true;
    log.complete[511] = true;

    std::array<std::uint8_t, sizeof(campaignlog_t)> expected{};
    expected[0]       = 0x01;
    expected[1]       = 0x02;
    expected[2]       = 0x01;
    expected[2 + 256] = 0x01;
    expected[2 + 511] = 0x01;

    ok = expectBytes(bytesOf(log), expected, "campaignlog_t golden bytes") && ok;
    return ok;
}

} // namespace

auto runMMOProgressLogSelfTests() -> bool
{
    return testQuestLogLayoutAndBytes() &&
           testMissionLogLayoutAndBytes() &&
           testAssaultLogLayoutAndBytes() &&
           testCampaignLogLayoutAndBytes();
}
