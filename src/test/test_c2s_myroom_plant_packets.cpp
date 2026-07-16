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

#include "test_c2s_myroom_plant_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include "map/item_container.h"
#include "map/items/item_flowerpot.h"
#include "map/packets/c2s/0x0fc_myroom_plant_add.h"
#include "map/packets/c2s/0x0fd_myroom_plant_check.h"
#include "map/packets/c2s/0x0fe_myroom_plant_crop.h"
#include "map/packets/c2s/0x0ff_myroom_plant_stop.h"

namespace
{

using MyRoomPlantAddBytes   = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_PLANT_ADD)>;
using MyRoomPlantCheckBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CHECK)>;
using MyRoomPlantCropBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CROP)>;
using MyRoomPlantStopBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_MYROOM_PLANT_STOP)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom plant packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s myroom plant packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s myroom plant packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom plant packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s myroom plant packet self-test failed: " << label << " got";
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

auto myroomPlantValidCategories() -> std::set<std::uint8_t>
{
    return { LOC_MOGSAFE, LOC_MOGSAFE2 };
}

auto validatePlantAddPure(std::uint16_t plantItemNo, std::uint16_t addItemNo, std::uint8_t plantCategory, std::uint8_t addCategory) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustNotEqual(plantItemNo, 0, "MyroomPlantItemNo must not be 0")
        .mustNotEqual(addItemNo, 0, "MyroomAddItemNo must not be 0")
        .oneOf("MyroomPlantCategory", plantCategory, myroomPlantValidCategories())
        .oneOf("MyroomAddCategory", addCategory, myroomPlantValidCategories());
    return validator;
}

auto validatePlantPure(std::uint16_t plantItemNo, std::uint8_t plantCategory) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustNotEqual(plantItemNo, 0, "MyroomPlantItemNo must not be 0")
        .oneOf("MyroomPlantCategory", plantCategory, myroomPlantValidCategories());
    return validator;
}

auto makeMyRoomPlantAddPacket() -> GP_CLI_COMMAND_MYROOM_PLANT_ADD
{
    auto packet                    = GP_CLI_COMMAND_MYROOM_PLANT_ADD{};
    packet.header.id              = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_PLANT_ADD);
    packet.header.size            = sizeof(GP_CLI_COMMAND_MYROOM_PLANT_ADD) / 4U;
    packet.header.sync            = 0xBEEF;
    packet.MyroomPlantItemNo      = 0x1234;
    packet.MyroomAddItemNo        = 0x5678;
    packet.MyroomPlantItemIndex   = 0x9A;
    packet.MyroomAddItemIndex     = 0xBC;
    packet.MyroomPlantCategory    = LOC_MOGSAFE;
    packet.MyroomAddCategory      = LOC_MOGSAFE2;
    return packet;
}

auto makeMyRoomPlantCheckPacket() -> GP_CLI_COMMAND_MYROOM_PLANT_CHECK
{
    auto packet                  = GP_CLI_COMMAND_MYROOM_PLANT_CHECK{};
    packet.header.id            = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_PLANT_CHECK);
    packet.header.size          = sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CHECK) / 4U;
    packet.header.sync          = 0xBEEF;
    packet.MyroomPlantItemNo    = 0x1234;
    packet.MyroomPlantItemIndex = 0x56;
    packet.MyroomPlantCategory  = LOC_MOGSAFE2;
    return packet;
}

auto makeMyRoomPlantCropPacket() -> GP_CLI_COMMAND_MYROOM_PLANT_CROP
{
    auto packet                  = GP_CLI_COMMAND_MYROOM_PLANT_CROP{};
    packet.header.id            = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_PLANT_CROP);
    packet.header.size          = sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CROP) / 4U;
    packet.header.sync          = 0xBEEF;
    packet.MyroomPlantItemNo    = 0x1234;
    packet.MyroomPlantItemIndex = 0x56;
    packet.MyroomPlantCategory  = LOC_MOGSAFE;
    packet.CancellFlg           = 0x02;
    packet.padding00[0]         = 0xAA;
    packet.padding00[1]         = 0xBB;
    packet.padding00[2]         = 0xCC;
    return packet;
}

auto makeMyRoomPlantStopPacket() -> GP_CLI_COMMAND_MYROOM_PLANT_STOP
{
    auto packet                  = GP_CLI_COMMAND_MYROOM_PLANT_STOP{};
    packet.header.id            = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_MYROOM_PLANT_STOP);
    packet.header.size          = sizeof(GP_CLI_COMMAND_MYROOM_PLANT_STOP) / 4U;
    packet.header.sync          = 0xBEEF;
    packet.MyroomPlantItemNo    = 0x1234;
    packet.MyroomPlantItemIndex = 0x56;
    packet.MyroomPlantCategory  = LOC_MOGSAFE2;
    return packet;
}

auto testMyRoomPlantLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_PLANT_ADD::name, "GP_CLI_COMMAND_MYROOM_PLANT_ADD", "MYROOM_PLANT_ADD name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_PLANT_ADD::packetId), 0x0FC, "MYROOM_PLANT_ADD packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_PLANT_ADD), 12, "MYROOM_PLANT_ADD sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_ADD, MyroomPlantItemNo), 4, "MYROOM_PLANT_ADD MyroomPlantItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_ADD, MyroomAddItemNo), 6, "MYROOM_PLANT_ADD MyroomAddItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_ADD, MyroomPlantItemIndex), 8, "MYROOM_PLANT_ADD MyroomPlantItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_ADD, MyroomAddItemIndex), 9, "MYROOM_PLANT_ADD MyroomAddItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_ADD, MyroomPlantCategory), 10, "MYROOM_PLANT_ADD MyroomPlantCategory offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_ADD, MyroomAddCategory), 11, "MYROOM_PLANT_ADD MyroomAddCategory offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_PLANT_CHECK::name, "GP_CLI_COMMAND_MYROOM_PLANT_CHECK", "MYROOM_PLANT_CHECK name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_PLANT_CHECK::packetId), 0x0FD, "MYROOM_PLANT_CHECK packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CHECK), 8, "MYROOM_PLANT_CHECK sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CHECK, MyroomPlantItemNo), 4, "MYROOM_PLANT_CHECK MyroomPlantItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CHECK, MyroomPlantItemIndex), 6, "MYROOM_PLANT_CHECK MyroomPlantItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CHECK, MyroomPlantCategory), 7, "MYROOM_PLANT_CHECK MyroomPlantCategory offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_PLANT_CROP::name, "GP_CLI_COMMAND_MYROOM_PLANT_CROP", "MYROOM_PLANT_CROP name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_PLANT_CROP::packetId), 0x0FE, "MYROOM_PLANT_CROP packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CROP), 12, "MYROOM_PLANT_CROP sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CROP, MyroomPlantItemNo), 4, "MYROOM_PLANT_CROP MyroomPlantItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CROP, MyroomPlantItemIndex), 6, "MYROOM_PLANT_CROP MyroomPlantItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CROP, MyroomPlantCategory), 7, "MYROOM_PLANT_CROP MyroomPlantCategory offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CROP, CancellFlg), 8, "MYROOM_PLANT_CROP CancellFlg offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_CROP, padding00), 9, "MYROOM_PLANT_CROP padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_PLANT_CROP::padding00), 3, "MYROOM_PLANT_CROP padding00 sizeof") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_MYROOM_PLANT_STOP::name, "GP_CLI_COMMAND_MYROOM_PLANT_STOP", "MYROOM_PLANT_STOP name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_MYROOM_PLANT_STOP::packetId), 0x0FF, "MYROOM_PLANT_STOP packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_MYROOM_PLANT_STOP), 8, "MYROOM_PLANT_STOP sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_STOP, MyroomPlantItemNo), 4, "MYROOM_PLANT_STOP MyroomPlantItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_STOP, MyroomPlantItemIndex), 6, "MYROOM_PLANT_STOP MyroomPlantItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_MYROOM_PLANT_STOP, MyroomPlantCategory), 7, "MYROOM_PLANT_STOP MyroomPlantCategory offset") && ok;
    ok = expectEqualInt(LOC_MOGSAFE, 1, "LOC_MOGSAFE") && ok;
    ok = expectEqualInt(LOC_MOGSAFE2, 9, "LOC_MOGSAFE2") && ok;
    return ok;
}

auto testMyRoomPlantEncodedBytesAndPayloads() -> bool
{
    bool ok = true;
    ok      = expectBytes(encodedPacketBytes(makeMyRoomPlantAddPacket()), MyRoomPlantAddBytes{ 0xFC, 0x06, 0xEF, 0xBE, 0x34, 0x12, 0x78, 0x56, 0x9A, 0xBC, 0x01, 0x09 }, "MYROOM_PLANT_ADD encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeMyRoomPlantCheckPacket()), MyRoomPlantCheckBytes{ 0xFD, 0x04, 0xEF, 0xBE, 0x34, 0x12, 0x56, 0x09 }, "MYROOM_PLANT_CHECK encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeMyRoomPlantCropPacket()), MyRoomPlantCropBytes{ 0xFE, 0x06, 0xEF, 0xBE, 0x34, 0x12, 0x56, 0x01, 0x02, 0xAA, 0xBB, 0xCC }, "MYROOM_PLANT_CROP encoded packet") && ok;
    ok      = expectBytes(encodedPacketBytes(makeMyRoomPlantStopPacket()), MyRoomPlantStopBytes{ 0xFF, 0x04, 0xEF, 0xBE, 0x34, 0x12, 0x56, 0x09 }, "MYROOM_PLANT_STOP encoded packet") && ok;
    return ok;
}

auto testMyRoomPlantValidation() -> bool
{
    bool ok = true;
    ok      = expectValid(validatePlantAddPure(1, 2, LOC_MOGSAFE, LOC_MOGSAFE2), "MYROOM_PLANT_ADD valid categories validation") && ok;
    ok      = expectInvalidError(validatePlantAddPure(0, 2, LOC_MOGSAFE, LOC_MOGSAFE2), "MyroomPlantItemNo must not be 0", "MYROOM_PLANT_ADD plant item zero validation") && ok;
    ok      = expectInvalidError(validatePlantAddPure(1, 0, LOC_MOGSAFE, LOC_MOGSAFE2), "MyroomAddItemNo must not be 0", "MYROOM_PLANT_ADD add item zero validation") && ok;
    ok      = expectInvalidError(validatePlantAddPure(1, 2, LOC_INVENTORY, LOC_MOGSAFE2), "MyroomPlantCategory value 0 is not allowed.", "MYROOM_PLANT_ADD invalid plant category validation") && ok;
    ok      = expectInvalidError(validatePlantAddPure(1, 2, LOC_MOGSAFE, LOC_INVENTORY), "MyroomAddCategory value 0 is not allowed.", "MYROOM_PLANT_ADD invalid add category validation") && ok;
    ok      = expectInvalidError(validatePlantAddPure(0, 0, LOC_INVENTORY, LOC_INVENTORY), "MyroomPlantItemNo must not be 0", "MYROOM_PLANT_ADD validation order") && ok;

    ok = expectValid(validatePlantPure(1, LOC_MOGSAFE), "MYROOM_PLANT_CHECK mogsafe validation") && ok;
    ok = expectValid(validatePlantPure(1, LOC_MOGSAFE2), "MYROOM_PLANT_CHECK mogsafe2 validation") && ok;
    ok = expectInvalidError(validatePlantPure(0, LOC_MOGSAFE), "MyroomPlantItemNo must not be 0", "MYROOM_PLANT_CHECK plant item zero validation") && ok;
    ok = expectInvalidError(validatePlantPure(1, LOC_INVENTORY), "MyroomPlantCategory value 0 is not allowed.", "MYROOM_PLANT_CHECK invalid category validation") && ok;

    ok = expectValid(validatePlantPure(1, LOC_MOGSAFE), "MYROOM_PLANT_CROP mogsafe validation") && ok;
    ok = expectInvalidError(validatePlantPure(0, LOC_MOGSAFE), "MyroomPlantItemNo must not be 0", "MYROOM_PLANT_CROP plant item zero validation") && ok;
    ok = expectInvalidError(validatePlantPure(1, LOC_INVENTORY), "MyroomPlantCategory value 0 is not allowed.", "MYROOM_PLANT_CROP invalid category validation") && ok;

    ok = expectValid(validatePlantPure(1, LOC_MOGSAFE2), "MYROOM_PLANT_STOP mogsafe2 validation") && ok;
    ok = expectInvalidError(validatePlantPure(0, LOC_MOGSAFE2), "MyroomPlantItemNo must not be 0", "MYROOM_PLANT_STOP plant item zero validation") && ok;
    ok = expectInvalidError(validatePlantPure(1, LOC_INVENTORY), "MyroomPlantCategory value 0 is not allowed.", "MYROOM_PLANT_STOP invalid category validation") && ok;
    return ok;
}

auto testMyRoomPlantStopRuntimePlan() -> bool
{
    const auto isNoop = [](const myroomplantstophelpers::RuntimePlan& plan) {
        return !plan.sendMoogleDriesPlant && !plan.sendMyRoomOperation && !plan.setDried && !plan.persistExtra && !plan.sendItemAttr && !plan.sendItemSame;
    };
    const auto eligible = myroomplantstophelpers::MakeRuntimePlan({
        .hasFlowerpot = true,
        .planted       = true,
        .stage         = FLOWERPOT_STAGE_FIRST_SPROUTS,
        .dried         = false,
    });
    const auto noFlowerpot = myroomplantstophelpers::MakeRuntimePlan({
        .hasFlowerpot = false,
        .planted       = true,
        .stage         = FLOWERPOT_STAGE_FIRST_SPROUTS,
        .dried         = false,
    });
    const auto unplanted = myroomplantstophelpers::MakeRuntimePlan({
        .hasFlowerpot = true,
        .planted       = false,
        .stage         = FLOWERPOT_STAGE_FIRST_SPROUTS,
        .dried         = false,
    });
    const auto initial = myroomplantstophelpers::MakeRuntimePlan({
        .hasFlowerpot = true,
        .planted       = true,
        .stage         = FLOWERPOT_STAGE_INITIAL,
        .dried         = false,
    });
    const auto wilted = myroomplantstophelpers::MakeRuntimePlan({
        .hasFlowerpot = true,
        .planted       = true,
        .stage         = FLOWERPOT_STAGE_WILTED,
        .dried         = false,
    });
    const auto dried = myroomplantstophelpers::MakeRuntimePlan({
        .hasFlowerpot = true,
        .planted       = true,
        .stage         = FLOWERPOT_STAGE_MATURE_PLANT,
        .dried         = true,
    });

    return expectTrue(eligible.sendMoogleDriesPlant && eligible.sendMyRoomOperation && eligible.setDried && eligible.persistExtra && eligible.sendItemAttr && eligible.sendItemSame, "MYROOM_PLANT_STOP eligible plan") &&
           expectTrue(isNoop(noFlowerpot) && isNoop(unplanted) && isNoop(initial) && isNoop(wilted) && isNoop(dried), "MYROOM_PLANT_STOP ineligible plans");
}

} // namespace

auto runC2SMyRoomPlantPacketSelfTests() -> bool
{
    return testMyRoomPlantLayoutsAndMetadata() &&
           testMyRoomPlantEncodedBytesAndPayloads() &&
           testMyRoomPlantValidation() &&
           testMyRoomPlantStopRuntimePlan();
}
