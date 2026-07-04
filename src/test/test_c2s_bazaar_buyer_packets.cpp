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

#include "test_c2s_bazaar_buyer_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x104_bazaar_exit.h"
#include "map/packets/c2s/0x105_bazaar_list.h"
#include "map/packets/c2s/0x106_bazaar_buy.h"

namespace
{

using BazaarExitBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BAZAAR_EXIT)>;
using BazaarListBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BAZAAR_LIST)>;
using BazaarBuyBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BAZAAR_BUY)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s bazaar buyer packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s bazaar buyer packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s bazaar buyer packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s bazaar buyer packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s bazaar buyer packet self-test failed: " << label << " got";
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

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

template <typename Packet>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, sizeof(Packet)>
{
    auto bytes = std::array<std::uint8_t, sizeof(Packet)>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto validateBazaarExitPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateBazaarListPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateBazaarBuyPure(std::uint32_t buyNum) -> PacketValidationResult
{
    // The full packet validators require live CCharEntity state for blocked-by-event checks.
    // This helper pins the portable scalar subset used by GP_CLI_COMMAND_BAZAAR_BUY::validate.
    auto validator = PacketValidator(nullptr);
    validator.range("BuyNum", buyNum, 1, 99);
    return validator;
}

auto makeBazaarExitPacket() -> GP_CLI_COMMAND_BAZAAR_EXIT
{
    auto packet         = GP_CLI_COMMAND_BAZAAR_EXIT{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BAZAAR_EXIT);
    packet.header.size = sizeof(GP_CLI_COMMAND_BAZAAR_EXIT) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto makeBazaarListPacket() -> GP_CLI_COMMAND_BAZAAR_LIST
{
    auto packet         = GP_CLI_COMMAND_BAZAAR_LIST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BAZAAR_LIST);
    packet.header.size = sizeof(GP_CLI_COMMAND_BAZAAR_LIST) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x5566;
    packet.padding00   = 0x7788;
    return packet;
}

auto makeBazaarBuyPacket() -> GP_CLI_COMMAND_BAZAAR_BUY
{
    auto packet                 = GP_CLI_COMMAND_BAZAAR_BUY{};
    packet.header.id           = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BAZAAR_BUY);
    packet.header.size         = sizeof(GP_CLI_COMMAND_BAZAAR_BUY) / 4U;
    packet.header.sync         = 0xBEEF;
    packet.BazaarItemIndex     = 0x44;
    packet.padding00[0]        = 0xAA;
    packet.padding00[1]        = 0xBB;
    packet.padding00[2]        = 0xCC;
    packet.BuyNum              = 0x55667788;
    return packet;
}

auto testBazaarBuyerLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_BAZAAR_EXIT::name, "GP_CLI_COMMAND_BAZAAR_EXIT", "BAZAAR_EXIT name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_BAZAAR_LIST::name, "GP_CLI_COMMAND_BAZAAR_LIST", "BAZAAR_LIST name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_BAZAAR_BUY::name, "GP_CLI_COMMAND_BAZAAR_BUY", "BAZAAR_BUY name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BAZAAR_EXIT::packetId), 0x104, "BAZAAR_EXIT packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BAZAAR_LIST::packetId), 0x105, "BAZAAR_LIST packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BAZAAR_BUY::packetId), 0x106, "BAZAAR_BUY packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BAZAAR_EXIT), 4, "BAZAAR_EXIT sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BAZAAR_LIST), 12, "BAZAAR_LIST sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_LIST, UniqueNo), 4, "BAZAAR_LIST UniqueNo offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_LIST, ActIndex), 8, "BAZAAR_LIST ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_LIST, padding00), 10, "BAZAAR_LIST padding00 offset") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BAZAAR_BUY), 12, "BAZAAR_BUY sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_BUY, BazaarItemIndex), 4, "BAZAAR_BUY BazaarItemIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_BUY, padding00), 5, "BAZAAR_BUY padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_BUY, BuyNum), 8, "BAZAAR_BUY BuyNum offset") && ok;
    return ok;
}

auto testBazaarBuyerEncodedBytes() -> bool
{
    bool ok = true;

    auto exitExpected = BazaarExitBytes{ 0x04, 0x03, 0xEF, 0xBE };
    ok                = expectBytes(encodedPacketBytes(makeBazaarExitPacket()), exitExpected, "BAZAAR_EXIT encoded packet") && ok;

    auto listExpected = BazaarListBytes{ 0x05, 0x07, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11, 0x66, 0x55, 0x88, 0x77 };
    ok                = expectBytes(encodedPacketBytes(makeBazaarListPacket()), listExpected, "BAZAAR_LIST encoded packet") && ok;

    auto buyExpected = BazaarBuyBytes{ 0x06, 0x07, 0xEF, 0xBE, 0x44, 0xAA, 0xBB, 0xCC, 0x88, 0x77, 0x66, 0x55 };
    ok               = expectBytes(encodedPacketBytes(makeBazaarBuyPacket()), buyExpected, "BAZAAR_BUY encoded packet") && ok;

    return ok;
}

auto testBazaarBuyerValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateBazaarExitPure(), "BAZAAR_EXIT validation") && ok;
    ok      = expectValid(validateBazaarListPure(), "BAZAAR_LIST validation") && ok;
    ok      = expectValid(validateBazaarBuyPure(1), "BAZAAR_BUY min validation") && ok;
    ok      = expectValid(validateBazaarBuyPure(99), "BAZAAR_BUY max validation") && ok;
    ok      = expectInvalidError(validateBazaarBuyPure(0), "BuyNum out of range: 0 not in [1, 99]", "BAZAAR_BUY below validation") && ok;
    ok      = expectInvalidError(validateBazaarBuyPure(100), "BuyNum out of range: 100 not in [1, 99]", "BAZAAR_BUY above validation") && ok;
    return ok;
}

} // namespace

auto runC2SBazaarBuyerPacketSelfTests() -> bool
{
    return testBazaarBuyerLayoutsAndMetadata() &&
           testBazaarBuyerEncodedBytes() &&
           testBazaarBuyerValidation();
}
