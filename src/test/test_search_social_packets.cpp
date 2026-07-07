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

#include "test_search_social_packets.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "common/utils.h"
#include "search/data_loader.h"
#include "search/packets/linkshell_list.h"
#include "search/packets/party_list.h"
#include "search/packets/search_comment.h"
#include "search/packets/search_list.h"

namespace
{

template <typename T>
auto read(const std::uint8_t* data, const std::size_t offset) -> T
{
    T value{};
    std::memcpy(&value, data + offset, sizeof(value));
    return value;
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search social packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(const std::uint8_t* actual, const std::string& expected, const std::string& label) -> bool
{
    if (std::memcmp(actual, expected.data(), expected.size()) != 0)
    {
        std::cerr << "search social packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto readBits(const std::uint8_t* data, std::uint32_t& bitOffset, const std::uint8_t length) -> std::uint64_t
{
    const auto value = unpackBitsLE(data, 0, static_cast<std::int32_t>(bitOffset), length);
    bitOffset += length;
    return value;
}

auto makeVisiblePlayer(const std::string& name) -> SearchEntity*
{
    auto player         = std::make_unique<SearchEntity>();
    player->name        = name;
    player->id          = 0xABCDE;
    player->mjob        = 7;
    player->mlvl        = 75;
    player->sjob        = 3;
    player->slvl        = 37;
    player->nation      = 2;
    player->rank        = 10;
    player->race        = 5;
    player->zone        = 230;
    player->flags1      = 0x1234;
    player->flags2      = 0x87654321;
    player->languages   = 0x00A5;
    player->seacom_type = 0x44;
    return player.release();
}

auto expectVisibleSearchFields(const std::uint8_t* data, std::uint32_t bitOffset, const std::string& name, const bool withLinkshell) -> bool
{
    bool ok = true;
    ok      = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_NAME, "name field type") && ok;
    ok      = expectEqualInt(readBits(data, bitOffset, 4), name.size(), "name length") && ok;
    for (const auto ch : name)
    {
        ok = expectEqualInt(readBits(data, bitOffset, 7), static_cast<std::uint8_t>(ch), "name character") && ok;
    }

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_AREA, "area field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 10), 230, "zone") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_NATION, "nation field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 2), 2, "nation") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_JOB, "job field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 5), 7, "main job") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 5), 3, "sub job") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_LEVEL, "level field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 8), 75, "main level") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 8), 37, "sub level") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_RACE, "race field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 4), 5, "race") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_RANK, "rank field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 8), 10, "rank") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_FLAGS1, "flags1 field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 16), 0x1234, "flags1") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_ID, "id field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 20), 0xABCDE, "id") && ok;

    if (withLinkshell)
    {
        ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_LINKSHELLRANK, "linkshell rank field type") && ok;
        ok = expectEqualInt(readBits(data, bitOffset, 8), 2, "linkshell rank 1") && ok;
        ok = expectEqualInt(readBits(data, bitOffset, 8), 3, "linkshell rank 2") && ok;
        ok = expectEqualInt(readBits(data, bitOffset, 8), 0, "linkshell rank 3") && ok;
        ok = expectEqualInt(readBits(data, bitOffset, 32), 0x01020304, "linkshell id 1") && ok;
        ok = expectEqualInt(readBits(data, bitOffset, 32), 0x05060708, "linkshell id 2") && ok;
        ok = expectEqualInt(readBits(data, bitOffset, 32), 0, "linkshell id 3") && ok;
    }

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_UNK0x0E, "unknown field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 32), 0, "unknown field value") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_COMMENT, "comment field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 32), 0x44, "comment type") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_FLAGS2, "flags2 field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 32), 0x87654321, "flags2") && ok;

    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_LANGUAGE, "language field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 16), 0x00A5, "languages") && ok;
    return ok;
}

auto testSearchListHeadersFinalAndPlayer() -> bool
{
    auto packet = CSearchListPacket(321);
    bool ok     = true;
    ok          = expectEqualInt(packet.GetSize(), 44, "search list empty size") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0A], 0x00, "search list initial final flag") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0B], 0x80, "search list packet type") && ok;
    ok          = expectEqualInt(read<std::uint16_t>(packet.GetData(), 0x0E), 321, "search list total") && ok;

    packet.SetFinal();
    ok = expectEqualInt(packet.GetData()[0x0A], 0x80, "search list final flag") && ok;

    ok = expectEqualInt(packet.AddPlayer(makeVisiblePlayer("PlayerOne")), true, "search list add player") && ok;
    const auto* data = packet.GetData();
    ok               = expectEqualInt(data[0x18], read<std::uint16_t>(data, 0x08) - 0x18 - 1, "search list row size byte") && ok;
    ok               = expectEqualInt(packet.GetSize(), read<std::uint16_t>(data, 0x08) + 20, "search list size follows header length") && ok;
    ok               = expectVisibleSearchFields(data, 0x19 * 8, "PlayerOne", false) && ok;
    return ok;
}

auto testSearchListTruncatesNameAndSkipsPrivateFields() -> bool
{
    auto packet = CSearchListPacket(1);
    auto player = std::unique_ptr<SearchEntity>(makeVisiblePlayer("VeryLongPlayerName"));
    player->flags1 |= 0x4000;
    player->seacom_type = 0;
    packet.AddPlayer(player.release());

    const auto* data       = packet.GetData();
    std::uint32_t bitOffset = 0x19 * 8;
    bool ok                = true;
    ok                     = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_NAME, "private name field type") && ok;
    ok                     = expectEqualInt(readBits(data, bitOffset, 4), 15, "private truncated name length") && ok;
    for (const auto ch : std::string("VeryLongPlayerN"))
    {
        ok = expectEqualInt(readBits(data, bitOffset, 7), static_cast<std::uint8_t>(ch), "private truncated name character") && ok;
    }
    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_AREA, "private area field type") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 10), 230, "private zone") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 5), SEARCH_FLAGS1, "private skips to flags1") && ok;
    ok = expectEqualInt(readBits(data, bitOffset, 16), 0x5234, "private flags1") && ok;
    return ok;
}

auto testPartyListHeadersAndPlayer() -> bool
{
    auto packet = CPartyListPacket(0x11223344, 260);
    bool ok     = true;
    ok          = expectEqualInt(packet.GetSize(), 44, "party list empty size") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0A], 0x80, "party list final flag") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0B], 0x82, "party list packet type") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0E], 4, "party list total truncates to uint8") && ok;

    packet.AddPlayer(makeVisiblePlayer("Party"));
    const auto* data = packet.GetData();
    ok               = expectEqualInt(data[0x18], read<std::uint16_t>(data, 0x08) - 0x18 - 1, "party list row size byte") && ok;
    ok               = expectVisibleSearchFields(data, 0x19 * 8, "Party", false) && ok;
    return ok;
}

auto testLinkshellListHeadersFinalAndPlayer() -> bool
{
    auto packet = CLinkshellListPacket(0x11223344, 513);
    bool ok     = true;
    ok          = expectEqualInt(packet.GetSize(), 44, "linkshell list empty size") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0A], 0x00, "linkshell list initial final flag") && ok;
    ok          = expectEqualInt(packet.GetData()[0x0B], 0x82, "linkshell list packet type") && ok;
    ok          = expectEqualInt(read<std::uint16_t>(packet.GetData(), 0x0E), 513, "linkshell list total") && ok;

    auto player             = std::unique_ptr<SearchEntity>(makeVisiblePlayer("LinkShellPlayer"));
    player->linkshellrank1  = 2;
    player->linkshellrank2  = 3;
    player->linkshellid1    = 0x01020304;
    player->linkshellid2    = 0x05060708;
    ok                      = expectEqualInt(packet.AddPlayer(player.release()), true, "linkshell list add player") && ok;
    packet.SetFinal();

    const auto* data = packet.GetData();
    ok               = expectEqualInt(data[0x0A], 0x80, "linkshell list final flag") && ok;
    ok               = expectEqualInt(data[0x18], read<std::uint16_t>(data, 0x08) - 0x18 - 1, "linkshell list row size byte") && ok;
    ok               = expectVisibleSearchFields(data, 0x19 * 8, "LinkShellPlayer", true) && ok;
    return ok;
}

auto testSearchCommentPacketHeadersAndPadding() -> bool
{
    auto packet      = SearchCommentPacket(0xAABBCCDD, "hello");
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(packet.GetSize(), 204, "search comment size") && ok;
    ok      = expectEqualInt(data[0x08], 154, "search comment data size") && ok;
    ok      = expectEqualInt(data[0x0A], 0x80, "search comment search server flag") && ok;
    ok      = expectEqualInt(data[0x0B], 0x88, "search comment packet type") && ok;
    ok      = expectEqualInt(data[0x0E], 1, "search comment marker") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x18), 0xAABBCCDD, "search comment player id") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x1C), 124, "search comment length") && ok;
    ok      = expectBytes(data + 0x1E, "hello", "search comment text") && ok;
    ok      = expectEqualInt(data[0x23], ' ', "search comment first padding byte") && ok;
    ok      = expectEqualInt(data[0x98], ' ', "search comment last padding byte") && ok;
    ok      = expectEqualInt(data[0x99], 0, "search comment penultimate zero") && ok;
    ok      = expectEqualInt(data[0x9A], 0, "search comment terminator") && ok;
    return ok;
}

} // namespace

auto runSearchSocialPacketSelfTests() -> bool
{
    return testSearchListHeadersFinalAndPlayer() &&
           testSearchListTruncatesNameAndSkipsPrivateFields() &&
           testPartyListHeadersAndPlayer() &&
           testLinkshellListHeadersFinalAndPlayer() &&
           testSearchCommentPacketHeadersAndPadding();
}
