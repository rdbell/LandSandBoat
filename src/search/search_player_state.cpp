#include "search_player_state.h"

#include "common/mmo.h"
#include "data_loader.h"

#include <cstring>

namespace
{

constexpr uint8 JOB_MON = 23;

} // namespace

void NormalizeSearchPlayerForList(SearchEntity& player, const uint32 settingsInt, const uint32 partyId)
{
    SAVE_CONF playerSettings = {};
    std::memcpy(&playerSettings, &settingsInt, sizeof(uint32));

    player.zone   = player.zone == 0 ? player.prevzone : player.zone;
    player.mentor = playerSettings.MentorFlg;

    if (player.mentor)
    {
        player.flags1 |= 0x0001;
    }

    if (partyId == player.id)
    {
        player.flags1 |= 0x0008;
    }

    if (player.seacom_type)
    {
        player.flags1 |= 0x0010;
    }

    if (playerSettings.AwayFlg)
    {
        player.flags1 |= 0x0100;
    }

    if (player.disconnecting)
    {
        player.flags1 |= 0x0800;
    }

    if (partyId != 0)
    {
        player.flags1 |= 0x2000;
    }

    if (playerSettings.AnonymityFlg)
    {
        player.flags1 |= 0x4000;
    }

    if (playerSettings.InviteFlg)
    {
        player.flags1 |= 0x8000;
    }

    if (player.muted)
    {
        player.flags1 |= 0x20000000;
    }

    player.flags2 = player.flags1;

    if (player.mjob == JOB_MON || player.sjob == JOB_MON)
    {
        player.mjob = 0;
        player.sjob = 0;
    }
}
