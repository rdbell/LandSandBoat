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

#include "test_c2s_header_only_request_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x05a_reqconquest.h"
#include "map/packets/c2s/0x114_map_markers.h"

namespace
{

using HeaderBytes = std::array<std::uint8_t, sizeof(GP_CLI_HEADER)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s header-only request self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s header-only request self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s header-only request self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

auto expectBytes(const HeaderBytes& actual, const HeaderBytes& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s header-only request self-test failed: " << label << " got";
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

template <typename PacketT>
auto encodedHeader(PacketC2S packetId) -> HeaderBytes
{
    auto packet        = PacketT{};
    packet.header.id   = static_cast<std::uint16_t>(packetId);
    packet.header.size = sizeof(PacketT) / 4;
    packet.header.sync = 0xBEEF;

    auto bytes = HeaderBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto testReqConquest() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_REQCONQUEST::name, "GP_CLI_COMMAND_REQCONQUEST", "REQCONQUEST static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_REQCONQUEST::packetId == PacketC2S::GP_CLI_COMMAND_REQCONQUEST, "REQCONQUEST static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_REQCONQUEST), sizeof(GP_CLI_HEADER), "sizeof(GP_CLI_COMMAND_REQCONQUEST)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_REQCONQUEST, header), 0, "REQCONQUEST header offset") && ok;
    ok = expectBytes(encodedHeader<GP_CLI_COMMAND_REQCONQUEST>(PacketC2S::GP_CLI_COMMAND_REQCONQUEST),
                     HeaderBytes{ 0x5A, 0x02, 0xEF, 0xBE },
                     "REQCONQUEST encoded header") &&
         ok;
    ok = expectValid(GP_CLI_COMMAND_REQCONQUEST{}.validate(nullptr, nullptr), "REQCONQUEST validation") && ok;

    return ok;
}

auto testMapMarkers() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_MAP_MARKERS::name, "GP_CLI_COMMAND_MAP_MARKERS", "MAP_MARKERS static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_MAP_MARKERS::packetId == PacketC2S::GP_CLI_COMMAND_MAP_MARKERS, "MAP_MARKERS static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MAP_MARKERS), sizeof(GP_CLI_HEADER), "sizeof(GP_CLI_COMMAND_MAP_MARKERS)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MAP_MARKERS, header), 0, "MAP_MARKERS header offset") && ok;
    ok = expectBytes(encodedHeader<GP_CLI_COMMAND_MAP_MARKERS>(PacketC2S::GP_CLI_COMMAND_MAP_MARKERS),
                     HeaderBytes{ 0x14, 0x03, 0xEF, 0xBE },
                     "MAP_MARKERS encoded header") &&
         ok;
    ok = expectValid(GP_CLI_COMMAND_MAP_MARKERS{}.validate(nullptr, nullptr), "MAP_MARKERS validation") && ok;

    return ok;
}

} // namespace

auto runC2SHeaderOnlyRequestPacketSelfTests() -> bool
{
    bool ok = true;

    ok = testReqConquest() && ok;
    ok = testMapMarkers() && ok;

    return ok;
}
