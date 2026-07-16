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

#include "test_c2s_player_trade_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "map/packets/c2s/0x032_trade_req.h"
#include "map/packets/c2s/0x033_trade_res.h"
#include "map/packets/c2s/0x034_trade_list.h"

namespace
{

using ReqPacketBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRADE_REQ)>;
using ResPacketBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRADE_RES)>;
using ListPacketBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_TRADE_LIST)>;

struct EnumCase
{
    std::uint64_t actual;
    std::uint64_t expected;
    std::string   label;
};

template <typename T>
auto enumValue(T value) -> std::uint64_t
{
    return static_cast<std::uint64_t>(value);
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s player trade packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s player trade packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s player trade packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s player trade packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s player trade packet self-test failed: " << label << " got";
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

auto expectAll(const std::vector<EnumCase>& tests) -> bool
{
    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(test.actual, test.expected, test.label) && ok;
    }
    return ok;
}

auto encodedTradeReq(std::uint32_t uniqueNo, std::uint16_t actIndex, std::uint16_t padding00) -> ReqPacketBytes
{
    auto packet         = GP_CLI_COMMAND_TRADE_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRADE_REQ);
    packet.header.size = sizeof(GP_CLI_COMMAND_TRADE_REQ) / 4;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = uniqueNo;
    packet.ActIndex    = actIndex;
    packet.padding00   = padding00;

    auto bytes = ReqPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedTradeRes(std::uint32_t kind, std::uint16_t tradeCounter) -> ResPacketBytes
{
    auto packet          = GP_CLI_COMMAND_TRADE_RES{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRADE_RES);
    packet.header.size  = sizeof(GP_CLI_COMMAND_TRADE_RES) / 4;
    packet.header.sync  = 0xBEEF;
    packet.Kind         = kind;
    packet.TradeCounter = tradeCounter;

    auto bytes = ResPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto encodedTradeList(std::uint32_t itemNum, std::uint16_t itemNo, std::uint8_t itemIndex, std::uint8_t tradeIndex) -> ListPacketBytes
{
    auto packet          = GP_CLI_COMMAND_TRADE_LIST{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_TRADE_LIST);
    packet.header.size  = sizeof(GP_CLI_COMMAND_TRADE_LIST) / 4;
    packet.header.sync  = 0xBEEF;
    packet.ItemNum      = itemNum;
    packet.ItemNo       = itemNo;
    packet.ItemIndex    = itemIndex;
    packet.TradeIndex   = tradeIndex;

    auto bytes = ListPacketBytes{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto tradeReqPureValidation() -> PacketValidationResult
{
    return PacketValidationResult();
}

auto tradeResKindValidation(std::uint32_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_TRADE_RES_KIND>(kind);
    return static_cast<PacketValidationResult>(validator);
}

auto tradeListPureValidation(std::uint8_t tradeIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("TradeIndex", tradeIndex, 0, 8);
    return static_cast<PacketValidationResult>(validator);
}

auto testTradeReqLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_TRADE_REQ::name, "GP_CLI_COMMAND_TRADE_REQ", "TRADE_REQ static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_TRADE_REQ::packetId == PacketC2S::GP_CLI_COMMAND_TRADE_REQ, "TRADE_REQ static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRADE_REQ), 12, "sizeof(GP_CLI_COMMAND_TRADE_REQ)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_REQ, header), 0, "TRADE_REQ header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_REQ, UniqueNo), 4, "TRADE_REQ UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_REQ, ActIndex), 8, "TRADE_REQ ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_REQ, padding00), 10, "TRADE_REQ padding00 offset") && ok;
    ok = expectBytes(encodedTradeReq(0x11223344, 0x5566, 0x7788),
                     ReqPacketBytes{ 0x32, 0x06, 0xEF, 0xBE,
                                     0x44, 0x33, 0x22, 0x11,
                                     0x66, 0x55, 0x88, 0x77 },
                     "TRADE_REQ encoded packet") &&
         ok;

    return ok;
}

auto testTradeReqPayloadStorage() -> bool
{
    auto packet       = GP_CLI_COMMAND_TRADE_REQ{};
    packet.UniqueNo  = 0xFFFFFFFF;
    packet.ActIndex  = 0xFFFE;
    packet.padding00 = 0xFFFF;

    bool ok = true;
    ok      = expectEqualInt(packet.UniqueNo, 0xFFFFFFFF, "TRADE_REQ UniqueNo") && ok;
    ok      = expectEqualInt(packet.ActIndex, 0xFFFE, "TRADE_REQ ActIndex") && ok;
    ok      = expectEqualInt(packet.padding00, 0xFFFF, "TRADE_REQ padding00") && ok;
    return ok;
}

auto testTradeReqPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(tradeReqPureValidation(), "TRADE_REQ pure scalar validation") && ok;
    return ok;
}

auto testTradeResLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_TRADE_RES::name, "GP_CLI_COMMAND_TRADE_RES", "TRADE_RES static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_TRADE_RES::packetId == PacketC2S::GP_CLI_COMMAND_TRADE_RES, "TRADE_RES static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRADE_RES), 12, "sizeof(GP_CLI_COMMAND_TRADE_RES)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_RES, header), 0, "TRADE_RES header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_RES, Kind), 4, "TRADE_RES Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_RES, TradeCounter), 8, "TRADE_RES TradeCounter offset") && ok;
    ok = expectBytes(encodedTradeRes(enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::MakeCancell), 0x5566),
                     ResPacketBytes{ 0x33, 0x06, 0xEF, 0xBE,
                                     0x03, 0x00, 0x00, 0x00,
                                     0x66, 0x55, 0x00, 0x00 },
                     "TRADE_RES encoded packet") &&
         ok;

    return ok;
}

auto testTradeResKindEnumValues() -> bool
{
    return expectAll({
        { enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::Start), 0, "TradeResKind::Start" },
        { enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::Cancell), 1, "TradeResKind::Cancell" },
        { enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::Make), 2, "TradeResKind::Make" },
        { enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::MakeCancell), 3, "TradeResKind::MakeCancell" },
    });
}

auto testTradeResPayloadStorage() -> bool
{
    auto packet          = GP_CLI_COMMAND_TRADE_RES{};
    packet.Kind         = 0xFFFFFFFF;
    packet.TradeCounter = 0xFFFE;

    bool ok = true;
    ok      = expectEqualInt(packet.Kind, 0xFFFFFFFF, "TRADE_RES Kind") && ok;
    ok      = expectEqualInt(packet.TradeCounter, 0xFFFE, "TRADE_RES TradeCounter") && ok;
    return ok;
}

auto testTradeResPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(tradeResKindValidation(enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::Start)), "TRADE_RES Start validation") && ok;
    ok      = expectValid(tradeResKindValidation(enumValue(GP_CLI_COMMAND_TRADE_RES_KIND::MakeCancell)), "TRADE_RES MakeCancell validation") && ok;
    ok      = expectInvalidError(tradeResKindValidation(4), "4 not a valid GP_CLI_COMMAND_TRADE_RES_KIND value.", "TRADE_RES invalid Kind validation") && ok;
    ok      = expectInvalidError(tradeResKindValidation(0xFFFFFFFF), "4294967295 not a valid GP_CLI_COMMAND_TRADE_RES_KIND value.", "TRADE_RES max Kind validation") && ok;
    return ok;
}

auto testTradeListLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_TRADE_LIST::name, "GP_CLI_COMMAND_TRADE_LIST", "TRADE_LIST static name") && ok;
    ok = expectTrue(GP_CLI_COMMAND_TRADE_LIST::packetId == PacketC2S::GP_CLI_COMMAND_TRADE_LIST, "TRADE_LIST static packetId") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_TRADE_LIST), 12, "sizeof(GP_CLI_COMMAND_TRADE_LIST)") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_LIST, header), 0, "TRADE_LIST header offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_LIST, ItemNum), 4, "TRADE_LIST ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_LIST, ItemNo), 8, "TRADE_LIST ItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_LIST, ItemIndex), 10, "TRADE_LIST ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_TRADE_LIST, TradeIndex), 11, "TRADE_LIST TradeIndex offset") && ok;
    ok = expectBytes(encodedTradeList(0x11223344, 0x5566, 0x77, 0x08),
                     ListPacketBytes{ 0x34, 0x06, 0xEF, 0xBE,
                                      0x44, 0x33, 0x22, 0x11,
                                      0x66, 0x55, 0x77, 0x08 },
                     "TRADE_LIST encoded packet") &&
         ok;

    return ok;
}

auto testTradeListPayloadStorage() -> bool
{
    auto packet        = GP_CLI_COMMAND_TRADE_LIST{};
    packet.ItemNum    = 0xFFFFFFFF;
    packet.ItemNo     = 0xFFFE;
    packet.ItemIndex  = 0xFE;
    packet.TradeIndex = 0xFF;

    bool ok = true;
    ok      = expectEqualInt(packet.ItemNum, 0xFFFFFFFF, "TRADE_LIST ItemNum") && ok;
    ok      = expectEqualInt(packet.ItemNo, 0xFFFE, "TRADE_LIST ItemNo") && ok;
    ok      = expectEqualInt(packet.ItemIndex, 0xFE, "TRADE_LIST ItemIndex") && ok;
    ok      = expectEqualInt(packet.TradeIndex, 0xFF, "TRADE_LIST TradeIndex") && ok;
    return ok;
}

auto testTradeListPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(tradeListPureValidation(0), "TRADE_LIST first slot validation") && ok;
    ok      = expectValid(tradeListPureValidation(8), "TRADE_LIST last slot validation") && ok;
    ok      = expectInvalidError(tradeListPureValidation(9), "TradeIndex out of range: 9 not in [0, 8]", "TRADE_LIST one-past slot validation") && ok;
    ok      = expectInvalidError(tradeListPureValidation(0xFF), "TradeIndex out of range: 255 not in [0, 8]", "TRADE_LIST max slot validation") && ok;
    return ok;
}

auto testTradeListRuntimePlan() -> bool
{
    using namespace tradelisthelpers;

    const auto validItem = ItemFacts{
        .exists             = true,
        .matchesRequestedID = true,
        .requestedQuantity  = 2,
        .reservedQuantity   = 1,
        .availableQuantity  = 3,
    };

    bool ok = true;
    ok      = expectTrue(makePlan(false, true, validItem).outcome == Outcome::InvalidTarget, "TRADE_LIST rejects mismatched targets") && ok;
    ok      = expectTrue(!makePlan(false, true, validItem).releaseExistingOffer, "TRADE_LIST preserves offer when targets mismatch") && ok;
    ok      = expectTrue(makePlan(true, true, ItemFacts{}).outcome == Outcome::InvalidItem, "TRADE_LIST rejects missing item") && ok;
    ok      = expectTrue(makePlan(true, true, ItemFacts{}).releaseExistingOffer, "TRADE_LIST releases existing offer before item rejection") && ok;
    ok      = expectTrue(makePlan(true, false, ItemFacts{ .exists = true, .matchesRequestedID = true, .exclusive = true }).outcome == Outcome::InvalidItem,
                         "TRADE_LIST rejects exclusive item") && ok;
    ok      = expectTrue(makePlan(true, false, ItemFacts{ .exists = true, .matchesRequestedID = true, .locked = true }).outcome == Outcome::InvalidItem,
                         "TRADE_LIST rejects locked item") && ok;
    ok      = expectTrue(makePlan(true, false, ItemFacts{ .exists = true, .matchesRequestedID = true, .requestedQuantity = 3, .reservedQuantity = 1, .availableQuantity = 3 }).outcome == Outcome::InvalidItem,
                         "TRADE_LIST rejects insufficient quantity") && ok;
    ok      = expectTrue(makePlan(true, false, ItemFacts{ .exists = true, .matchesRequestedID = true, .linkshell = true }).outcome == Outcome::LinkshellNotEquipped,
                         "TRADE_LIST requires equipped linkshell") && ok;
    ok      = expectTrue(makePlan(true, false, ItemFacts{ .exists = true, .matchesRequestedID = true, .linkshell = true, .linkshellEquipped = true }).outcome == Outcome::Update,
                         "TRADE_LIST accepts equipped linkshell") && ok;
    ok      = expectTrue(makePlan(true, false, validItem).outcome == Outcome::Update, "TRADE_LIST accepts valid item") && ok;
    return ok;
}

} // namespace

auto runC2SPlayerTradePacketSelfTests() -> bool
{
    bool ok = true;

    ok = testTradeReqLayoutAndMetadata() && ok;
    ok = testTradeReqPayloadStorage() && ok;
    ok = testTradeReqPureValidationFacts() && ok;
    ok = testTradeResLayoutAndMetadata() && ok;
    ok = testTradeResKindEnumValues() && ok;
    ok = testTradeResPayloadStorage() && ok;
    ok = testTradeResPureValidationFacts() && ok;
    ok = testTradeListLayoutAndMetadata() && ok;
    ok = testTradeListPayloadStorage() && ok;
    ok = testTradeListPureValidationFacts() && ok;
    ok = testTradeListRuntimePlan() && ok;

    return ok;
}
