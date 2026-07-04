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

#include "test_c2s_extended_job_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/entities/battle_entity.h"
#include "map/packets/c2s/0x102_extended_job.h"

namespace
{

using ExtendedJobBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_EXTENDED_JOB)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s extended job packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s extended job packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s extended job packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s extended job packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s extended job packet self-test failed: " << label << " got";
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

auto validateMonstrosityPure(mon_flags0_t flags, std::uint16_t speciesIndex, const std::array<std::uint16_t, 12>& slots, std::uint8_t descriptor1Index, std::uint8_t descriptor2Index) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    if (flags.SpeciesFlag)
    {
        validator.range("SpeciesIndex", speciesIndex, 1, 511);
    }
    if (flags.InstinctFlag)
    {
        for (const auto slot : slots)
        {
            if (slot != 0xFFFF && slot != 0x0)
            {
                validator.range("Slots", slot, 3, 799);
            }
        }
    }
    if (flags.Descriptor1Flag)
    {
        validator.range("Descriptor1Index", descriptor1Index, 0, 248);
    }
    if (flags.Descriptor2Flag)
    {
        validator.range("Descriptor2Index", descriptor2Index, 0, 248);
    }
    return validator;
}

auto expectedExtendedJobBytes() -> ExtendedJobBytes
{
    auto expected = ExtendedJobBytes{};
    expected[0]   = 0x02;
    expected[1]   = 0x53;
    expected[2]   = 0xEF;
    expected[3]   = 0xBE;
    return expected;
}

auto makeBluPacket() -> GP_CLI_COMMAND_EXTENDED_JOB
{
    auto packet                       = GP_CLI_COMMAND_EXTENDED_JOB{};
    packet.header.id                 = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EXTENDED_JOB);
    packet.header.size               = sizeof(GP_CLI_COMMAND_EXTENDED_JOB) / 4U;
    packet.header.sync               = 0xBEEF;
    packet.Data.bluData.SpellId      = 0x44;
    packet.Data.bluData.unknown00    = 0x55;
    packet.Data.bluData.padding00[0] = 0x66;
    packet.Data.bluData.padding00[1] = 0x77;
    packet.Data.bluData.JobIndex     = JOB_BLU;
    packet.Data.bluData.SupportJobFlg = 1;
    packet.Data.bluData.padding01[0] = 0x88;
    packet.Data.bluData.padding01[1] = 0x99;
    packet.Data.bluData.Spells[0]    = 0x11;
    packet.Data.bluData.Spells[19]   = 0x22;
    packet.Data.bluData.unused00[0]  = 0x33;
    packet.Data.bluData.unused00[131] = 0x44;
    return packet;
}

auto makePupPacket() -> GP_CLI_COMMAND_EXTENDED_JOB
{
    auto packet                       = GP_CLI_COMMAND_EXTENDED_JOB{};
    packet.header.id                 = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EXTENDED_JOB);
    packet.header.size               = sizeof(GP_CLI_COMMAND_EXTENDED_JOB) / 4U;
    packet.header.sync               = 0xBEEF;
    packet.Data.pupData.ItemId       = 0x44;
    packet.Data.pupData.unknown00    = 0x55;
    packet.Data.pupData.padding00[0] = 0x66;
    packet.Data.pupData.padding00[1] = 0x77;
    packet.Data.pupData.JobIndex     = JOB_PUP;
    packet.Data.pupData.SupportJobFlg = 1;
    packet.Data.pupData.padding01[0] = 0x88;
    packet.Data.pupData.padding01[1] = 0x99;
    packet.Data.pupData.Slots[static_cast<std::uint8_t>(AutomatonSlot::Head)] = 0x20;
    packet.Data.pupData.Slots[static_cast<std::uint8_t>(AutomatonSlot::Frame)] = 0x21;
    packet.Data.pupData.Slots[static_cast<std::uint8_t>(AutomatonSlot::Attachment12)] = 0x2D;
    packet.Data.pupData.unused00[0]  = 0x33;
    packet.Data.pupData.unused00[137] = 0x44;
    return packet;
}

auto makeMonPacket() -> GP_CLI_COMMAND_EXTENDED_JOB
{
    auto packet                 = GP_CLI_COMMAND_EXTENDED_JOB{};
    packet.header.id           = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_EXTENDED_JOB);
    packet.header.size         = sizeof(GP_CLI_COMMAND_EXTENDED_JOB) / 4U;
    packet.header.sync         = 0xBEEF;
    packet.Data.monData.unknown00[0] = 1;
    packet.Data.monData.unknown00[5] = 6;
    packet.Data.monData.Flags0.SpeciesFlag = 1;
    packet.Data.monData.Flags0.InstinctFlag = 1;
    packet.Data.monData.Flags0.Descriptor1Flag = 1;
    packet.Data.monData.Flags0.Descriptor2Flag = 1;
    packet.Data.monData.unknown01 = 0x77;
    packet.Data.monData.SpeciesIndex = 511;
    packet.Data.monData.unknown02 = 0x8899;
    packet.Data.monData.Slots[0] = 0x0000;
    packet.Data.monData.Slots[1] = 0xFFFF;
    packet.Data.monData.Slots[2] = 3;
    packet.Data.monData.Slots[11] = 799;
    packet.Data.monData.Descriptor1Index = 248;
    packet.Data.monData.Descriptor2Index = 42;
    packet.Data.monData.unused00[0] = 0x33;
    packet.Data.monData.unused00[121] = 0x44;
    return packet;
}

auto testExtendedJobLayoutsAndMetadata() -> bool
{
    bool ok = true;
    ok      = expectEqualString(GP_CLI_COMMAND_EXTENDED_JOB::name, "GP_CLI_COMMAND_EXTENDED_JOB", "EXTENDED_JOB name") && ok;
    ok      = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_EXTENDED_JOB::packetId), 0x102, "EXTENDED_JOB packet id") && ok;
    ok      = expectEqualInt(sizeof(GP_CLI_COMMAND_EXTENDED_JOB), 164, "EXTENDED_JOB sizeof") && ok;
    ok      = expectEqualInt(offsetof(GP_CLI_COMMAND_EXTENDED_JOB, Data), 4, "EXTENDED_JOB Data offset") && ok;
    ok      = expectEqualInt(sizeof(blu_data_t), 160, "blu_data_t sizeof") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, SpellId), 0, "blu_data_t SpellId offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, unknown00), 1, "blu_data_t unknown00 offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, padding00), 2, "blu_data_t padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, JobIndex), 4, "blu_data_t JobIndex offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, SupportJobFlg), 5, "blu_data_t SupportJobFlg offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, padding01), 6, "blu_data_t padding01 offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, Spells), 8, "blu_data_t Spells offset") && ok;
    ok      = expectEqualInt(offsetof(blu_data_t, unused00), 28, "blu_data_t unused00 offset") && ok;
    ok      = expectEqualInt(sizeof(pup_data_t), 160, "pup_data_t sizeof") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, ItemId), 0, "pup_data_t ItemId offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, unknown00), 1, "pup_data_t unknown00 offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, padding00), 2, "pup_data_t padding00 offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, JobIndex), 4, "pup_data_t JobIndex offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, SupportJobFlg), 5, "pup_data_t SupportJobFlg offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, padding01), 6, "pup_data_t padding01 offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, Slots), 8, "pup_data_t Slots offset") && ok;
    ok      = expectEqualInt(offsetof(pup_data_t, unused00), 22, "pup_data_t unused00 offset") && ok;
    ok      = expectEqualInt(sizeof(mon_data_t), 160, "mon_data_t sizeof") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, unknown00), 0, "mon_data_t unknown00 offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, Flags0), 6, "mon_data_t Flags0 offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, unknown01), 7, "mon_data_t unknown01 offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, SpeciesIndex), 8, "mon_data_t SpeciesIndex offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, unknown02), 10, "mon_data_t unknown02 offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, Slots), 12, "mon_data_t Slots offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, Descriptor1Index), 36, "mon_data_t Descriptor1Index offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, Descriptor2Index), 37, "mon_data_t Descriptor2Index offset") && ok;
    ok      = expectEqualInt(offsetof(mon_data_t, unused00), 38, "mon_data_t unused00 offset") && ok;
    ok      = expectEqualInt(JOB_BLU, 16, "JOB_BLU") && ok;
    ok      = expectEqualInt(JOB_PUP, 18, "JOB_PUP") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(AutomatonSlot::Head), 0, "AutomatonSlot Head") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(AutomatonSlot::Frame), 1, "AutomatonSlot Frame") && ok;
    ok      = expectEqualInt(static_cast<std::uint8_t>(AutomatonSlot::Attachment12), 13, "AutomatonSlot Attachment12") && ok;
    return ok;
}

auto testExtendedJobEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    auto bluExpected = expectedExtendedJobBytes();
    bluExpected[4] = 0x44;
    bluExpected[5] = 0x55;
    bluExpected[6] = 0x66;
    bluExpected[7] = 0x77;
    bluExpected[8] = JOB_BLU;
    bluExpected[9] = 1;
    bluExpected[10] = 0x88;
    bluExpected[11] = 0x99;
    bluExpected[12] = 0x11;
    bluExpected[31] = 0x22;
    bluExpected[32] = 0x33;
    bluExpected[163] = 0x44;
    ok = expectBytes(encodedPacketBytes(makeBluPacket()), bluExpected, "EXTENDED_JOB BLU encoded packet") && ok;

    auto pupExpected = expectedExtendedJobBytes();
    pupExpected[4] = 0x44;
    pupExpected[5] = 0x55;
    pupExpected[6] = 0x66;
    pupExpected[7] = 0x77;
    pupExpected[8] = JOB_PUP;
    pupExpected[9] = 1;
    pupExpected[10] = 0x88;
    pupExpected[11] = 0x99;
    pupExpected[12] = 0x20;
    pupExpected[13] = 0x21;
    pupExpected[25] = 0x2D;
    pupExpected[26] = 0x33;
    pupExpected[163] = 0x44;
    ok = expectBytes(encodedPacketBytes(makePupPacket()), pupExpected, "EXTENDED_JOB PUP encoded packet") && ok;

    auto monExpected = expectedExtendedJobBytes();
    monExpected[4] = 1;
    monExpected[9] = 6;
    monExpected[10] = 0x1D;
    monExpected[11] = 0x77;
    monExpected[12] = 0xFF;
    monExpected[13] = 0x01;
    monExpected[14] = 0x99;
    monExpected[15] = 0x88;
    monExpected[16] = 0x00;
    monExpected[17] = 0x00;
    monExpected[18] = 0xFF;
    monExpected[19] = 0xFF;
    monExpected[20] = 0x03;
    monExpected[21] = 0x00;
    monExpected[38] = 0x1F;
    monExpected[39] = 0x03;
    monExpected[40] = 248;
    monExpected[41] = 42;
    monExpected[42] = 0x33;
    monExpected[163] = 0x44;
    ok = expectBytes(encodedPacketBytes(makeMonPacket()), monExpected, "EXTENDED_JOB MON encoded packet") && ok;
    return ok;
}

auto testExtendedJobMonstrosityValidation() -> bool
{
    bool ok = true;
    auto none = mon_flags0_t{};
    auto species = mon_flags0_t{};
    species.SpeciesFlag = 1;
    auto instinct = mon_flags0_t{};
    instinct.InstinctFlag = 1;
    auto desc1 = mon_flags0_t{};
    desc1.Descriptor1Flag = 1;
    auto desc2 = mon_flags0_t{};
    desc2.Descriptor2Flag = 1;
    auto speciesAndInstinct = mon_flags0_t{};
    speciesAndInstinct.SpeciesFlag = 1;
    speciesAndInstinct.InstinctFlag = 1;

    ok = expectValid(validateMonstrosityPure(none, 0, { 2 }, 249, 249), "Monstrosity no flags validation") && ok;
    ok = expectValid(validateMonstrosityPure(species, 1, {}, 0, 0), "Monstrosity species min validation") && ok;
    ok = expectValid(validateMonstrosityPure(species, 511, {}, 0, 0), "Monstrosity species max validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(species, 0, {}, 0, 0), "SpeciesIndex out of range: 0 not in [1, 511]", "Monstrosity species below validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(species, 512, {}, 0, 0), "SpeciesIndex out of range: 512 not in [1, 511]", "Monstrosity species above validation") && ok;
    ok = expectValid(validateMonstrosityPure(instinct, 0, { 0, 0xFFFF, 3, 799 }, 0, 0), "Monstrosity instinct sentinels validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(instinct, 0, { 2 }, 0, 0), "Slots out of range: 2 not in [3, 799]", "Monstrosity slot below validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(instinct, 0, { 800 }, 0, 0), "Slots out of range: 800 not in [3, 799]", "Monstrosity slot above validation") && ok;
    ok = expectValid(validateMonstrosityPure(desc1, 0, {}, 248, 0), "Monstrosity descriptor1 max validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(desc1, 0, {}, 249, 0), "Descriptor1Index out of range: 249 not in [0, 248]", "Monstrosity descriptor1 above validation") && ok;
    ok = expectValid(validateMonstrosityPure(desc2, 0, {}, 0, 248), "Monstrosity descriptor2 max validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(desc2, 0, {}, 0, 249), "Descriptor2Index out of range: 249 not in [0, 248]", "Monstrosity descriptor2 above validation") && ok;
    ok = expectInvalidError(validateMonstrosityPure(speciesAndInstinct, 0, { 2 }, 0, 0), "SpeciesIndex out of range: 0 not in [1, 511]", "Monstrosity validation order") && ok;
    return ok;
}

} // namespace

auto runC2SExtendedJobPacketSelfTests() -> bool
{
    return testExtendedJobLayoutsAndMetadata() &&
           testExtendedJobEncodedBytesAndPayloads() &&
           testExtendedJobMonstrosityValidation();
}
