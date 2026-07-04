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

#include "test_c2s_bazaar_seller_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x109_bazaar_open.h"
#include "map/packets/c2s/0x10a_bazaar_itemset.h"
#include "map/packets/c2s/0x10b_bazaar_close.h"

namespace
{

using BazaarOpenBytes    = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BAZAAR_OPEN)>;
using BazaarItemSetBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BAZAAR_ITEMSET)>;
using BazaarCloseBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_BAZAAR_CLOSE)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s bazaar seller packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s bazaar seller packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s bazaar seller packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s bazaar seller packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s bazaar seller packet self-test failed: " << label << " got";
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

auto validateBazaarOpenPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateBazaarItemSetPure(std::uint32_t price) -> PacketValidationResult
{
    // The full packet validators require live CCharEntity state for blocked-by-event checks.
    // This helper pins the portable scalar subset used by GP_CLI_COMMAND_BAZAAR_ITEMSET::validate.
    auto validator = PacketValidator(nullptr);
    validator.range("Price", price, 0, 99999999);
    return validator;
}

auto validateBazaarClosePure(std::uint32_t allListClearFlg) -> PacketValidationResult
{
    // The full packet validators require live CCharEntity state for blocked-by-event checks.
    // This helper pins the portable scalar subset used by GP_CLI_COMMAND_BAZAAR_CLOSE::validate.
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(allListClearFlg, 0, "AllListClearFlg not 0");
    return validator;
}

auto makeBazaarOpenPacket() -> GP_CLI_COMMAND_BAZAAR_OPEN
{
    auto packet         = GP_CLI_COMMAND_BAZAAR_OPEN{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BAZAAR_OPEN);
    packet.header.size = sizeof(GP_CLI_COMMAND_BAZAAR_OPEN) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto makeBazaarItemSetPacket() -> GP_CLI_COMMAND_BAZAAR_ITEMSET
{
    auto packet          = GP_CLI_COMMAND_BAZAAR_ITEMSET{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BAZAAR_ITEMSET);
    packet.header.size  = sizeof(GP_CLI_COMMAND_BAZAAR_ITEMSET) / 4U;
    packet.header.sync  = 0xBEEF;
    packet.ItemIndex    = 0x44;
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;
    packet.Price        = 0x55667788;
    return packet;
}

auto makeBazaarClosePacket() -> GP_CLI_COMMAND_BAZAAR_CLOSE
{
    auto packet             = GP_CLI_COMMAND_BAZAAR_CLOSE{};
    packet.header.id       = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_BAZAAR_CLOSE);
    packet.header.size     = sizeof(GP_CLI_COMMAND_BAZAAR_CLOSE) / 4U;
    packet.header.sync     = 0xBEEF;
    packet.AllListClearFlg = 0x11223344;
    return packet;
}

auto testBazaarSellerLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_BAZAAR_OPEN::name, "GP_CLI_COMMAND_BAZAAR_OPEN", "BAZAAR_OPEN name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_BAZAAR_ITEMSET::name, "GP_CLI_COMMAND_BAZAAR_ITEMSET", "BAZAAR_ITEMSET name") && ok;
    ok      = expectEqualString(GP_CLI_COMMAND_BAZAAR_CLOSE::name, "GP_CLI_COMMAND_BAZAAR_CLOSE", "BAZAAR_CLOSE name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BAZAAR_OPEN::packetId), 0x109, "BAZAAR_OPEN packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BAZAAR_ITEMSET::packetId), 0x10A, "BAZAAR_ITEMSET packet id") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_BAZAAR_CLOSE::packetId), 0x10B, "BAZAAR_CLOSE packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BAZAAR_OPEN), 4, "BAZAAR_OPEN sizeof") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BAZAAR_ITEMSET), 12, "BAZAAR_ITEMSET sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_ITEMSET, ItemIndex), 4, "BAZAAR_ITEMSET ItemIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_ITEMSET, padding00), 5, "BAZAAR_ITEMSET padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_ITEMSET, Price), 8, "BAZAAR_ITEMSET Price offset") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_BAZAAR_CLOSE), 8, "BAZAAR_CLOSE sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_BAZAAR_CLOSE, AllListClearFlg), 4, "BAZAAR_CLOSE AllListClearFlg offset") && ok;
    return ok;
}

auto testBazaarSellerEncodedBytes() -> bool
{
    bool ok = true;

    auto openExpected = BazaarOpenBytes{ 0x09, 0x03, 0xEF, 0xBE };
    ok                = expectBytes(encodedPacketBytes(makeBazaarOpenPacket()), openExpected, "BAZAAR_OPEN encoded packet") && ok;

    auto itemSetExpected = BazaarItemSetBytes{ 0x0A, 0x07, 0xEF, 0xBE, 0x44, 0xAA, 0xBB, 0xCC, 0x88, 0x77, 0x66, 0x55 };
    ok                   = expectBytes(encodedPacketBytes(makeBazaarItemSetPacket()), itemSetExpected, "BAZAAR_ITEMSET encoded packet") && ok;

    auto closeExpected = BazaarCloseBytes{ 0x0B, 0x05, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11 };
    ok                 = expectBytes(encodedPacketBytes(makeBazaarClosePacket()), closeExpected, "BAZAAR_CLOSE encoded packet") && ok;

    return ok;
}

auto testBazaarSellerValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validateBazaarOpenPure(), "BAZAAR_OPEN validation") && ok;
    ok      = expectValid(validateBazaarItemSetPure(0), "BAZAAR_ITEMSET min validation") && ok;
    ok      = expectValid(validateBazaarItemSetPure(99999999), "BAZAAR_ITEMSET max validation") && ok;
    ok      = expectInvalidError(validateBazaarItemSetPure(100000000), "Price out of range: 100000000 not in [0, 99999999]", "BAZAAR_ITEMSET above validation") && ok;
    ok      = expectValid(validateBazaarClosePure(0), "BAZAAR_CLOSE zero validation") && ok;
    ok      = expectInvalidError(validateBazaarClosePure(1), "AllListClearFlg not 0", "BAZAAR_CLOSE nonzero validation") && ok;
    return ok;
}

} // namespace

auto runC2SBazaarSellerPacketSelfTests() -> bool
{
    return testBazaarSellerLayoutsAndMetadata() &&
           testBazaarSellerEncodedBytes() &&
           testBazaarSellerValidation();
}
