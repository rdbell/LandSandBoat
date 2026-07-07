#include "search_player_filter.h"

#include "common/logging.h"
#include "common/types/maybe.h"
#include "data_loader.h"
#include "search.h"

#include <cctype>

namespace
{

auto SearchRaceMatches(const uint8 requestRace, const uint8 playerRace) -> bool
{
    if (requestRace == 255)
    {
        return true;
    }

    if (requestRace == 0 && (playerRace != 1 && playerRace != 2))
    {
        return false;
    }
    if (requestRace == 1 && (playerRace != 3 && playerRace != 4))
    {
        return false;
    }
    if (requestRace == 2 && (playerRace != 5 && playerRace != 6))
    {
        return false;
    }
    if (requestRace == 3 && playerRace != 7)
    {
        return false;
    }
    if (requestRace == 4 && playerRace != 8)
    {
        return false;
    }

    return true;
}

auto SearchNameMatches(const SearchEntity& player, const search_req& request) -> bool
{
    if (request.nameLen == 0)
    {
        return true;
    }

    const auto nameLength = static_cast<std::size_t>(request.nameLen);
    if (nameLength > player.name.length())
    {
        return false;
    }

    for (std::size_t i = 0; i < nameLength; ++i)
    {
        const auto requested = static_cast<unsigned char>(request.name[i]);
        const auto actual    = static_cast<unsigned char>(player.name[i]);
        if (std::tolower(requested) != std::tolower(actual))
        {
            return false;
        }
    }

    return true;
}

auto RequestUsesPrivatePlayerFields(const search_req& request) -> bool
{
    return request.jobid > 0 ||
           request.nation != 255 ||
           request.race != 255 ||
           request.minRank > 0 ||
           request.maxRank > 0 ||
           request.minlvl > 0 ||
           request.maxlvl > 0;
}

} // namespace

auto SearchPlayerMatchesRequest(const SearchEntity& player, const search_req& request) -> bool
{
    if (request.lsId.has_value())
    {
        const auto searchedLsId = request.lsId.value();
        if (searchedLsId == 0)
        {
            return false;
        }

        if (player.linkshellid1 != searchedLsId && player.linkshellid2 != searchedLsId)
        {
            return false;
        }
    }

    if ((player.flags1 & 0x4000) && RequestUsesPrivatePlayerFields(request))
    {
        return false;
    }

    if (request.jobid > 0 && request.jobid != player.mjob)
    {
        return false;
    }

    if (request.nation != 255 && request.nation != player.nation)
    {
        return false;
    }

    if (!SearchRaceMatches(request.race, player.race))
    {
        return false;
    }

    if (request.minRank > 0 && request.maxRank >= request.minRank)
    {
        if (player.rank < request.minRank || player.rank > request.maxRank)
        {
            return false;
        }
    }

    if (request.flags != 0)
    {
        if (const auto searchUnityId = request.flags >> 22; searchUnityId != 0)
        {
            if (player.unityLeader != searchUnityId)
            {
                return false;
            }
        }
        else if (!(player.flags2 & request.flags))
        {
            return false;
        }
    }

    if (request.minlvl > 0 && request.maxlvl >= request.minlvl)
    {
        if (player.mlvl < request.minlvl || player.mlvl > request.maxlvl)
        {
            return false;
        }
    }

    if (!SearchNameMatches(player, request))
    {
        return false;
    }

    if (player.gmHidden)
    {
        return false;
    }

    return true;
}
