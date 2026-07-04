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

#include "test_ipc_message_framing.h"

#include "common/ipc.h"

#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace
{

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC message framing self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualBool(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC message framing self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC message framing self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testPayloadRoundTrip() -> bool
{
    bool ok = true;

    const ipc::AccountLogin login{ .accountId = 0x12345678 };
    const auto              bytes = ipc::toBytes(login);
    ok = expectEqualBool(bytes.empty(), false, "account login serialized bytes nonempty") && ok;

    const auto decoded = ipc::fromBytes<ipc::AccountLogin>(std::span<const uint8>{ bytes.data(), bytes.size() });
    ok = expectEqualBool(decoded.has_value(), true, "account login decoded") && ok;
    if (decoded)
    {
        ok = expectEqualInt(decoded->accountId, 0x12345678, "account login decoded account id") && ok;
    }

    const std::vector<uint8> empty{};
    const auto               emptyDecoded = ipc::fromBytes<ipc::AccountLogin>(std::span<const uint8>{ empty.data(), empty.size() });
    ok = expectEqualBool(emptyDecoded.has_value(), false, "empty payload decode rejected") && ok;

    return ok;
}

auto testHeaderRoundTrip() -> bool
{
    bool ok = true;

    const ipc::AccountLogin login{ .accountId = 0x87654321 };
    const auto              payload = ipc::toBytes(login);
    const auto              frame   = ipc::toBytesWithHeader(login);

    ok = expectEqualInt(static_cast<int>(frame.size()), static_cast<int>(payload.size() + 1), "framed length") && ok;
    ok = expectEqualInt(frame.at(0), static_cast<int>(ipc::MessageType::AccountLogin), "framed message type") && ok;

    const auto decoded = ipc::fromBytesWithHeader<ipc::AccountLogin>(std::span<const uint8>{ frame.data(), frame.size() });
    ok = expectEqualBool(decoded.has_value(), true, "framed account login decoded") && ok;
    if (decoded)
    {
        ok = expectEqualInt(decoded->accountId, 0x87654321, "framed account login account id") && ok;
    }

    auto mismatched = frame;
    mismatched[0]   = static_cast<uint8>(ipc::MessageType::CharZone);
    const auto mismatchedDecoded = ipc::fromBytesWithHeader<ipc::AccountLogin>(std::span<const uint8>{ mismatched.data(), mismatched.size() });
    ok = expectEqualBool(mismatchedDecoded.has_value(), false, "mismatched header rejected") && ok;

    const std::vector<uint8> empty{};
    const auto               emptyDecoded = ipc::fromBytesWithHeader<ipc::AccountLogin>(std::span<const uint8>{ empty.data(), empty.size() });
    ok = expectEqualBool(emptyDecoded.has_value(), false, "empty frame rejected") && ok;

    return ok;
}

auto testStringPayloadRoundTrip() -> bool
{
    bool ok = true;

    const ipc::ChatMessageTell tell{
        .senderId      = 1001,
        .senderName    = "Sender",
        .recipientName = "Recipient",
        .message       = "Hello",
        .zoneId        = 230,
        .gmLevel       = 3,
    };
    const auto frame = ipc::toBytesWithHeader(tell);
    ok = expectEqualInt(frame.at(0), static_cast<int>(ipc::MessageType::ChatMessageTell), "tell framed message type") && ok;

    const auto decoded = ipc::fromBytesWithHeader<ipc::ChatMessageTell>(std::span<const uint8>{ frame.data(), frame.size() });
    ok = expectEqualBool(decoded.has_value(), true, "tell decoded") && ok;
    if (decoded)
    {
        ok = expectEqualInt(decoded->senderId, 1001, "tell sender id") && ok;
        ok = expectEqualString(decoded->senderName, "Sender", "tell sender name") && ok;
        ok = expectEqualString(decoded->recipientName, "Recipient", "tell recipient name") && ok;
        ok = expectEqualString(decoded->message, "Hello", "tell message") && ok;
        ok = expectEqualInt(decoded->zoneId, 230, "tell zone id") && ok;
        ok = expectEqualInt(decoded->gmLevel, 3, "tell gm level") && ok;
    }

    return ok;
}

auto testVectorPayloadRoundTrip() -> bool
{
    bool ok = true;

    const ipc::ConquestEvent event{
        .type    = W2M_BroadcastInfluencePoints,
        .payload = { 1, 2, 3, 4 },
    };
    const auto frame = ipc::toBytesWithHeader(event);
    ok = expectEqualInt(frame.at(0), static_cast<int>(ipc::MessageType::ConquestEvent), "conquest framed message type") && ok;

    const auto decoded = ipc::fromBytesWithHeader<ipc::ConquestEvent>(std::span<const uint8>{ frame.data(), frame.size() });
    ok = expectEqualBool(decoded.has_value(), true, "conquest decoded") && ok;
    if (decoded)
    {
        ok = expectEqualInt(decoded->type, W2M_BroadcastInfluencePoints, "conquest event type") && ok;
        ok = expectEqualInt(static_cast<int>(decoded->payload.size()), 4, "conquest payload size") && ok;
        ok = expectEqualInt(decoded->payload.at(0), 1, "conquest payload first byte") && ok;
        ok = expectEqualInt(decoded->payload.at(3), 4, "conquest payload last byte") && ok;
    }

    return ok;
}

} // namespace

auto runIPCMessageFramingSelfTests() -> bool
{
    return testPayloadRoundTrip() && testHeaderRoundTrip() && testStringPayloadRoundTrip() && testVectorPayloadRoundTrip();
}
