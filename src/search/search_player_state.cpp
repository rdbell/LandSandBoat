#include "search_player_state.h"

#include "common/logging.h"
#include "common/mmo.h"
#include "data_loader.h"

#include <cstring>

namespace
{

constexpr uint8 JOB_MON = 23;

auto SearchSettingsFromInt(const uint32 settingsInt) -> SAVE_CONF
{
    SAVE_CONF playerSettings = {};
    std::memcpy(&playerSettings, &settingsInt, sizeof(uint32));
    return playerSettings;
}

} // namespace

auto SearchRankForNation(const uint8 nation, const uint8 rankSandoria, const uint8 rankBastok, const uint8 rankWindurst) -> uint8
{
    switch (nation)
    {
        case 0:
            return rankSandoria;
        case 1:
            return rankBastok;
        case 2:
            return rankWindurst;
        default:
            ShowWarningFmt("Inconsistent player nation allegiance : {}", nation);
            return static_cast<uint8>(0U);
    }
}

void NormalizeSearchPlayerForList(SearchEntity& player, const uint32 settingsInt, const uint32 partyId)
{
    const auto playerSettings = SearchSettingsFromInt(settingsInt);

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

void NormalizeSearchPartyMemberForList(SearchEntity& player, const uint32 settingsInt, const uint32 partyId)
{
    const auto playerSettings = SearchSettingsFromInt(settingsInt);

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

    player.flags2 = player.flags1;
}

void NormalizeSearchLinkshellMemberForList(SearchEntity& player, const uint32 settingsInt, const uint32 partyId)
{
    const auto playerSettings = SearchSettingsFromInt(settingsInt);

    if (partyId == player.id)
    {
        player.flags1 |= 0x0008;
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

    player.flags2 = player.flags1;
}
