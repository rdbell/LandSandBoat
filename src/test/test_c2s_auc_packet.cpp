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

#include "test_c2s_auc_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/entities/char_entity.h"
#include "map/packets/c2s/0x04e_auc.h"

namespace
{

constexpr auto aucCommandOffset      = 4U;
constexpr auto aucWorkIndexOffset    = 5U;
constexpr auto aucResultOffset       = 6U;
constexpr auto aucResultStatusOffset = 7U;
constexpr auto aucParamOffset        = 8U;
constexpr auto aucParamSize          = 12U;
constexpr auto aucParcelOffset       = 20U;
constexpr auto aucParcelSize         = 40U;
constexpr auto aucNameLen            = 16U;

constexpr auto aucParamPriceOffset         = 0U;
constexpr auto aucParamItemWorkIndexOffset = 4U;
constexpr auto aucParamPadding00Offset     = 6U;
constexpr auto aucParamItemStacksOffset    = 8U;
constexpr auto aucTransTotalFragmentsOffset = 10U;

constexpr auto aucParcelStatOffset         = 0U;
constexpr auto aucParcelNameOffset         = 4U;
constexpr auto aucParcelItemNoOffset       = 20U;
constexpr auto aucParcelItemQuantityOffset = 22U;
constexpr auto aucParcelItemCategoryOffset = 23U;
constexpr auto aucParcelPriceOffset        = 24U;
constexpr auto aucParcelMarketNoOffset     = 28U;
constexpr auto aucParcelLotNoOffset        = 32U;
constexpr auto aucParcelTimeStampOffset    = 36U;

using AUCBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_AUC)>;

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s AUC packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s AUC packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s AUC packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s AUC packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s AUC packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s AUC packet self-test failed: " << label << " got";
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

auto makeAUCPacket() -> GP_CLI_COMMAND_AUC
{
    auto packet          = GP_CLI_COMMAND_AUC{};
    packet.header.id    = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_AUC);
    packet.header.size  = sizeof(packet) / 4U;
    packet.header.sync  = 0xBEEF;
    packet.Command      = GP_CLI_COMMAND_AUC_COMMAND::AskCommit;
    packet.AucWorkIndex = -2;
    packet.Result       = -3;
    packet.ResultStatus = -4;

    packet.Param.AskCommit.Commission    = 0x01020304;
    packet.Param.AskCommit.ItemWorkIndex = 0x0506;
    packet.Param.AskCommit.ItemNo        = 0x0708;
    packet.Param.AskCommit.ItemStacks    = 0x090A0B0C;

    packet.Parcel.Stat      = 0x11;
    packet.Parcel.padding00 = 0x12;
    packet.Parcel.ItemIndex = 0x13;
    packet.Parcel.padding01 = 0x14;

    const auto name = std::array<std::uint8_t, aucNameLen>{
        'A',
        'u',
        'c',
        't',
        'i',
        'o',
        'n',
        0x00,
        0xFE,
        0xFD,
        'B',
        'o',
        'x',
        0x80,
        '!',
        0x00,
    };
    std::memcpy(packet.Parcel.Name, name.data(), name.size());

    packet.Parcel.ItemNo       = 0x1516;
    packet.Parcel.ItemQuantity = 0x17;
    packet.Parcel.ItemCategory = 0x18;
    packet.Parcel.Price        = 0x191A1B1C;
    packet.Parcel.MarketNo     = 0x1D1E1F20;
    packet.Parcel.LotNo        = 0x21222324;
    packet.Parcel.TimeStamp    = 0x25262728;
    return packet;
}

auto makeExpectedAUCBytes() -> AUCBytes
{
    auto bytes = AUCBytes{};
    bytes[0] = 0x4E;
    bytes[1] = 0x1E;
    bytes[2] = 0xEF;
    bytes[3] = 0xBE;
    bytes[aucCommandOffset] = static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::AskCommit);
    bytes[aucWorkIndexOffset] = 0xFE;
    bytes[aucResultOffset] = 0xFD;
    bytes[aucResultStatusOffset] = 0xFC;
    bytes[aucParamOffset + 0U] = 0x04;
    bytes[aucParamOffset + 1U] = 0x03;
    bytes[aucParamOffset + 2U] = 0x02;
    bytes[aucParamOffset + 3U] = 0x01;
    bytes[aucParamOffset + 4U] = 0x06;
    bytes[aucParamOffset + 5U] = 0x05;
    bytes[aucParamOffset + 6U] = 0x08;
    bytes[aucParamOffset + 7U] = 0x07;
    bytes[aucParamOffset + 8U] = 0x0C;
    bytes[aucParamOffset + 9U] = 0x0B;
    bytes[aucParamOffset + 10U] = 0x0A;
    bytes[aucParamOffset + 11U] = 0x09;
    bytes[aucParcelOffset + 0U] = 0x11;
    bytes[aucParcelOffset + 1U] = 0x12;
    bytes[aucParcelOffset + 2U] = 0x13;
    bytes[aucParcelOffset + 3U] = 0x14;

    const auto name = std::array<std::uint8_t, aucNameLen>{
        'A',
        'u',
        'c',
        't',
        'i',
        'o',
        'n',
        0x00,
        0xFE,
        0xFD,
        'B',
        'o',
        'x',
        0x80,
        '!',
        0x00,
    };
    std::memcpy(bytes.data() + aucParcelOffset + aucParcelNameOffset, name.data(), name.size());
    bytes[aucParcelOffset + aucParcelItemNoOffset] = 0x16;
    bytes[aucParcelOffset + aucParcelItemNoOffset + 1U] = 0x15;
    bytes[aucParcelOffset + aucParcelItemQuantityOffset] = 0x17;
    bytes[aucParcelOffset + aucParcelItemCategoryOffset] = 0x18;
    bytes[aucParcelOffset + aucParcelPriceOffset] = 0x1C;
    bytes[aucParcelOffset + aucParcelPriceOffset + 1U] = 0x1B;
    bytes[aucParcelOffset + aucParcelPriceOffset + 2U] = 0x1A;
    bytes[aucParcelOffset + aucParcelPriceOffset + 3U] = 0x19;
    bytes[aucParcelOffset + aucParcelMarketNoOffset] = 0x20;
    bytes[aucParcelOffset + aucParcelMarketNoOffset + 1U] = 0x1F;
    bytes[aucParcelOffset + aucParcelMarketNoOffset + 2U] = 0x1E;
    bytes[aucParcelOffset + aucParcelMarketNoOffset + 3U] = 0x1D;
    bytes[aucParcelOffset + aucParcelLotNoOffset] = 0x24;
    bytes[aucParcelOffset + aucParcelLotNoOffset + 1U] = 0x23;
    bytes[aucParcelOffset + aucParcelLotNoOffset + 2U] = 0x22;
    bytes[aucParcelOffset + aucParcelLotNoOffset + 3U] = 0x21;
    bytes[aucParcelOffset + aucParcelTimeStampOffset] = 0x28;
    bytes[aucParcelOffset + aucParcelTimeStampOffset + 1U] = 0x27;
    bytes[aucParcelOffset + aucParcelTimeStampOffset + 2U] = 0x26;
    bytes[aucParcelOffset + aucParcelTimeStampOffset + 3U] = 0x25;
    return bytes;
}

auto validateAUCPacket(const GP_CLI_COMMAND_AUC& packet) -> PacketValidationResult
{
    auto character       = CCharEntity{};
    character.m_GMlevel = 1;
    return packet.validate(nullptr, &character);
}

auto baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND command) -> GP_CLI_COMMAND_AUC
{
    auto packet    = GP_CLI_COMMAND_AUC{};
    packet.Command = command;
    return packet;
}

auto validAUCAskCommit(std::uint32_t commission, std::uint32_t itemStacks) -> GP_CLI_COMMAND_AUC
{
    auto packet                         = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::AskCommit);
    packet.Param.AskCommit.Commission  = commission;
    packet.Param.AskCommit.ItemStacks  = itemStacks;
    return packet;
}

auto validAUCLotIn(std::int8_t aucWorkIndex, std::uint32_t limitPrice, std::uint32_t itemStacks) -> GP_CLI_COMMAND_AUC
{
    auto packet                    = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::LotIn);
    packet.AucWorkIndex            = aucWorkIndex;
    packet.Param.LotIn.LimitPrice  = limitPrice;
    packet.Param.LotIn.ItemStacks  = itemStacks;
    return packet;
}

auto validAUCBid(std::int8_t aucWorkIndex, std::uint32_t bidPrice, std::uint32_t itemStacks) -> GP_CLI_COMMAND_AUC
{
    auto packet                 = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::Bid);
    packet.AucWorkIndex         = aucWorkIndex;
    packet.Param.Bid.BidPrice   = bidPrice;
    packet.Param.Bid.ItemStacks = itemStacks;
    return packet;
}

auto testAUCLayoutAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_AUC::name, "GP_CLI_COMMAND_AUC", "AUC name") && ok;
    ok = expectEqualUInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_AUC::packetId), 0x04E, "AUC packet id") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_HEADER), 4, "client header size") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM_LOT), 12, "LotIn sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM_BID), 12, "Bid sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM_SUMMARY), 8, "Summary sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM_HISTORY), 8, "History sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM_ASKCOMMIT), 12, "AskCommit sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM_TRANS), 12, "Trans sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_PARAM), aucParamSize, "Param sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_AUC_BOX), aucParcelSize, "Parcel sizeof") && ok;
    ok = expectEqualUInt(sizeof(GP_CLI_COMMAND_AUC), 60, "AUC sizeof") && ok;

    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_AUC, Command), aucCommandOffset, "Command offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_AUC, AucWorkIndex), aucWorkIndexOffset, "AucWorkIndex offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_AUC, Result), aucResultOffset, "Result offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_AUC, ResultStatus), aucResultStatusOffset, "ResultStatus offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_AUC, Param), aucParamOffset, "Param offset") && ok;
    ok = expectEqualUInt(offsetof(GP_CLI_COMMAND_AUC, Parcel), aucParcelOffset, "Parcel offset") && ok;

    ok = expectEqualUInt(offsetof(GP_AUC_PARAM_LOT, LimitPrice), aucParamPriceOffset, "LotIn LimitPrice offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_PARAM_LOT, ItemWorkIndex), aucParamItemWorkIndexOffset, "LotIn ItemWorkIndex offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_PARAM_LOT, padding00), aucParamPadding00Offset, "LotIn padding00 offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_PARAM_LOT, ItemStacks), aucParamItemStacksOffset, "LotIn ItemStacks offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_PARAM_TRANS, TotalFragments), aucTransTotalFragmentsOffset, "Trans TotalFragments offset") && ok;

    ok = expectEqualUInt(offsetof(GP_AUC_BOX, Stat), aucParcelStatOffset, "Parcel Stat offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_BOX, Name), aucParcelNameOffset, "Parcel Name offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_BOX, ItemNo), aucParcelItemNoOffset, "Parcel ItemNo offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_BOX, Price), aucParcelPriceOffset, "Parcel Price offset") && ok;
    ok = expectEqualUInt(offsetof(GP_AUC_BOX, TimeStamp), aucParcelTimeStampOffset, "Parcel TimeStamp offset") && ok;
    const auto parcel = GP_AUC_BOX{};
    ok = expectEqualUInt(sizeof(parcel.Name), aucNameLen, "Parcel Name len") && ok;

    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::Open), 0x02, "Open") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::AskCommit), 0x04, "AskCommit") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::Info), 0x05, "Info") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::WorkCheck), 0x0A, "WorkCheck") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::LotIn), 0x0B, "LotIn") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::LotCancel), 0x0C, "LotCancel") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::LotCheck), 0x0D, "LotCheck") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_AUC_COMMAND::Bid), 0x0E, "Bid") && ok;

    return ok;
}

auto testAUCEncodedBytes() -> bool
{
    return expectBytes(encodedPacketBytes(makeAUCPacket()), makeExpectedAUCBytes(), "AUC bytes");
}

auto testAUCValidation() -> bool
{
    bool ok = true;

    auto packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::Open);
    packet.AucWorkIndex = 42;
    ok = expectValid(validateAUCPacket(packet), "open") && ok;
    ok = expectValid(validateAUCPacket(validAUCAskCommit(1, 0)), "ask commit min") && ok;
    ok = expectValid(validateAUCPacket(validAUCAskCommit(999999999, 1)), "ask commit max") && ok;
    packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::Info);
    packet.AucWorkIndex = -7;
    ok = expectValid(validateAUCPacket(packet), "info") && ok;
    packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::WorkCheck);
    packet.AucWorkIndex = -1;
    ok = expectValid(validateAUCPacket(packet), "work check") && ok;
    ok = expectValid(validateAUCPacket(validAUCLotIn(0, 1, 0)), "lot in min") && ok;
    ok = expectValid(validateAUCPacket(validAUCLotIn(6, 999999999, 1)), "lot in max") && ok;
    ok = expectValid(validateAUCPacket(validAUCBid(0, 1, 0)), "bid min") && ok;
    ok = expectValid(validateAUCPacket(validAUCBid(6, 999999999, 1)), "bid max") && ok;
    packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::LotCancel);
    packet.AucWorkIndex = 6;
    ok = expectValid(validateAUCPacket(packet), "lot cancel") && ok;
    packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::LotCheck);
    packet.AucWorkIndex = 0;
    ok = expectValid(validateAUCPacket(packet), "lot check") && ok;

    packet = baseAUCPacket(static_cast<GP_CLI_COMMAND_AUC_COMMAND>(0));
    ok = expectInvalidError(validateAUCPacket(packet), "0 not a valid GP_CLI_COMMAND_AUC_COMMAND value.", "invalid command") && ok;

    packet = validAUCAskCommit(1, 0);
    packet.Result = 1;
    ok = expectInvalidError(validateAUCPacket(packet), "Result not 0", "result") && ok;

    packet = validAUCAskCommit(1, 0);
    packet.ResultStatus = 1;
    ok = expectInvalidError(validateAUCPacket(packet), "Result status", "result status") && ok;

    ok = expectInvalidError(validateAUCPacket(validAUCAskCommit(0, 0)), "Commission out of range: 0 not in [1, 999999999]", "ask commit commission") && ok;
    ok = expectInvalidError(validateAUCPacket(validAUCAskCommit(1, 2)), "ItemStacks out of range: 2 not in [0, 1]", "ask commit stacks") && ok;

    packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::WorkCheck);
    packet.AucWorkIndex = 0;
    ok = expectInvalidError(validateAUCPacket(packet), "AucWorkIndex not -1", "work check index") && ok;
    ok = expectInvalidError(validateAUCPacket(validAUCLotIn(-1, 1, 0)), "AucWorkIndex out of range: -1 not in [0, 6]", "lot in index") && ok;
    ok = expectInvalidError(validateAUCPacket(validAUCLotIn(0, 0, 0)), "LimitPrice out of range: 0 not in [1, 999999999]", "lot in limit") && ok;
    ok = expectInvalidError(validateAUCPacket(validAUCBid(0, 1000000000, 0)), "BidPrice out of range: 1000000000 not in [1, 999999999]", "bid price") && ok;
    ok = expectInvalidError(validateAUCPacket(validAUCBid(0, 1, 2)), "ItemStacks out of range: 2 not in [0, 1]", "bid stacks") && ok;

    packet = baseAUCPacket(GP_CLI_COMMAND_AUC_COMMAND::LotCancel);
    packet.AucWorkIndex = 7;
    ok = expectInvalidError(validateAUCPacket(packet), "AucWorkIndex out of range: 7 not in [0, 6]", "lot cancel index") && ok;

    return ok;
}

} // namespace

auto runC2SAUCPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testAUCLayoutAndMetadata() && ok;
    ok      = testAUCEncodedBytes() && ok;
    ok      = testAUCValidation() && ok;
    return ok;
}
