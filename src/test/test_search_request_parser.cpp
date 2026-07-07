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

#include "test_search_request_parser.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/utils.h"
#include "search/data_loader.h"
#include "search/packets/search_list.h"
#include "search/search.h"

namespace
{

struct SearchPayload
{
    std::array<std::uint8_t, 64> data{};
    std::uint32_t                bitOffset = 0;

    void appendField(const SEARCHTYPE field, const bool present)
    {
        bitOffset = packBitsLE(data.data(), field, static_cast<std::int32_t>(bitOffset), 5);
        bitOffset = packBitsLE(data.data(), 0, static_cast<std::int32_t>(bitOffset), 1);
        bitOffset = packBitsLE(data.data(), present ? 1 : 0, static_cast<std::int32_t>(bitOffset), 1);
    }

    void appendRawField(const SEARCHTYPE field)
    {
        bitOffset = packBitsLE(data.data(), field, static_cast<std::int32_t>(bitOffset), 5);
    }

    void appendValue(const std::uint64_t value, const std::uint8_t bits)
    {
        bitOffset = packBitsLE(data.data(), value, static_cast<std::int32_t>(bitOffset), bits);
    }

    [[nodiscard]] auto size() const -> std::uint8_t
    {
        return static_cast<std::uint8_t>((bitOffset + 7) / 8);
    }
};

auto expectEqualUInt(const std::uint64_t actual, const std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search request parser self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search request parser self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto appendName(SearchPayload& payload, const std::string& name)
{
    payload.appendField(SEARCH_NAME, true);
    payload.appendValue(name.size(), 5);
    for (const auto ch : name)
    {
        payload.appendValue(static_cast<std::uint8_t>(ch), 7);
    }
}

auto testParsesRepresentativeSearchRequest() -> bool
{
    SearchPayload payload;
    appendName(payload, "Alpha");

    payload.appendField(SEARCH_AREA, true);
    payload.appendValue(230, 10);
    payload.appendField(SEARCH_AREA, true);
    payload.appendValue(231, 10);
    payload.appendField(SEARCH_AREA, false);

    payload.appendField(SEARCH_NATION, true);
    payload.appendValue(0, 2);
    payload.appendField(SEARCH_JOB, true);
    payload.appendValue(7, 5);
    payload.appendField(SEARCH_LEVEL, true);
    payload.appendValue(10, 8);
    payload.appendValue(75, 8);
    payload.appendField(SEARCH_RACE, true);
    payload.appendValue(0, 4);
    payload.appendField(SEARCH_RANK, true);
    payload.appendValue(1, 8);
    payload.appendValue(10, 8);
    payload.appendField(SEARCH_FLAGS1, true);
    payload.appendValue(0x8123, 16);

    payload.appendRawField(SEARCH_COMMENT);
    payload.appendValue(0xAABBCCDD, 32);
    payload.appendRawField(SEARCH_LINKSHELL);
    payload.appendValue(0x11223344, 32);
    payload.appendRawField(SEARCH_LINKSHELL2);
    payload.appendValue(0x55667788, 32);
    payload.appendRawField(SEARCH_FRIEND);
    payload.appendRawField(SEARCH_FLAGS2);
    payload.appendValue(0x87654321, 32);

    const auto request = ParseSearchRequestPayload(payload.data.data(), payload.size());
    bool       ok      = true;
    ok                 = expectEqualString(request.name, "Alpha", "name") && ok;
    ok                 = expectEqualUInt(request.nameLen, 5, "name length") && ok;
    ok                 = expectEqualUInt(request.zoneid[0], 230, "first area") && ok;
    ok                 = expectEqualUInt(request.zoneid[1], 231, "second area") && ok;
    ok                 = expectEqualUInt(request.zoneid[2], 0, "area terminator leaves remaining areas zero") && ok;
    ok                 = expectEqualUInt(request.nation, 0, "nation zero is valid") && ok;
    ok                 = expectEqualUInt(request.jobid, 7, "job") && ok;
    ok                 = expectEqualUInt(request.minlvl, 10, "min level") && ok;
    ok                 = expectEqualUInt(request.maxlvl, 75, "max level") && ok;
    ok                 = expectEqualUInt(request.race, 0, "race zero is valid") && ok;
    ok                 = expectEqualUInt(request.minRank, 1, "min rank") && ok;
    ok                 = expectEqualUInt(request.maxRank, 10, "max rank") && ok;
    ok                 = expectEqualUInt(request.commentType, 0xDD, "comment type truncates to uint8") && ok;
    ok                 = expectEqualUInt(request.flags, 0x87654321, "flags2 overwrites flags1") && ok;
    ok                 = expectEqualUInt(request.lsId.value(), 0x55667788, "linkshell2 overwrites linkshell1") && ok;
    return ok;
}

auto testDefaultsAndAbsentFields() -> bool
{
    SearchPayload payload;
    payload.appendField(SEARCH_NATION, false);
    payload.appendField(SEARCH_RACE, false);
    payload.appendField(SEARCH_JOB, false);

    const auto request = ParseSearchRequestPayload(payload.data.data(), payload.size());
    bool       ok      = true;
    ok                 = expectEqualString(request.name, "", "default name") && ok;
    ok                 = expectEqualUInt(request.nameLen, 0, "default name length") && ok;
    ok                 = expectEqualUInt(request.nation, 255, "default nation") && ok;
    ok                 = expectEqualUInt(request.race, 255, "default race") && ok;
    ok                 = expectEqualUInt(request.jobid, 0, "absent job") && ok;
    ok                 = expectEqualUInt(request.zoneid[0], 0, "default first area") && ok;
    ok                 = expectEqualUInt(request.flags, 0, "default flags") && ok;
    return ok;
}

auto testTruncatedNameStopsParsing() -> bool
{
    SearchPayload payload;
    payload.appendField(SEARCH_NAME, true);

    const auto request = ParseSearchRequestPayload(payload.data.data(), payload.size());
    bool       ok      = true;
    ok                 = expectEqualString(request.name, "", "truncated name") && ok;
    ok                 = expectEqualUInt(request.nameLen, 0, "truncated name length") && ok;
    ok                 = expectEqualUInt(request.nation, 255, "truncated parse keeps default nation") && ok;
    return ok;
}

} // namespace

auto runSearchRequestParserSelfTests() -> bool
{
    bool ok = true;
    ok      = testParsesRepresentativeSearchRequest() && ok;
    ok      = testDefaultsAndAbsentFields() && ok;
    ok      = testTruncatedNameStopsParsing() && ok;
    return ok;
}
