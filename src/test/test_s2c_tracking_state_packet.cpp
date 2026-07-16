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

#include "test_s2c_tracking_state_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0f6_tracking_state.h"

namespace
{

constexpr auto trackingStateStateOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TRACKING_STATE::PacketData, State);
constexpr auto trackingStatePacketDataSize    = sizeof(GP_SERV_COMMAND_TRACKING_STATE::PacketData);
constexpr auto trackingStatePacketDefaultSize = sizeof(GP_SERV_HEADER) + trackingStatePacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TRACKING_STATE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 8>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c TRACKING_STATE packet self-test failed: " << label << " got";
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c TRACKING_STATE packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(trackingStatePacketDataSize, 1, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(trackingStatePacketDefaultSize, 5, "packet default size before rounding") && ok;
    ok      = expectEqualUInt(trackingStateStateOffset, 4, "State offset") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TRACKING_STATE::None), 0x00, "None enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TRACKING_STATE::ListStart), 0x01, "ListStart enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TRACKING_STATE::ListEnd), 0x02, "ListEnd enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TRACKING_STATE::End), 0x03, "End enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TRACKING_STATE::ErrEtc), 0x0A, "ErrEtc enum") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_TRACKING_STATE(GP_TRACKING_STATE::ListStart);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 8>{
        0xF6, 0x04, 0xEF, 0xBE,
        0x01, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0F6, "TRACKING_STATE type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "TRACKING_STATE rounded size") && ok;
    ok      = expectBytes(packet, expected, "encoded TRACKING_STATE prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "TRACKING_STATE tail") && ok;
    return ok;
}

auto testConstructorStateValues() -> bool
{
    const auto states = std::array<GP_TRACKING_STATE, 5>{
        GP_TRACKING_STATE::None,
        GP_TRACKING_STATE::ListStart,
        GP_TRACKING_STATE::ListEnd,
        GP_TRACKING_STATE::End,
        GP_TRACKING_STATE::ErrEtc,
    };

    bool ok = true;
    for (const auto state : states)
    {
        auto packet         = GP_SERV_COMMAND_TRACKING_STATE(state);
        const auto expected = static_cast<std::uint8_t>(state);

        ok = expectEqualUInt(packetData(packet)[trackingStateStateOffset], expected, "constructor state") && ok;
        ok = expectEqualUInt(packet.getSize(), 8, "constructor rounded size") && ok;
    }
    return ok;
}

auto testRuntimePlan() -> bool
{
    const auto states = std::array<GP_TRACKING_STATE, 5>{
        GP_TRACKING_STATE::None,
        GP_TRACKING_STATE::ListStart,
        GP_TRACKING_STATE::ListEnd,
        GP_TRACKING_STATE::End,
        GP_TRACKING_STATE::ErrEtc,
    };

    bool ok = true;
    for (const auto state : states)
    {
        const auto plan = trackingstatehelpers::PlanFor({
            .state = state,
        });
        ok = expectEqualUInt(static_cast<std::uint8_t>(plan.State), static_cast<std::uint8_t>(state), "runtime plan state") && ok;
    }
    return ok;
}

} // namespace

auto runS2CTrackingStatePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testConstructorStateValues() && ok;
    ok      = testRuntimePlan() && ok;
    return ok;
}
