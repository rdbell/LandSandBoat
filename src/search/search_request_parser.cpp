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

#include "common/logging.h"
#include "common/types/maybe.h"
#include "common/utils.h"
#include "data_loader.h"
#include "packets/search_list.h"
#include "search.h"

search_req ParseSearchRequestPayload(uint8* payload, uint8 size)
{
    // This function constructs a `search_req` based on which query should be sent to the database.
    // The results from the database will eventually be sent to the client.
    search_req sr;

    uint32 bitOffset = 0;

    unsigned char sortDescending = 0;
    unsigned char isPresent      = 0;
    unsigned char areaCount      = 0;

    char  name[16] = {};
    uint8 nameLen  = 0;

    uint8 minLvl = 0;
    uint8 maxLvl = 0;

    uint8 jobid    = 0;
    uint8 raceid   = 255; // 255 because race 0 is an actual filter (hume)
    uint8 nationid = 255; // 255 because nation 0 is an actual filter (sandoria)

    uint8 minRank = 0;
    uint8 maxRank = 0;

    uint16 areas[15] = {};

    uint32 flags = 0;

    uint16 workloadBits = size * 8;

    uint8 commentType = 0;

    while (bitOffset < workloadBits)
    {
        if ((bitOffset + 5) >= workloadBits)
        {
            bitOffset = workloadBits;
            break;
        }

        uint8 EntryType = (uint8)unpackBitsLE(payload, bitOffset, 5);
        bitOffset += 5;

        if ((EntryType != SEARCH_FRIEND) && (EntryType != SEARCH_LINKSHELL) && (EntryType != SEARCH_LINKSHELL2) && (EntryType != SEARCH_COMMENT) && (EntryType != SEARCH_FLAGS2))
        {
            if ((bitOffset + 3) >= workloadBits) // so 0000000 at the end does not get interpreted as name entry
            {
                bitOffset = workloadBits;
                break;
            }
            sortDescending = (unsigned char)unpackBitsLE(payload, bitOffset, 1);
            bitOffset += 1;

            isPresent = (unsigned char)unpackBitsLE(payload, bitOffset, 1);
            bitOffset += 1;
        }

        switch (EntryType)
        {
            case SEARCH_NAME:
            {
                if (isPresent == 0x1) // Name send
                {
                    if ((bitOffset + 5) >= workloadBits)
                    {
                        bitOffset = workloadBits;
                        break;
                    }
                    nameLen       = (unsigned char)unpackBitsLE(payload, bitOffset, 5);
                    name[nameLen] = '\0';

                    bitOffset += 5;

                    for (unsigned char i = 0; i < nameLen; i++)
                    {
                        name[i] = (char)unpackBitsLE(payload, bitOffset, 7);
                        bitOffset += 7;
                    }
                }
                break;
            }
            case SEARCH_AREA: // Area Code Entry - 10 bit
            {
                if (isPresent == 0) // no more Area entries
                {
                    ShowTraceFmt("Area List End found.");
                }
                else // 8 Bit = 1 Byte per Area Code
                {
                    areas[areaCount] = (uint16)unpackBitsLE(payload, bitOffset, 10);
                    areaCount++;
                    bitOffset += 10;
                }
                break;
            }
            case SEARCH_NATION: // Country - 2 bit
            {
                if (isPresent == 0x1)
                {
                    unsigned char country = (unsigned char)unpackBitsLE(payload, bitOffset, 2);
                    bitOffset += 2;
                    nationid = country;

                    ShowInfoFmt("Nationality Entry found. ({}) Sorting: ({}).", hex8ToString(country), (sortDescending == 0x00) ? "ascending" : "descending");
                }
                break;
            }
            case SEARCH_JOB: // Job - 5 bit
            {
                if (isPresent == 0x1)
                {
                    unsigned char job = (unsigned char)unpackBitsLE(payload, bitOffset, 5);
                    bitOffset += 5;
                    jobid = job;
                }
                break;
            }
            case SEARCH_LEVEL: // Level- 16 bit
            {
                if (isPresent == 0x1)
                {
                    unsigned char fromLvl = (unsigned char)unpackBitsLE(payload, bitOffset, 8);
                    bitOffset += 8;
                    unsigned char toLvl = (unsigned char)unpackBitsLE(payload, bitOffset, 8);
                    bitOffset += 8;
                    minLvl = fromLvl;
                    maxLvl = toLvl;
                }
                break;
            }
            case SEARCH_RACE: // Race - 4 bit
            {
                if (isPresent == 0x1)
                {
                    unsigned char race = (unsigned char)unpackBitsLE(payload, bitOffset, 4);
                    bitOffset += 4;
                    raceid = race;

                    ShowInfoFmt("Race Entry found. ({}) Sorting: ({}).", hex8ToString(race), (sortDescending == 0x00) ? "ascending" : "descending");
                }
                ShowInfoFmt("SortByRace: {}.", (sortDescending == 0x00) ? "ascending" : "descending");
                break;
            }
            case SEARCH_RANK: // Rank - 2 byte
            {
                if (isPresent == 0x1)
                {
                    unsigned char fromRank = (unsigned char)unpackBitsLE(payload, bitOffset, 8);
                    bitOffset += 8;
                    minRank              = fromRank;
                    unsigned char toRank = (unsigned char)unpackBitsLE(payload, bitOffset, 8);
                    bitOffset += 8;
                    maxRank = toRank;

                    ShowInfoFmt("Rank Entry found. ({} - {}) Sorting: ({}).", fromRank, toRank, (sortDescending == 0x00) ? "ascending" : "descending");
                }
                ShowInfoFmt("SortByRank: {}.", (sortDescending == 0x00) ? "ascending" : "descending");
                break;
            }
            case SEARCH_COMMENT: // 4 Byte
            {
                commentType = (uint8)unpackBitsLE(payload, bitOffset, 32);
                bitOffset += 32;

                ShowInfoFmt("Comment Entry found. ({}).", hex8ToString(commentType));
                break;
            }
            // the following 4 Entries were generated with /sea (ballista|friend|linkshell|away|inv)
            // so they may be off
            case SEARCH_LINKSHELL: // 4 Byte
            {
                sr.lsId = static_cast<uint32>(unpackBitsLE(payload, bitOffset, 32));
                bitOffset += 32;

                ShowInfoFmt("Linkshell Entry found. Value: {}", hex32ToString(sr.lsId.value()));
                break;
            }
            case SEARCH_LINKSHELL2: // 4 Byte
            {
                sr.lsId = static_cast<uint32>(unpackBitsLE(payload, bitOffset, 32));
                bitOffset += 32;

                ShowInfoFmt("Linkshell2 Entry found. Value: {}", hex32ToString(sr.lsId.value()));
                break;
            }
            case SEARCH_FRIEND: // Friend Packet, 0 byte
            {
                ShowInfoFmt("Friend Entry found.");
                break;
            }
            case SEARCH_FLAGS1: // Flag Entry #1, 2 byte,
            {
                if (isPresent == 0x1)
                {
                    unsigned short flags1 = (unsigned short)unpackBitsLE(payload, bitOffset, 16);
                    bitOffset += 16;

                    ShowInfoFmt("Flag Entry #1 ({}) found. Sorting: ({}).", hex16ToString(flags1), (sortDescending == 0x00) ? "ascending" : "descending");

                    flags = flags1;
                }
                ShowInfoFmt("SortByFlags: {}", (sortDescending == 0 ? "ascending" : "descending"));
                break;
            }
            case SEARCH_FLAGS2: // Flag Entry #2 - 4 byte
            {
                unsigned int flags2 = (unsigned int)unpackBitsLE(payload, bitOffset, 32);

                bitOffset += 32;
                flags = flags2;
                break;
            }
            default:
            {
                ShowInfoFmt("Unknown Search Param {}!", EntryType);
                break;
            }
        }
    }

    const auto printableName = nameLen > 0 ? name : "<empty>";
    ShowInfoFmt("Name: {} Job: {} Lvls: {} ~ {}", printableName, jobid, minLvl, maxLvl);

    sr.jobid  = jobid;
    sr.maxlvl = maxLvl;
    sr.minlvl = minLvl;

    sr.race        = raceid;
    sr.nation      = nationid;
    sr.minRank     = minRank;
    sr.maxRank     = maxRank;
    sr.flags       = flags;
    sr.commentType = commentType;

    sr.nameLen = nameLen;
    memcpy(&sr.zoneid, areas, sizeof(sr.zoneid));
    if (nameLen > 0)
    {
        sr.name.insert(0, name);
    }

    return sr;
    // Do not process the last bits, which can interfere with other operations
    // For example: "/blacklist delete Name" and "/sea all Name"
}
