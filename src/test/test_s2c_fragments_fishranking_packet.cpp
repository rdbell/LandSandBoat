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

#include "test_s2c_fragments_fishranking_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "map/packets/s2c/0x04d_fragments_fishranking.h"

namespace
{

constexpr auto fragmentsFishRankingCommandOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, Command);
constexpr auto fragmentsFishRankingResultOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, Result);
constexpr auto fragmentsFishRankingValue1Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, value1);
constexpr auto fragmentsFishRankingValue2Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, value2);
constexpr auto fragmentsFishRankingTimestampOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, timestamp);
constexpr auto fragmentsFishRankingSizeTotalOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, size_total);
constexpr auto fragmentsFishRankingOffsetOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, offset);
constexpr auto fragmentsFishRankingDataSizeOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, data_size);
constexpr auto fragmentsFishRankingTotalEntriesOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, totalEntries);
constexpr auto fragmentsFishRankingPaddingOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, padding);
constexpr auto fragmentsFishRankingDataOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData, data);
constexpr auto fragmentsFishRankingPacketDataSize     = sizeof(GP_SERV_COMMAND_FRAGMENTS::FISHRANKING::PacketData);
constexpr auto fragmentsFishRankingFullPacketSize     = sizeof(GP_SERV_HEADER) + fragmentsFishRankingPacketDataSize;
constexpr auto fragmentsFishRankingEntrySize          = sizeof(FishingContestEntry);
constexpr auto fragmentsFishRankingEntryCount         = 6U;
constexpr auto fragmentsFishRankingDataBytes          = fragmentsFishRankingEntrySize * fragmentsFishRankingEntryCount;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c FRAGMENTS FISHRANKING packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c FRAGMENTS FISHRANKING packet self-test failed: " << label << " got";
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

auto expectRepeatedByte(CBasicPacket& packet, std::size_t offset, std::size_t count, uint8 expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = 0; i < count; ++i)
    {
        if (data[offset + i] != expected)
        {
            std::cerr << "s2c FRAGMENTS FISHRANKING packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto putUint32(std::array<uint8, 36>& data, std::size_t offset, std::uint32_t value) -> void
{
    data[offset]     = static_cast<uint8>(value & 0xFF);
    data[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    data[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    data[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

auto makeEntry(const std::string& name, uint8 mjob, uint8 sjob, uint8 mlvl, uint8 slvl, uint8 race, uint8 allegiance, uint8 fishRank, std::uint32_t score, std::uint32_t submitTime, uint8 contestRank, uint8 resultCount, uint8 share, uint8 datasetB) -> FishingContestEntry
{
    auto entry = FishingContestEntry{};
    std::memcpy(entry.name, name.data(), name.size() > sizeof(entry.name) ? sizeof(entry.name) : name.size());
    entry.mjob        = mjob;
    entry.sjob        = sjob;
    entry.mlvl        = mlvl;
    entry.slvl        = slvl;
    entry.race        = race;
    entry.allegiance  = allegiance;
    entry.fishRank    = fishRank;
    entry.score       = score;
    entry.submitTime  = submitTime;
    entry.contestRank = contestRank;
    entry.resultCount = resultCount;
    entry.share       = share;
    entry.dataset_b   = datasetB;
    return entry;
}

auto expectedEntryBytes(const std::string& name, uint8 mjob, uint8 sjob, uint8 mlvl, uint8 slvl, uint8 race, uint8 allegiance, uint8 fishRank, std::uint32_t score, std::uint32_t submitTime, uint8 contestRank, uint8 resultCount, uint8 share, uint8 datasetB) -> std::array<uint8, 36>
{
    auto data = std::array<uint8, 36>{};
    for (std::size_t i = 0; i < name.size() && i < 16; ++i)
    {
        data[i] = static_cast<uint8>(name[i]);
    }
    data[16] = mjob;
    data[17] = sjob;
    data[18] = mlvl;
    data[19] = slvl;
    data[20] = race;
    data[21] = 0;
    data[22] = allegiance;
    data[23] = fishRank;
    putUint32(data, 24, score);
    putUint32(data, 28, submitTime);
    data[32] = contestRank;
    data[33] = resultCount;
    data[34] = share;
    data[35] = datasetB;
    return data;
}

auto selfEntry() -> FishingContestEntry
{
    return makeEntry("SelfFish", 1, 2, 99, 49, 5, 2, 10, 0x01020304, 0x11223344, 3, 7, 1, 9);
}

auto rankOneEntry() -> FishingContestEntry
{
    return makeEntry("RankOne", 6, 7, 75, 37, 8, 1, 9, 0x0A0B0C0D, 0x55667788, 1, 7, 2, 2);
}

auto rankTwoEntry() -> FishingContestEntry
{
    return makeEntry("RankTwo", 11, 12, 60, 30, 4, 0, 8, 0x10203040, 0x99AABBCC, 2, 7, 1, 1);
}

auto selfEntryBytes() -> std::array<uint8, 36>
{
    return expectedEntryBytes("SelfFish", 1, 2, 99, 49, 5, 2, 10, 0x01020304, 0x11223344, 3, 7, 1, 9);
}

auto rankOneEntryBytes() -> std::array<uint8, 36>
{
    return expectedEntryBytes("RankOne", 6, 7, 75, 37, 8, 1, 9, 0x0A0B0C0D, 0x55667788, 1, 7, 2, 2);
}

auto rankTwoEntryBytes() -> std::array<uint8, 36>
{
    return expectedEntryBytes("RankTwo", 11, 12, 60, 30, 4, 0, 8, 0x10203040, 0x99AABBCC, 2, 7, 1, 1);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_FRAGMENTS), 0x04D, "FRAGMENTS packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingEntrySize, 36, "sizeof(FishingContestEntry)") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingDataBytes, 216, "sizeof(data)") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingPacketDataSize, 244, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingFullPacketSize, 248, "full packet size") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingCommandOffset, 4, "Command offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingResultOffset, 5, "Result offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingValue1Offset, 6, "value1 offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingValue2Offset, 7, "value2 offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingTimestampOffset, 8, "timestamp offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingSizeTotalOffset, 12, "size_total offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingOffsetOffset, 16, "offset offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingDataSizeOffset, 20, "data_size offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingTotalEntriesOffset, 24, "totalEntries offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingPaddingOffset, 28, "padding offset") && ok;
    ok      = expectEqualUInt(fragmentsFishRankingDataOffset, 32, "data offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, name), 0, "entry name offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, mjob), 16, "entry mjob offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, sjob), 17, "entry sjob offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, mlvl), 18, "entry mlvl offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, slvl), 19, "entry slvl offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, race), 20, "entry race offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, padding), 21, "entry padding offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, allegiance), 22, "entry allegiance offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, fishRank), 23, "entry fishRank offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, score), 24, "entry score offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, submitTime), 28, "entry submitTime offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, contestRank), 32, "entry contestRank offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, resultCount), 33, "entry resultCount offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, share), 34, "entry share offset") && ok;
    ok      = expectEqualUInt(offsetof(FishingContestEntry, dataset_b), 35, "entry dataset_b offset") && ok;
    return ok;
}

auto testRepresentativeConstructor() -> bool
{
    auto entries = std::vector<FishingContestEntry>{ selfEntry(), rankOneEntry(), rankTwoEntry() };
    auto packet  = GP_SERV_COMMAND_FRAGMENTS::FISHRANKING(entries, 2, 0x11223344, 36, 7, 1);
    packet.setSequence(0xCAFE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x04D, "representative type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 104, "representative size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x4D, 0x34, 0xFE, 0xCA }, "representative header") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingCommandOffset, std::array<uint8, 4>{ 0x01, 0x01, 0x02, 0x02 }, "representative command fields") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingTimestampOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "representative timestamp") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingSizeTotalOffset, std::array<uint8, 4>{ 0xFC, 0x00, 0x00, 0x00 }, "representative size_total") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingOffsetOffset, std::array<uint8, 4>{ 0x24, 0x00, 0x00, 0x00 }, "representative offset") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingDataSizeOffset, std::array<uint8, 4>{ 0x48, 0x00, 0x00, 0x00 }, "representative data_size") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingTotalEntriesOffset, std::array<uint8, 4>{ 0x07, 0x00, 0x00, 0x00 }, "representative totalEntries") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingPaddingOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "representative padding") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingDataOffset, selfEntryBytes(), "representative self entry") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingDataOffset + fragmentsFishRankingEntrySize, rankOneEntryBytes(), "representative first ranking entry") && ok;
    ok      = expectBytes(packet, fragmentsFishRankingDataOffset + (2 * fragmentsFishRankingEntrySize), rankTwoEntryBytes(), "representative copied trailing entry") && ok;
    ok      = expectRepeatedByte(packet, fragmentsFishRankingDataOffset + (3 * fragmentsFishRankingEntrySize), PACKET_SIZE - fragmentsFishRankingDataOffset - (3 * fragmentsFishRankingEntrySize), 0, "representative tail") && ok;
    return ok;
}

auto testBoundaryEntryCounts() -> bool
{
    auto oneEntryPacket = GP_SERV_COMMAND_FRAGMENTS::FISHRANKING(std::vector<FishingContestEntry>{ selfEntry() }, 3, 0x01020304, 0, 1, 13);
    oneEntryPacket.setSequence(0xBEEF);

    auto maxEntries = std::vector<FishingContestEntry>{ selfEntry(), rankOneEntry(), rankTwoEntry(), selfEntry(), rankOneEntry(), rankTwoEntry() };
    auto maxPacket  = GP_SERV_COMMAND_FRAGMENTS::FISHRANKING(maxEntries, 1, 0x55667788, 72, 12, 2);
    maxPacket.setSequence(0x1234);

    bool ok = true;
    ok      = expectEqualUInt(oneEntryPacket.getSize(), 32, "one-entry size") && ok;
    ok      = expectBytes(oneEntryPacket, 0, std::array<uint8, 4>{ 0x4D, 0x10, 0xEF, 0xBE }, "one-entry header") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingCommandOffset, std::array<uint8, 4>{ 0x0D, 0x01, 0x02, 0x03 }, "one-entry command fields") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingTimestampOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "one-entry timestamp") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingSizeTotalOffset, std::array<uint8, 4>{ 0x24, 0x00, 0x00, 0x00 }, "one-entry size_total") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingOffsetOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "one-entry offset") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingDataSizeOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "one-entry data_size") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingTotalEntriesOffset, std::array<uint8, 4>{ 0x01, 0x00, 0x00, 0x00 }, "one-entry totalEntries") && ok;
    ok      = expectBytes(oneEntryPacket, fragmentsFishRankingDataOffset, selfEntryBytes(), "one-entry copied entry") && ok;
    ok      = expectRepeatedByte(oneEntryPacket, fragmentsFishRankingDataOffset + fragmentsFishRankingEntrySize, PACKET_SIZE - fragmentsFishRankingDataOffset - fragmentsFishRankingEntrySize, 0, "one-entry tail") && ok;

    ok = expectEqualUInt(maxPacket.getSize(), 212, "six-entry size") && ok;
    ok = expectBytes(maxPacket, 0, std::array<uint8, 4>{ 0x4D, 0x6A, 0x34, 0x12 }, "six-entry header") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingCommandOffset, std::array<uint8, 4>{ 0x02, 0x01, 0x02, 0x01 }, "six-entry command fields") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingTimestampOffset, std::array<uint8, 4>{ 0x88, 0x77, 0x66, 0x55 }, "six-entry timestamp") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingSizeTotalOffset, std::array<uint8, 4>{ 0xB0, 0x01, 0x00, 0x00 }, "six-entry size_total") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingOffsetOffset, std::array<uint8, 4>{ 0x48, 0x00, 0x00, 0x00 }, "six-entry offset") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingDataSizeOffset, std::array<uint8, 4>{ 0xB4, 0x00, 0x00, 0x00 }, "six-entry data_size") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingTotalEntriesOffset, std::array<uint8, 4>{ 0x0C, 0x00, 0x00, 0x00 }, "six-entry totalEntries") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingDataOffset, selfEntryBytes(), "six-entry first entry") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingDataOffset + (4 * fragmentsFishRankingEntrySize), rankOneEntryBytes(), "six-entry fifth entry") && ok;
    ok = expectBytes(maxPacket, fragmentsFishRankingDataOffset + (5 * fragmentsFishRankingEntrySize), rankTwoEntryBytes(), "six-entry copied trailing entry") && ok;
    ok = expectRepeatedByte(maxPacket, fragmentsFishRankingDataOffset + (6 * fragmentsFishRankingEntrySize), PACKET_SIZE - fragmentsFishRankingDataOffset - (6 * fragmentsFishRankingEntrySize), 0, "six-entry tail") && ok;
    return ok;
}

auto testEmptyOrOversizedEntriesKeepDefaultSize() -> bool
{
    auto emptyPacket = GP_SERV_COMMAND_FRAGMENTS::FISHRANKING({}, 4, 0x01020304, 72, 5, 12);
    auto entries     = std::vector<FishingContestEntry>{ selfEntry(), rankOneEntry(), rankTwoEntry(), selfEntry(), rankOneEntry(), rankTwoEntry(), selfEntry() };
    auto largePacket = GP_SERV_COMMAND_FRAGMENTS::FISHRANKING(entries, 1, 0x55667788, 36, 9, 2);

    bool ok = true;
    ok      = expectEqualUInt(emptyPacket.getSize(), fragmentsFishRankingFullPacketSize, "empty entries size") && ok;
    ok      = expectEqualUInt(largePacket.getSize(), fragmentsFishRankingFullPacketSize, "oversized entries size") && ok;
    ok      = expectBytes(emptyPacket, fragmentsFishRankingCommandOffset, std::array<uint8, 4>{ 0x0C, 0x01, 0x02, 0x04 }, "empty command fields") && ok;
    ok      = expectBytes(emptyPacket, fragmentsFishRankingTimestampOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "empty timestamp") && ok;
    ok      = expectBytes(emptyPacket, fragmentsFishRankingTotalEntriesOffset, std::array<uint8, 4>{ 0x05, 0x00, 0x00, 0x00 }, "empty totalEntries") && ok;
    ok      = expectRepeatedByte(emptyPacket, fragmentsFishRankingSizeTotalOffset, fragmentsFishRankingTotalEntriesOffset - fragmentsFishRankingSizeTotalOffset, 0, "empty size/offset fields") && ok;
    ok      = expectRepeatedByte(emptyPacket, fragmentsFishRankingPaddingOffset, PACKET_SIZE - fragmentsFishRankingPaddingOffset, 0, "empty padding and data") && ok;
    ok      = expectBytes(largePacket, fragmentsFishRankingCommandOffset, std::array<uint8, 4>{ 0x02, 0x01, 0x02, 0x01 }, "oversized command fields") && ok;
    ok      = expectBytes(largePacket, fragmentsFishRankingTimestampOffset, std::array<uint8, 4>{ 0x88, 0x77, 0x66, 0x55 }, "oversized timestamp") && ok;
    ok      = expectBytes(largePacket, fragmentsFishRankingTotalEntriesOffset, std::array<uint8, 4>{ 0x09, 0x00, 0x00, 0x00 }, "oversized totalEntries") && ok;
    ok      = expectRepeatedByte(largePacket, fragmentsFishRankingSizeTotalOffset, fragmentsFishRankingTotalEntriesOffset - fragmentsFishRankingSizeTotalOffset, 0, "oversized size/offset fields") && ok;
    ok      = expectRepeatedByte(largePacket, fragmentsFishRankingPaddingOffset, PACKET_SIZE - fragmentsFishRankingPaddingOffset, 0, "oversized padding and data") && ok;
    return ok;
}

} // namespace

auto runS2CFragmentsFishRankingPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testRepresentativeConstructor() && ok;
    ok      = testBoundaryEntryCounts() && ok;
    ok      = testEmptyOrOversizedEntriesKeepDefaultSize() && ok;
    return ok;
}
