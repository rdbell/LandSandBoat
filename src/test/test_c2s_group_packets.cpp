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

#include "test_c2s_group_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/enums/party_kind.h"
#include "map/packets/c2s/0x06e_group_solicit_req.h"
#include "map/packets/c2s/0x06f_group_leave.h"
#include "map/packets/c2s/0x070_group_breakup.h"
#include "map/packets/c2s/0x071_group_strike.h"
#include "map/packets/c2s/0x074_group_solicit_res.h"
#include "map/packets/c2s/0x076_group_list_req.h"
#include "map/packets/c2s/0x077_group_change2.h"
#include "map/packets/c2s/0x078_group_checkid.h"

namespace
{

constexpr auto GroupLeaveRoundedPacketSize   = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GROUP_LEAVE)));
constexpr auto GroupBreakupRoundedPacketSize = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GROUP_BREAKUP)));
constexpr auto GroupListReqRoundedPacketSize = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GROUP_LIST_REQ)));
constexpr auto GroupChange2RoundedPacketSize = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GROUP_CHANGE2)));

using GroupSolicitReqBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ)>;
using GroupLeaveBytes      = std::array<std::uint8_t, GroupLeaveRoundedPacketSize>;
using GroupBreakupBytes    = std::array<std::uint8_t, GroupBreakupRoundedPacketSize>;
using GroupStrikeBytes     = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GROUP_STRIKE)>;
using GroupSolicitResBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_RES)>;
using GroupListReqBytes    = std::array<std::uint8_t, GroupListReqRoundedPacketSize>;
using GroupChange2Bytes    = std::array<std::uint8_t, GroupChange2RoundedPacketSize>;
using GroupCheckIDBytes    = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GROUP_CHECKID)>;
using GroupStrikeName      = std::array<std::uint8_t, 15>;
using GroupChange2Name     = std::array<std::uint8_t, 16>;

const std::set validLinkshellChange2Kinds = {
    GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell1,
    GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell2,
};

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s group packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s group packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s group packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s group packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s group packet self-test failed: " << label << " got";
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

template <typename Packet, std::size_t N>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, N>
{
    auto bytes = std::array<std::uint8_t, N>{};
    std::memcpy(bytes.data(), &packet, sizeof(packet));
    return bytes;
}

auto testStrikeName() -> GroupStrikeName
{
    return GroupStrikeName{ 'M', 'e', 'm', 'b', 'e', 'r', 0x00, 'A', 'B', 'C', 0xFF, 0x01, 0x02, 0x03, 0x04 };
}

auto testChange2Name() -> GroupChange2Name
{
    return GroupChange2Name{ 'L', 'e', 'a', 'd', 'e', 'r', 0x00, 'R', 'o', 'l', 'e', 0x01, 0x02, 0x03, 0x04, 0x05 };
}

auto makeGroupSolicitReqPacket() -> GP_CLI_COMMAND_GROUP_SOLICIT_REQ
{
    auto packet         = GP_CLI_COMMAND_GROUP_SOLICIT_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_SOLICIT_REQ);
    packet.header.size = sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x5566;
    packet.Kind        = PartyKind::Alliance;
    packet.padding00   = 0x77;
    return packet;
}

auto makeGroupLeavePacket() -> GP_CLI_COMMAND_GROUP_LEAVE
{
    auto packet         = GP_CLI_COMMAND_GROUP_LEAVE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_LEAVE);
    packet.header.size = GroupLeaveRoundedPacketSize / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = PartyKind::Alliance;
    return packet;
}

auto makeGroupBreakupPacket() -> GP_CLI_COMMAND_GROUP_BREAKUP
{
    auto packet         = GP_CLI_COMMAND_GROUP_BREAKUP{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_BREAKUP);
    packet.header.size = GroupBreakupRoundedPacketSize / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = PartyKind::Party;
    return packet;
}

auto makeGroupStrikePacket() -> GP_CLI_COMMAND_GROUP_STRIKE
{
    auto packet         = GP_CLI_COMMAND_GROUP_STRIKE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_STRIKE);
    packet.header.size = sizeof(GP_CLI_COMMAND_GROUP_STRIKE) / 4U;
    packet.header.sync = 0xBEEF;
    packet.UniqueNo    = 0x11223344;
    packet.ActIndex    = 0x5566;
    packet.Kind        = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_STRIKE_KIND::Linkshell2);
    packet.padding00   = 0x77;

    const auto name = testStrikeName();
    std::memcpy(packet.sName, name.data(), name.size());
    return packet;
}

auto makeGroupSolicitResPacket() -> GP_CLI_COMMAND_GROUP_SOLICIT_RES
{
    auto packet         = GP_CLI_COMMAND_GROUP_SOLICIT_RES{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_SOLICIT_RES);
    packet.header.size = sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_RES) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Res         = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_SOLICIT_RES_RES::Accept);
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;
    return packet;
}

auto makeGroupListReqPacket() -> GP_CLI_COMMAND_GROUP_LIST_REQ
{
    auto packet         = GP_CLI_COMMAND_GROUP_LIST_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_LIST_REQ);
    packet.header.size = GroupListReqRoundedPacketSize / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = 0;
    return packet;
}

auto makeGroupChange2Packet() -> GP_CLI_COMMAND_GROUP_CHANGE2
{
    auto packet         = GP_CLI_COMMAND_GROUP_CHANGE2{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_CHANGE2);
    packet.header.size = GroupChange2RoundedPacketSize / 4U;
    packet.header.sync = 0xBEEF;

    const auto name = testChange2Name();
    std::memcpy(packet.sName, name.data(), name.size());
    packet.Kind       = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Alliance);
    packet.ChangeKind = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetAllianceLeader);
    return packet;
}

auto makeGroupCheckIDPacket() -> GP_CLI_COMMAND_GROUP_CHECKID
{
    auto packet         = GP_CLI_COMMAND_GROUP_CHECKID{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_CHECKID);
    packet.header.size = sizeof(GP_CLI_COMMAND_GROUP_CHECKID) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto validatePartyKindPure(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<PartyKind>(kind);
    return validator;
}

auto validateGroupStrikePure(std::uint32_t uniqueNo, std::uint16_t actIndex, std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf<GP_CLI_COMMAND_GROUP_STRIKE_KIND>(kind)
        .mustEqual(actIndex, 0, "ActIndex not 0")
        .mustEqual(uniqueNo, 0, "UniqueNo not 0");
    return validator;
}

auto validateGroupSolicitResPure(std::uint8_t res) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_GROUP_SOLICIT_RES_RES>(res);
    return validator;
}

auto validateGroupListReqPure(std::uint8_t kind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(kind, 0, "Kind not 0");
    return validator;
}

auto validateGroupChange2Pure(std::uint8_t kind, std::uint8_t changeKind) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator
        .oneOf<GP_CLI_COMMAND_GROUP_CHANGE2_KIND>(kind)
        .oneOf<GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND>(changeKind);

    switch (static_cast<GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND>(changeKind))
    {
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetPartyLeader:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetQuartermaster:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetLottery:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetLevelSync:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::DisableLevelSync:
        {
            validator.mustEqual(kind, static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Party), "Invalid operation");
        }
        break;
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::PearlToSack:
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SackToPearl:
        {
            validator.oneOf("Kind", static_cast<GP_CLI_COMMAND_GROUP_CHANGE2_KIND>(kind), validLinkshellChange2Kinds);
        }
        break;
        case GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetAllianceLeader:
        {
            validator.mustEqual(kind, static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Alliance), "Invalid operation");
        }
        break;
    }

    return validator;
}

auto testGroupLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_GROUP_SOLICIT_REQ::name, "GP_CLI_COMMAND_GROUP_SOLICIT_REQ", "GROUP_SOLICIT_REQ name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_SOLICIT_REQ::packetId), 0x06E, "GROUP_SOLICIT_REQ packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ), 12, "GROUP_SOLICIT_REQ sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ, UniqueNo), 4, "GROUP_SOLICIT_REQ UniqueNo offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ, ActIndex), 8, "GROUP_SOLICIT_REQ ActIndex offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ, Kind), 10, "GROUP_SOLICIT_REQ Kind offset") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ, padding00), 11, "GROUP_SOLICIT_REQ padding00 offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_LEAVE::name, "GP_CLI_COMMAND_GROUP_LEAVE", "GROUP_LEAVE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_LEAVE::packetId), 0x06F, "GROUP_LEAVE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_LEAVE), 6, "GROUP_LEAVE sizeof") && ok;
    ok = expectEqualInt(GroupLeaveRoundedPacketSize, 8, "GROUP_LEAVE rounded size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_LEAVE, Kind), 4, "GROUP_LEAVE Kind offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_BREAKUP::name, "GP_CLI_COMMAND_GROUP_BREAKUP", "GROUP_BREAKUP name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_BREAKUP::packetId), 0x070, "GROUP_BREAKUP packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_BREAKUP), 6, "GROUP_BREAKUP sizeof") && ok;
    ok = expectEqualInt(GroupBreakupRoundedPacketSize, 8, "GROUP_BREAKUP rounded size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_BREAKUP, Kind), 4, "GROUP_BREAKUP Kind offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_STRIKE::name, "GP_CLI_COMMAND_GROUP_STRIKE", "GROUP_STRIKE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_STRIKE::packetId), 0x071, "GROUP_STRIKE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_STRIKE), 28, "GROUP_STRIKE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_STRIKE, UniqueNo), 4, "GROUP_STRIKE UniqueNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_STRIKE, ActIndex), 8, "GROUP_STRIKE ActIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_STRIKE, Kind), 10, "GROUP_STRIKE Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_STRIKE, padding00), 11, "GROUP_STRIKE padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_STRIKE, sName), 12, "GROUP_STRIKE sName offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_STRIKE::sName), 15, "GROUP_STRIKE sName bytes") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_SOLICIT_RES::name, "GP_CLI_COMMAND_GROUP_SOLICIT_RES", "GROUP_SOLICIT_RES name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_SOLICIT_RES::packetId), 0x074, "GROUP_SOLICIT_RES packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_RES), 8, "GROUP_SOLICIT_RES sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_SOLICIT_RES, Res), 4, "GROUP_SOLICIT_RES Res offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_SOLICIT_RES, padding00), 5, "GROUP_SOLICIT_RES padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_RES::padding00), 3, "GROUP_SOLICIT_RES padding00 bytes") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_LIST_REQ::name, "GP_CLI_COMMAND_GROUP_LIST_REQ", "GROUP_LIST_REQ name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_LIST_REQ::packetId), 0x076, "GROUP_LIST_REQ packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_LIST_REQ), 6, "GROUP_LIST_REQ sizeof") && ok;
    ok = expectEqualInt(GroupListReqRoundedPacketSize, 8, "GROUP_LIST_REQ rounded size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_LIST_REQ, Kind), 4, "GROUP_LIST_REQ Kind offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_CHANGE2::name, "GP_CLI_COMMAND_GROUP_CHANGE2", "GROUP_CHANGE2 name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_CHANGE2::packetId), 0x077, "GROUP_CHANGE2 packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_CHANGE2), 22, "GROUP_CHANGE2 sizeof") && ok;
    ok = expectEqualInt(GroupChange2RoundedPacketSize, 24, "GROUP_CHANGE2 rounded size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_CHANGE2, sName), 4, "GROUP_CHANGE2 sName offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_CHANGE2::sName), 16, "GROUP_CHANGE2 sName bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_CHANGE2, Kind), 20, "GROUP_CHANGE2 Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_CHANGE2, ChangeKind), 21, "GROUP_CHANGE2 ChangeKind offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_CHECKID::name, "GP_CLI_COMMAND_GROUP_CHECKID", "GROUP_CHECKID name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_CHECKID::packetId), 0x078, "GROUP_CHECKID packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_CHECKID), 4, "GROUP_CHECKID sizeof") && ok;
    return ok;
}

auto testGroupEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    const auto solicitReq = makeGroupSolicitReqPacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_SOLICIT_REQ, sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_REQ)>(solicitReq),
                     GroupSolicitReqBytes{ 0x6E, 0x06, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11, 0x66, 0x55, 0x05, 0x77 },
                     "GROUP_SOLICIT_REQ encoded packet") &&
         ok;
    ok = expectEqualInt(solicitReq.UniqueNo, 0x11223344, "GROUP_SOLICIT_REQ UniqueNo") && ok;
    ok = expectEqualInt(solicitReq.ActIndex, 0x5566, "GROUP_SOLICIT_REQ ActIndex") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(solicitReq.Kind), static_cast<std::uint8_t>(PartyKind::Alliance), "GROUP_SOLICIT_REQ Kind") && ok;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_LEAVE, GroupLeaveRoundedPacketSize>(makeGroupLeavePacket()),
                     GroupLeaveBytes{ 0x6F, 0x04, 0xEF, 0xBE, 0x05, 0x00, 0x00, 0x00 },
                     "GROUP_LEAVE encoded packet") &&
         ok;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_BREAKUP, GroupBreakupRoundedPacketSize>(makeGroupBreakupPacket()),
                     GroupBreakupBytes{ 0x70, 0x04, 0xEF, 0xBE, 0x00, 0x00, 0x00, 0x00 },
                     "GROUP_BREAKUP encoded packet") &&
         ok;

    const auto strike = makeGroupStrikePacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_STRIKE, sizeof(GP_CLI_COMMAND_GROUP_STRIKE)>(strike),
                     GroupStrikeBytes{ 0x71, 0x0E, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11,
                                       0x66, 0x55, 0x02, 0x77, 'M', 'e', 'm', 'b',
                                       'e', 'r', 0x00, 'A', 'B', 'C', 0xFF, 0x01,
                                       0x02, 0x03, 0x04, 0x00 },
                     "GROUP_STRIKE encoded packet") &&
         ok;
    ok = expectEqualInt(strike.UniqueNo, 0x11223344, "GROUP_STRIKE UniqueNo") && ok;
    ok = expectEqualInt(strike.ActIndex, 0x5566, "GROUP_STRIKE ActIndex") && ok;
    ok = expectEqualInt(strike.Kind, static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_STRIKE_KIND::Linkshell2), "GROUP_STRIKE Kind") && ok;
    auto strikeName = GroupStrikeName{};
    std::memcpy(strikeName.data(), strike.sName, strikeName.size());
    ok = expectBytes(strikeName, testStrikeName(), "GROUP_STRIKE sName raw bytes") && ok;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_SOLICIT_RES, sizeof(GP_CLI_COMMAND_GROUP_SOLICIT_RES)>(makeGroupSolicitResPacket()),
                     GroupSolicitResBytes{ 0x74, 0x04, 0xEF, 0xBE, 0x01, 0xAA, 0xBB, 0xCC },
                     "GROUP_SOLICIT_RES encoded packet") &&
         ok;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_LIST_REQ, GroupListReqRoundedPacketSize>(makeGroupListReqPacket()),
                     GroupListReqBytes{ 0x76, 0x04, 0xEF, 0xBE, 0x00, 0x00, 0x00, 0x00 },
                     "GROUP_LIST_REQ encoded packet") &&
         ok;

    const auto change2 = makeGroupChange2Packet();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_CHANGE2, GroupChange2RoundedPacketSize>(change2),
                     GroupChange2Bytes{ 0x77, 0x0C, 0xEF, 0xBE, 'L', 'e', 'a', 'd',
                                        'e', 'r', 0x00, 'R', 'o', 'l', 'e', 0x01,
                                        0x02, 0x03, 0x04, 0x05, 0x05, 0x01, 0x00, 0x00 },
                     "GROUP_CHANGE2 encoded packet") &&
         ok;
    auto change2Name = GroupChange2Name{};
    std::memcpy(change2Name.data(), change2.sName, change2Name.size());
    ok = expectBytes(change2Name, testChange2Name(), "GROUP_CHANGE2 sName raw bytes") && ok;

    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_CHECKID, sizeof(GP_CLI_COMMAND_GROUP_CHECKID)>(makeGroupCheckIDPacket()),
                     GroupCheckIDBytes{ 0x78, 0x02, 0xEF, 0xBE },
                     "GROUP_CHECKID encoded packet") &&
         ok;

    return ok;
}

auto testGroupEnumsAndPureValidation() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint8_t>(PartyKind::Party), 0, "PartyKind::Party") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(PartyKind::Alliance), 5, "PartyKind::Alliance") && ok;
    ok = expectValid(validatePartyKindPure(0), "PartyKind party validation") && ok;
    ok = expectValid(validatePartyKindPure(5), "PartyKind alliance validation") && ok;
    ok = expectInvalidError(validatePartyKindPure(1), "1 not a valid PartyKind value.", "PartyKind invalid validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_STRIKE_KIND::Party), 0, "GROUP_STRIKE_KIND::Party") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_STRIKE_KIND::Linkshell1), 1, "GROUP_STRIKE_KIND::Linkshell1") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_STRIKE_KIND::Linkshell2), 2, "GROUP_STRIKE_KIND::Linkshell2") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_STRIKE_KIND::Alliance), 5, "GROUP_STRIKE_KIND::Alliance") && ok;
    ok = expectValid(validateGroupStrikePure(0, 0, 0), "GROUP_STRIKE valid party validation") && ok;
    ok = expectInvalidError(validateGroupStrikePure(0, 0, 3), "3 not a valid GP_CLI_COMMAND_GROUP_STRIKE_KIND value.", "GROUP_STRIKE invalid kind validation") && ok;
    ok = expectInvalidError(validateGroupStrikePure(0, 1, 0), "ActIndex not 0", "GROUP_STRIKE ActIndex validation") && ok;
    ok = expectInvalidError(validateGroupStrikePure(1, 0, 0), "UniqueNo not 0", "GROUP_STRIKE UniqueNo validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_SOLICIT_RES_RES::Decline), 0, "GROUP_SOLICIT_RES_RES::Decline") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_SOLICIT_RES_RES::Accept), 1, "GROUP_SOLICIT_RES_RES::Accept") && ok;
    ok = expectValid(validateGroupSolicitResPure(0), "GROUP_SOLICIT_RES decline validation") && ok;
    ok = expectValid(validateGroupSolicitResPure(1), "GROUP_SOLICIT_RES accept validation") && ok;
    ok = expectInvalidError(validateGroupSolicitResPure(2), "2 not a valid GP_CLI_COMMAND_GROUP_SOLICIT_RES_RES value.", "GROUP_SOLICIT_RES invalid validation") && ok;

    ok = expectValid(validateGroupListReqPure(0), "GROUP_LIST_REQ Kind zero validation") && ok;
    ok = expectInvalidError(validateGroupListReqPure(1), "Kind not 0", "GROUP_LIST_REQ Kind one validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Party), 0, "GROUP_CHANGE2_KIND::Party") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell1), 1, "GROUP_CHANGE2_KIND::Linkshell1") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Linkshell2), 2, "GROUP_CHANGE2_KIND::Linkshell2") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_KIND::Alliance), 5, "GROUP_CHANGE2_KIND::Alliance") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetPartyLeader), 0, "ChangeKind::SetPartyLeader") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetAllianceLeader), 1, "ChangeKind::SetAllianceLeader") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::PearlToSack), 2, "ChangeKind::PearlToSack") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SackToPearl), 3, "ChangeKind::SackToPearl") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetQuartermaster), 4, "ChangeKind::SetQuartermaster") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetLottery), 5, "ChangeKind::SetLottery") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::SetLevelSync), 6, "ChangeKind::SetLevelSync") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND::DisableLevelSync), 7, "ChangeKind::DisableLevelSync") && ok;
    ok = expectValid(validateGroupChange2Pure(0, 6), "GROUP_CHANGE2 party op validation") && ok;
    ok = expectValid(validateGroupChange2Pure(1, 2), "GROUP_CHANGE2 linkshell1 op validation") && ok;
    ok = expectValid(validateGroupChange2Pure(2, 3), "GROUP_CHANGE2 linkshell2 op validation") && ok;
    ok = expectValid(validateGroupChange2Pure(5, 1), "GROUP_CHANGE2 alliance op validation") && ok;
    ok = expectInvalidError(validateGroupChange2Pure(3, 0), "3 not a valid GP_CLI_COMMAND_GROUP_CHANGE2_KIND value.", "GROUP_CHANGE2 invalid kind validation") && ok;
    ok = expectInvalidError(validateGroupChange2Pure(0, 8), "8 not a valid GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND value.", "GROUP_CHANGE2 invalid change kind validation") && ok;
    ok = expectInvalidError(validateGroupChange2Pure(5, 0), "Invalid operation", "GROUP_CHANGE2 party op wrong kind validation") && ok;
    ok = expectInvalidError(validateGroupChange2Pure(0, 2), "Kind value 0 is not allowed.", "GROUP_CHANGE2 linkshell op wrong kind validation") && ok;
    ok = expectInvalidError(validateGroupChange2Pure(0, 1), "Invalid operation", "GROUP_CHANGE2 alliance op wrong kind validation") && ok;

    return ok;
}

} // namespace

auto runC2SGroupPacketSelfTests() -> bool
{
    return testGroupLayoutsAndMetadata() &&
           testGroupEncodedBytesAndPayloads() &&
           testGroupEnumsAndPureValidation();
}
