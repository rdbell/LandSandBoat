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

#include "test_search_packet_buffer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/logging.h"
#include "common/md52.h"
#include "common/types/maybe.h"
#include "search/search_packet_hash.h"
#include "search/search.h"
#include "search/search_request_type.h"

namespace
{

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search packet buffer self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "search packet buffer self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search packet buffer self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testRequestTypeConstants() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(TCP_SEARCH_ALL, 0x00, "TCP_SEARCH_ALL value") && ok;
    ok      = expectEqualInt(TCP_GROUP_LIST, 0x02, "TCP_GROUP_LIST value") && ok;
    ok      = expectEqualInt(TCP_SEARCH, 0x03, "TCP_SEARCH value") && ok;
    ok      = expectEqualInt(TCP_AH_HISTORY_SINGLE, 0x05, "TCP_AH_HISTORY_SINGLE value") && ok;
    ok      = expectEqualInt(TCP_AH_HISTORY_STACK, 0x06, "TCP_AH_HISTORY_STACK value") && ok;
    ok      = expectEqualInt(TCP_SEARCH_COMMENT, 0x08, "TCP_SEARCH_COMMENT value") && ok;
    ok      = expectEqualInt(TCP_AH_REQUEST_MORE, 0x10, "TCP_AH_REQUEST_MORE value") && ok;
    ok      = expectEqualInt(TCP_AH_REQUEST, 0x15, "TCP_AH_REQUEST value") && ok;
    return ok;
}

auto testRequestTypeStrings() -> bool
{
    bool ok = true;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_SEARCH_ALL), "SEARCH_ALL", "TCP_SEARCH_ALL string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_GROUP_LIST), "GROUP_LIST", "TCP_GROUP_LIST string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_SEARCH), "SEARCH", "TCP_SEARCH string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_HISTORY_SINGLE), "AH_HISTORY_SINGLE", "TCP_AH_HISTORY_SINGLE string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_HISTORY_STACK), "AH_HISTORY_STACK", "TCP_AH_HISTORY_STACK string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_SEARCH_COMMENT), "SEARCH_COMMENT", "TCP_SEARCH_COMMENT string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_REQUEST_MORE), "AH_REQUEST_MORE", "TCP_AH_REQUEST_MORE string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_REQUEST), "AH_REQUEST", "TCP_AH_REQUEST string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(0xFF), "UNKNOWN", "unknown request type string") && ok;
    return ok;
}

void writeSearchPacketHash(std::uint8_t* packet, const std::uint16_t length)
{
    std::uint8_t digest[16]{};
    md5(packet + 8, digest, length - 28);
    std::memcpy(packet + length - 0x14, digest, sizeof(digest));
}

auto testPacketHashValidationAcceptsMatchingDigest() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 19U) + 3U);
    }

    writeSearchPacketHash(input.data(), input.size());

    return expectTrue(ValidateSearchPacketHash(input.data(), input.size()), "matching packet hash accepted");
}

auto testPacketHashValidationRejectsDigestMismatch() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 23U) + 5U);
    }

    writeSearchPacketHash(input.data(), input.size());
    input[input.size() - 0x14 + 7] ^= 0x80;

    return expectTrue(!ValidateSearchPacketHash(input.data(), input.size()), "mismatched packet hash rejected");
}

auto testPacketHashValidationIgnoresTrailingKeyBytes() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 29U) + 11U);
    }

    writeSearchPacketHash(input.data(), input.size());
    input[input.size() - 4] = 0xAA;
    input[input.size() - 3] = 0xBB;
    input[input.size() - 2] = 0xCC;
    input[input.size() - 1] = 0xDD;

    return expectTrue(ValidateSearchPacketHash(input.data(), input.size()), "trailing key bytes ignored by hash");
}

auto testPacketHashValidationAcceptsMinimumFrame() -> bool
{
    auto input = std::array<std::uint8_t, 28>{};
    writeSearchPacketHash(input.data(), input.size());

    return expectTrue(ValidateSearchPacketHash(input.data(), input.size()), "minimum packet hash frame accepted");
}

auto testAcceptedPacketCopiesBytesAndSize() -> bool
{
    const auto expected = std::array<std::uint8_t, 5>{ 0x10, 0x20, 0x30, 0x40, 0x50 };
    auto input          = expected;
    auto packet         = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    std::fill(input.begin(), input.end(), 0xEE);

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), expected.size(), "accepted packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), expected.data(), expected.size()) == 0, "accepted packet bytes copied") && ok;
    return ok;
}

auto testMaxSizePacketIsAccepted() -> bool
{
    auto input = std::array<std::uint8_t, searchPacket::max_size>{};
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 31U) + 7U);
    }

    auto packet = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), searchPacket::max_size, "max-size packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), input.data(), input.size()) == 0, "max-size packet bytes copied") && ok;
    return ok;
}

auto testShortPacketCopiesPrefixAndSize() -> bool
{
    const auto input = std::array<std::uint8_t, 3>{ 0xAA, 0xBB, 0xCC };
    auto mutableInput = input;
    auto packet       = searchPacket(mutableInput.data(), static_cast<std::uint16_t>(mutableInput.size()));

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), input.size(), "short packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), input.data(), input.size()) == 0, "short packet bytes copied") && ok;
    return ok;
}

auto testOversizedPacketIsRejected() -> bool
{
    auto input = std::array<std::uint8_t, searchPacket::max_size + 1>{};
    auto packet = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    return expectEqualInt(packet.getSize(), 0, "oversized packet size");
}

} // namespace

auto runSearchPacketBufferSelfTests() -> bool
{
    return testRequestTypeConstants() &&
           testRequestTypeStrings() &&
           testPacketHashValidationAcceptsMatchingDigest() &&
           testPacketHashValidationRejectsDigestMismatch() &&
           testPacketHashValidationIgnoresTrailingKeyBytes() &&
           testPacketHashValidationAcceptsMinimumFrame() &&
           testAcceptedPacketCopiesBytesAndSize() &&
           testMaxSizePacketIsAccepted() &&
           testShortPacketCopiesPrefixAndSize() &&
           testOversizedPacketIsRejected();
}
