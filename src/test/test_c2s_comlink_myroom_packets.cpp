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

#include "test_c2s_comlink_myroom_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "common/utils.h"
#include "map/packets/c2s/0x0c3_group_comlink_make.h"
#include "map/packets/c2s/0x0c4_group_comlink_active.h"
#include "map/packets/c2s/0x0cb_myroom_is.h"

namespace
{

constexpr auto GroupComlinkMakeRoundedPacketSize = roundUpToNearestFour(static_cast<uint32>(sizeof(GP_CLI_COMMAND_GROUP_COMLINK_MAKE)));

using GroupComlinkMakeBytes   = std::array<std::uint8_t, GroupComlinkMakeRoundedPacketSize>;
using GroupComlinkActiveBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE)>;
using MyRoomIsBytes           = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_IS)>;
using ComlinkNameBytes        = std::array<std::uint8_t, 15>;

const std::set validMakeLinkshellIds = {
    GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell1,
    GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell2,
};

const std::set validActiveFlags = {
    GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG::Unequip,
    GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG::EquipOrCreate,
};

const std::set validActiveLinkshellIds = {
    GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID::Linkshell1,
    GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID::Linkshell2,
};

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s comlink/myroom packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s comlink/myroom packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s comlink/myroom packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s comlink/myroom packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s comlink/myroom packet self-test failed: " << label << " got";
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

auto testComlinkName() -> ComlinkNameBytes
{
    static constexpr auto source = std::string_view("Linkshell");
    auto                  name   = ComlinkNameBytes{};
    std::memcpy(name.data(), source.data(), source.size());
    return name;
}

auto validateGroupComlinkMakePure(std::uint8_t state, std::uint8_t linkshellId) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(state, 0, "State not 0")
        .oneOf("LinkshellId", static_cast<GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID>(linkshellId), validMakeLinkshellIds);
    return validator;
}

auto validateGroupComlinkActivePure(std::uint16_t r, std::uint16_t g, std::uint16_t b, std::uint16_t a, std::uint8_t activeFlg, std::uint8_t linkshellId) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("r", r, 0, 15)
        .range("g", g, 0, 15)
        .range("b", b, 0, 15)
        .mustEqual(a, 15, "a not 15")
        .oneOf("ActiveFlg", static_cast<GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG>(activeFlg), validActiveFlags)
        .oneOf("LinkshellId", static_cast<GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID>(linkshellId), validActiveLinkshellIds);
    return validator;
}

auto validateMyRoomIsPure(std::uint8_t kind, std::uint16_t param2) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_MYROOM_IS_KIND>(kind)
        .oneOf<GP_CLI_COMMAND_MYROOM_IS_PARAM2>(param2);
    return validator;
}

auto makeGroupComlinkMakePacket() -> GP_CLI_COMMAND_GROUP_COMLINK_MAKE
{
    auto packet         = GP_CLI_COMMAND_GROUP_COMLINK_MAKE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_COMLINK_MAKE);
    packet.header.size = GroupComlinkMakeRoundedPacketSize / 4U;
    packet.header.sync = 0xBEEF;
    packet.State       = 0;
    packet.LinkshellId = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell2);
    return packet;
}

auto makeGroupComlinkActivePacket() -> GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE
{
    auto packet         = GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE);
    packet.header.size = sizeof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE) / 4U;
    packet.header.sync = 0xBEEF;
    packet.r           = 1;
    packet.g           = 2;
    packet.b           = 3;
    packet.a           = 15;
    packet.ItemIndex   = 0x09;
    packet.Category    = 0x01;
    packet.ActiveFlg   = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG::EquipOrCreate);
    packet.padding00[0] = 0xAA;
    packet.padding00[1] = 0xBB;
    packet.padding00[2] = 0xCC;

    const auto name = testComlinkName();
    std::memcpy(packet.sComLinkName, name.data(), name.size());
    packet.LinkshellId = static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID::Linkshell1);
    return packet;
}

auto makeMyRoomIsPacket() -> GP_CLI_COMMAND_MYROOM_IS
{
    auto packet         = GP_CLI_COMMAND_MYROOM_IS{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_IS);
    packet.header.size = sizeof(GP_CLI_COMMAND_MYROOM_IS) / 4U;
    packet.header.sync = 0xBEEF;
    packet.Kind        = static_cast<std::uint8_t>(GP_CLI_COMMAND_MYROOM_IS_KIND::Remodel);
    packet.Param1      = 0x22;
    packet.Param2      = static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::MogPatio);
    return packet;
}

auto testComlinkMyRoomLayoutMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_COMLINK_MAKE::name, "GP_CLI_COMMAND_GROUP_COMLINK_MAKE", "GROUP_COMLINK_MAKE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_COMLINK_MAKE::packetId), 0x0C3, "GROUP_COMLINK_MAKE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_COMLINK_MAKE), 6, "GROUP_COMLINK_MAKE sizeof") && ok;
    ok = expectEqualInt(GroupComlinkMakeRoundedPacketSize, 8, "GROUP_COMLINK_MAKE rounded size") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_MAKE, State), 4, "GROUP_COMLINK_MAKE State offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_MAKE, LinkshellId), 5, "GROUP_COMLINK_MAKE LinkshellId offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE::name, "GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE", "GROUP_COMLINK_ACTIVE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE::packetId), 0x0C4, "GROUP_COMLINK_ACTIVE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE), 28, "GROUP_COMLINK_ACTIVE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, ItemIndex), 6, "GROUP_COMLINK_ACTIVE ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, Category), 7, "GROUP_COMLINK_ACTIVE Category offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, ActiveFlg), 8, "GROUP_COMLINK_ACTIVE ActiveFlg offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, padding00), 9, "GROUP_COMLINK_ACTIVE padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE::padding00), 3, "GROUP_COMLINK_ACTIVE padding00 bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, sComLinkName), 12, "GROUP_COMLINK_ACTIVE sComLinkName offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE::sComLinkName), 15, "GROUP_COMLINK_ACTIVE sComLinkName bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, LinkshellId), 27, "GROUP_COMLINK_ACTIVE LinkshellId offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_IS::name, "GP_CLI_COMMAND_MYROOM_IS", "MYROOM_IS name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS::packetId), 0x0CB, "MYROOM_IS packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_IS), 8, "MYROOM_IS sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_IS, Kind), 4, "MYROOM_IS Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_IS, Param1), 5, "MYROOM_IS Param1 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_IS, Param2), 6, "MYROOM_IS Param2 offset") && ok;
    return ok;
}

auto testComlinkMyRoomEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    const auto makePacket = makeGroupComlinkMakePacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_COMLINK_MAKE, GroupComlinkMakeRoundedPacketSize>(makePacket),
                     GroupComlinkMakeBytes{ 0xC3, 0x04, 0xEF, 0xBE, 0x00, 0x02, 0x00, 0x00 },
                     "GROUP_COMLINK_MAKE encoded packet") &&
         ok;
    ok = expectEqualInt(makePacket.State, 0, "GROUP_COMLINK_MAKE State payload") && ok;
    ok = expectEqualInt(makePacket.LinkshellId, 2, "GROUP_COMLINK_MAKE LinkshellId payload") && ok;

    const auto activePacket = makeGroupComlinkActivePacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE, sizeof(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE)>(activePacket),
                     GroupComlinkActiveBytes{ 0xC4, 0x0E, 0xEF, 0xBE, 0x21, 0xF3, 0x09, 0x01,
                                              0x01, 0xAA, 0xBB, 0xCC, 'L', 'i', 'n', 'k',
                                              's', 'h', 'e', 'l', 'l', 0x00, 0x00, 0x00,
                                              0x00, 0x00, 0x00, 0x01 },
                     "GROUP_COMLINK_ACTIVE encoded packet") &&
         ok;
    ok = expectEqualInt(activePacket.r, 1, "GROUP_COMLINK_ACTIVE r payload") && ok;
    ok = expectEqualInt(activePacket.g, 2, "GROUP_COMLINK_ACTIVE g payload") && ok;
    ok = expectEqualInt(activePacket.b, 3, "GROUP_COMLINK_ACTIVE b payload") && ok;
    ok = expectEqualInt(activePacket.a, 15, "GROUP_COMLINK_ACTIVE a payload") && ok;
    ok = expectEqualInt(activePacket.ItemIndex, 0x09, "GROUP_COMLINK_ACTIVE ItemIndex payload") && ok;
    ok = expectEqualInt(activePacket.Category, 0x01, "GROUP_COMLINK_ACTIVE Category payload") && ok;
    ok = expectEqualInt(activePacket.ActiveFlg, 1, "GROUP_COMLINK_ACTIVE ActiveFlg payload") && ok;
    ok = expectEqualInt(activePacket.LinkshellId, 1, "GROUP_COMLINK_ACTIVE LinkshellId payload") && ok;

    const auto myRoomPacket = makeMyRoomIsPacket();
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_MYROOM_IS, sizeof(GP_CLI_COMMAND_MYROOM_IS)>(myRoomPacket),
                     MyRoomIsBytes{ 0xCB, 0x04, 0xEF, 0xBE, 0x05, 0x22, 0x6A, 0x02 },
                     "MYROOM_IS encoded packet") &&
         ok;
    ok = expectEqualInt(myRoomPacket.Kind, 5, "MYROOM_IS Kind payload") && ok;
    ok = expectEqualInt(myRoomPacket.Param1, 0x22, "MYROOM_IS Param1 payload") && ok;
    ok = expectEqualInt(myRoomPacket.Param2, 618, "MYROOM_IS Param2 payload") && ok;
    return ok;
}

auto testComlinkMyRoomConstantsAndValidation() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell1), 1, "GROUP_COMLINK_MAKE Linkshell1") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell2), 2, "GROUP_COMLINK_MAKE Linkshell2") && ok;
    ok = expectValid(validateGroupComlinkMakePure(0, 1), "GROUP_COMLINK_MAKE linkshell1 validation") && ok;
    ok = expectValid(validateGroupComlinkMakePure(0, 2), "GROUP_COMLINK_MAKE linkshell2 validation") && ok;
    ok = expectInvalidError(validateGroupComlinkMakePure(1, 1), "State not 0", "GROUP_COMLINK_MAKE State validation") && ok;
    ok = expectInvalidError(validateGroupComlinkMakePure(0, 3), "LinkshellId value 3 is not allowed.", "GROUP_COMLINK_MAKE LinkshellId validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG::Unequip), 0, "GROUP_COMLINK_ACTIVE Unequip") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG::EquipOrCreate), 1, "GROUP_COMLINK_ACTIVE EquipOrCreate") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID::Linkshell1), 1, "GROUP_COMLINK_ACTIVE Linkshell1") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID::Linkshell2), 2, "GROUP_COMLINK_ACTIVE Linkshell2") && ok;
    ok = expectValid(validateGroupComlinkActivePure(0, 0, 0, 15, 1, 1), "GROUP_COMLINK_ACTIVE low color validation") && ok;
    ok = expectValid(validateGroupComlinkActivePure(15, 15, 15, 15, 0, 2), "GROUP_COMLINK_ACTIVE high color validation") && ok;
    ok = expectInvalidError(validateGroupComlinkActivePure(16, 0, 0, 15, 0, 1), "r out of range: 16 not in [0, 15]", "GROUP_COMLINK_ACTIVE r validation") && ok;
    ok = expectInvalidError(validateGroupComlinkActivePure(0, 16, 0, 15, 0, 1), "g out of range: 16 not in [0, 15]", "GROUP_COMLINK_ACTIVE g validation") && ok;
    ok = expectInvalidError(validateGroupComlinkActivePure(0, 0, 16, 15, 0, 1), "b out of range: 16 not in [0, 15]", "GROUP_COMLINK_ACTIVE b validation") && ok;
    ok = expectInvalidError(validateGroupComlinkActivePure(0, 0, 0, 14, 0, 1), "a not 15", "GROUP_COMLINK_ACTIVE a validation") && ok;
    ok = expectInvalidError(validateGroupComlinkActivePure(0, 0, 0, 15, 2, 1), "ActiveFlg value 2 is not allowed.", "GROUP_COMLINK_ACTIVE ActiveFlg validation") && ok;
    ok = expectInvalidError(validateGroupComlinkActivePure(0, 0, 0, 15, 0, 0), "LinkshellId value 0 is not allowed.", "GROUP_COMLINK_ACTIVE LinkshellId validation") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MYROOM_IS_KIND::Open), 1, "MYROOM_IS Kind::Open") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MYROOM_IS_KIND::Close), 2, "MYROOM_IS Kind::Close") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_MYROOM_IS_KIND::Remodel), 5, "MYROOM_IS Kind::Remodel") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::Unk1), 0, "MYROOM_IS Param2::Unk1") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::Unk2), 1, "MYROOM_IS Param2::Unk2") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::SandorianStyle), 615, "MYROOM_IS Param2::SandorianStyle") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::BastokanStyle), 616, "MYROOM_IS Param2::BastokanStyle") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::WindurstianStyle), 617, "MYROOM_IS Param2::WindurstianStyle") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_IS_PARAM2::MogPatio), 618, "MYROOM_IS Param2::MogPatio") && ok;
    ok = expectValid(validateMyRoomIsPure(1, 0), "MYROOM_IS open validation") && ok;
    ok = expectValid(validateMyRoomIsPure(5, 618), "MYROOM_IS remodel validation") && ok;
    ok = expectInvalidError(validateMyRoomIsPure(3, 0), "3 not a valid GP_CLI_COMMAND_MYROOM_IS_KIND value.", "MYROOM_IS invalid Kind validation") && ok;
    ok = expectInvalidError(validateMyRoomIsPure(5, 614), "614 not a valid GP_CLI_COMMAND_MYROOM_IS_PARAM2 value.", "MYROOM_IS invalid Param2 gap validation") && ok;
    ok = expectInvalidError(validateMyRoomIsPure(5, 619), "619 not a valid GP_CLI_COMMAND_MYROOM_IS_PARAM2 value.", "MYROOM_IS invalid Param2 high validation") && ok;
    return ok;
}

} // namespace

auto runC2SComlinkMyRoomPacketSelfTests() -> bool
{
    return testComlinkMyRoomLayoutMetadata() &&
           testComlinkMyRoomEncodedBytesAndPayloads() &&
           testComlinkMyRoomConstantsAndValidation();
}
