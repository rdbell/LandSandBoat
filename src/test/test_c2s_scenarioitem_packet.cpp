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

#include "test_c2s_scenarioitem_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "map/enums/key_items.h"
#include "map/packets/c2s/0x064_scenarioitem.h"

static_assert(std::is_same_v<std::underlying_type_t<KeyItem>, uint16_t>);

namespace
{

using ScenarioItemPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SCENARIOITEM)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s SCENARIOITEM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s SCENARIOITEM packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s SCENARIOITEM packet self-test failed: " << label << " got";
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

auto expectVector(const std::vector<std::uint16_t>& actual, const std::vector<std::uint16_t>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s SCENARIOITEM packet self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << value;
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << value;
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto makeScenarioItemPacket() -> GP_CLI_COMMAND_SCENARIOITEM
{
    auto packet             = GP_CLI_COMMAND_SCENARIOITEM{};
    packet.header.id       = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SCENARIOITEM);
    packet.header.size     = sizeof(GP_CLI_COMMAND_SCENARIOITEM) / 4;
    packet.header.sync     = 0xBEEF;
    packet.UniqueNo        = 0x11223344;
    packet.LookItemFlag[0] = 0x80000005;
    packet.LookItemFlag[1] = 0x00000002;
    packet.LookItemFlag[5] = 0x00010000;
    packet.LookItemFlag[15] = 0x80000000;
    packet.ActIndex        = 0x99AA;
    packet.TableIndex      = 3;
    return packet;
}

auto encodedScenarioItemPacket() -> ScenarioItemPacketBytes
{
    const auto packet = makeScenarioItemPacket();
    auto       bytes  = ScenarioItemPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto markedKeyItems(const GP_CLI_COMMAND_SCENARIOITEM& packet) -> std::vector<std::uint16_t>
{
    auto marked = std::vector<std::uint16_t>{};
    for (int i = 0; i < 16; i++)
    {
        const std::uint32_t flags = packet.LookItemFlag[i];
        for (int bit = 0; bit < 32; bit++)
        {
            const auto keyItemId = (packet.TableIndex * 512) + (i * 32) + bit;
            if ((flags >> bit) & 1)
            {
                marked.emplace_back(static_cast<std::uint16_t>(static_cast<KeyItem>(keyItemId)));
            }
        }
    }
    return marked;
}

auto testScenarioItemLayoutAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_SCENARIOITEM::name, "GP_CLI_COMMAND_SCENARIOITEM", "SCENARIOITEM name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SCENARIOITEM::packetId), 0x064, "SCENARIOITEM packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_SCENARIOITEM), 76, "SCENARIOITEM sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_SCENARIOITEM, UniqueNo), 4, "SCENARIOITEM UniqueNo offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_SCENARIOITEM, LookItemFlag), 8, "SCENARIOITEM LookItemFlag offset") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_SCENARIOITEM::LookItemFlag), 64, "SCENARIOITEM LookItemFlag bytes") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_SCENARIOITEM, ActIndex), 72, "SCENARIOITEM ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_SCENARIOITEM, TableIndex), 74, "SCENARIOITEM TableIndex offset") && ok;
    return ok;
}

auto testScenarioItemEncodedBytesAndPayload() -> bool
{
    bool ok = true;

    constexpr auto expected = ScenarioItemPacketBytes{
        0x64, 0x26, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x05, 0x00, 0x00, 0x80,
        0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x80,
        0xAA, 0x99,
        0x03, 0x00,
    };

    const auto packet = makeScenarioItemPacket();
    ok                = expectBytes(encodedScenarioItemPacket(), expected, "SCENARIOITEM encoded bytes") && ok;
    ok                = expectEqualInt(packet.UniqueNo, 0x11223344, "SCENARIOITEM UniqueNo payload") && ok;
    ok                = expectEqualInt(packet.LookItemFlag[0], 0x80000005, "SCENARIOITEM LookItemFlag[0] payload") && ok;
    ok                = expectEqualInt(packet.LookItemFlag[1], 0x00000002, "SCENARIOITEM LookItemFlag[1] payload") && ok;
    ok                = expectEqualInt(packet.LookItemFlag[5], 0x00010000, "SCENARIOITEM LookItemFlag[5] payload") && ok;
    ok                = expectEqualInt(packet.LookItemFlag[15], 0x80000000, "SCENARIOITEM LookItemFlag[15] payload") && ok;
    ok                = expectEqualInt(packet.ActIndex, 0x99AA, "SCENARIOITEM ActIndex payload") && ok;
    ok                = expectEqualInt(packet.TableIndex, 3, "SCENARIOITEM TableIndex payload") && ok;
    return ok;
}

auto testScenarioItemKeyItemExpansion() -> bool
{
    bool ok = true;
    ok      = expectVector(markedKeyItems(makeScenarioItemPacket()), { 1536, 1538, 1567, 1569, 1712, 2047 }, "SCENARIOITEM marked key item order") && ok;

    auto wrappingPacket             = GP_CLI_COMMAND_SCENARIOITEM{};
    wrappingPacket.TableIndex      = 128;
    wrappingPacket.LookItemFlag[0] = 1;
    ok = expectVector(markedKeyItems(wrappingPacket), { static_cast<std::uint16_t>(KeyItem::NONE) }, "SCENARIOITEM KeyItem uint16 width") && ok;
    return ok;
}

} // namespace

auto runC2SScenarioItemPacketSelfTests() -> bool
{
    return testScenarioItemLayoutAndMetadata() &&
           testScenarioItemEncodedBytesAndPayload() &&
           testScenarioItemKeyItemExpansion();
}
