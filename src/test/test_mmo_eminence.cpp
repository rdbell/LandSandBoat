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

#include "test_mmo_eminence.h"

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
        std::cerr << "mmo eminence self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBool(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo eminence self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo eminence self-test failed: " << label << " got";
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

auto testEminenceLogLayoutAndBytes() -> bool
{
    bool ok = true;

    eminencelog_t log{};
    ok = expectEqualUInt(sizeof(eminencelog_t), 700, "eminencelog_t sizeof") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.active), 0, "eminencelog_t active offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.progress), 64, "eminencelog_t progress offset") && ok;
    ok = expectEqualUInt(fieldOffset(log, log.complete), 188, "eminencelog_t complete offset") && ok;

    log.active[0]     = 0x1111;
    log.active[30]    = 0x2222;
    log.progress[0]   = 0xAABBCCDD;
    log.progress[30]  = 0x01020304;
    log.complete[0]   = 0x01;
    log.complete[127] = 0x80;
    log.complete[128] = 0x02;
    log.complete[511] = 0x80;

    std::array<std::uint8_t, sizeof(eminencelog_t)> expected{};
    expected[0]         = 0x11;
    expected[1]         = 0x11;
    expected[60]        = 0x22;
    expected[61]        = 0x22;
    expected[64]        = 0xDD;
    expected[65]        = 0xCC;
    expected[66]        = 0xBB;
    expected[67]        = 0xAA;
    expected[64 + 120]  = 0x04;
    expected[64 + 121]  = 0x03;
    expected[64 + 122]  = 0x02;
    expected[64 + 123]  = 0x01;
    expected[188]       = 0x01;
    expected[188 + 127] = 0x80;
    expected[188 + 128] = 0x02;
    expected[188 + 511] = 0x80;

    ok = expectBytes(bytesOf(log), expected, "eminencelog_t golden bytes") && ok;
    return ok;
}

auto testEminenceCacheDefaultsAndMutation() -> bool
{
    bool ok = true;

    eminencecache_t cache{};
    ok = expectEqualUInt(sizeof(cache.activemap), 512, "eminencecache_t activemap sizeof") && ok;
    ok = expectBool(cache.activemap.none(), true, "eminencecache_t default activemap none") && ok;
    ok = expectBool(cache.notifyTimedRecord, false, "eminencecache_t default notifyTimedRecord") && ok;
    ok = expectBool(cache.lastWriteout == timer::time_point{}, true, "eminencecache_t default lastWriteout") && ok;

    cache.activemap.set(0);
    cache.activemap.set(4095);
    cache.notifyTimedRecord = true;
    cache.lastWriteout      = timer::now();

    ok = expectBool(cache.activemap.test(0), true, "eminencecache_t activemap first bit") && ok;
    ok = expectBool(cache.activemap.test(4095), true, "eminencecache_t activemap last bit") && ok;
    ok = expectBool(cache.notifyTimedRecord, true, "eminencecache_t assigned notifyTimedRecord") && ok;
    ok = expectBool(cache.lastWriteout != timer::time_point{}, true, "eminencecache_t assigned lastWriteout") && ok;
    return ok;
}

} // namespace

auto runMMOEminenceSelfTests() -> bool
{
    return testEminenceLogLayoutAndBytes() &&
           testEminenceCacheDefaultsAndMutation();
}
