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

#include "test_mmo_keyitems.h"

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
        std::cerr << "mmo keyitems self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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

template <std::size_t N>
auto expectOnlyBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    for (std::size_t i = 0; i < N; ++i)
    {
        if (actual[i] != expected[i])
        {
            std::cerr << "mmo keyitems self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(actual[i])
                      << " expected " << static_cast<unsigned>(expected[i]) << '\n';
            return false;
        }
    }
    return true;
}

auto testKeyItemsLayout() -> bool
{
    bool ok = true;

    keyitems_table_t table{};
    keyitems_t       keys{};

    ok = expectEqualUInt(sizeof(xi::bitset<512>), 64, "xi::bitset<512> sizeof") && ok;
    ok = expectEqualUInt(sizeof(keyitems_table_t), 128, "keyitems_table_t sizeof") && ok;
    ok = expectEqualUInt(sizeof(keyitems_t), 1024, "keyitems_t sizeof") && ok;
    ok = expectEqualUInt(keys.tables.size(), 8, "keyitems_t table count") && ok;
    ok = expectEqualUInt(fieldOffset(table, table.keyList), 0, "keyitems_table_t keyList offset") && ok;
    ok = expectEqualUInt(fieldOffset(table, table.seenList), 64, "keyitems_table_t seenList offset") && ok;
    ok = expectEqualUInt(fieldOffset(keys, keys.tables[1]), 128, "keyitems_t table stride") && ok;
    return ok;
}

auto testKeyItemsStorageBytes() -> bool
{
    bool ok = true;

    keyitems_t keys{};
    ok = expectOnlyBytes(bytesOf(keys), std::array<std::uint8_t, sizeof(keyitems_t)>{}, "zero keyitems_t bytes") && ok;

    keys.tables[0].keyList[0]     = true;
    keys.tables[0].keyList[9]     = true;
    keys.tables[0].keyList[511]   = true;
    keys.tables[2].seenList[12]   = true;
    keys.tables[7].seenList[511]  = true;
    keys.tables[0].keyList[9]     = false;
    keys.tables[2].seenList[12]   = false;
    keys.tables[2].seenList[12]   = true;

    std::array<std::uint8_t, sizeof(keyitems_t)> expected{};
    expected[0]                    = 0x01;
    expected[63]                   = 0x80;
    expected[(2 * 128) + 64 + 1]   = 0x10;
    expected[(7 * 128) + 64 + 63]  = 0x80;

    ok = expectOnlyBytes(bytesOf(keys), expected, "keyitems_t golden bytes") && ok;
    ok = expectEqualUInt(keys.tables[0].keyList[0], true, "keyList[0] set") && ok;
    ok = expectEqualUInt(keys.tables[0].keyList[9], false, "keyList[9] cleared") && ok;
    ok = expectEqualUInt(keys.tables[0].keyList[511], true, "keyList[511] set") && ok;
    ok = expectEqualUInt(keys.tables[2].seenList[12], true, "seenList[12] set") && ok;
    ok = expectEqualUInt(keys.tables[7].seenList[511], true, "last seenList bit set") && ok;
    return ok;
}

} // namespace

auto runMMOKeyItemsSelfTests() -> bool
{
    return testKeyItemsLayout() &&
           testKeyItemsStorageBytes();
}
