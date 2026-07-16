#pragma once

#include "0x061_clistatus.h"

#include <algorithm>
#include <array>
#include <iterator>

namespace clistatushelpers
{

struct Facts
{
    CLISTATUS            status{};
    std::array<int16, 7> baseParamModifiers{};
    int16                twoHandedSTR{};
    bool                 mainWeaponTwoHanded{};
    uint8                unityLeader{};
    uint8                unityRank{};
    uint32               unityAccolades{};
    uint32               currentAccolades{};
    uint32               previousAccolades{};
    bool                 unityChat{};
};

[[nodiscard]] inline auto PlanFor(const Facts& facts) -> GP_SERV_COMMAND_CLISTATUS::PacketData
{
    auto  packet = GP_SERV_COMMAND_CLISTATUS::PacketData{};
    auto& status = packet.statusdata;
    status       = facts.status;

    for (std::size_t index = 0; index < std::size(status.bp_adj); ++index)
    {
        auto modifier = facts.baseParamModifiers[index];
        if (index == 0 && facts.mainWeaponTwoHanded)
        {
            modifier = static_cast<int16>(modifier + facts.twoHandedSTR);
        }
        status.bp_adj[index] = std::clamp<int16>(modifier, static_cast<int16>(-999 + status.bp_base[index]), static_cast<int16>(999 - status.bp_base[index]));
    }

    status.MonsterBuster         = 0;
    status.myroom                = 0;
    status.padding4F             = 0;
    status.unity_info            = {};
    status.unity_info.Faction    = facts.unityLeader;
    status.unity_info.Unknown    = facts.unityRank;
    status.unity_info.Points     = facts.unityAccolades;
    status.unity_points1         = static_cast<uint16>(facts.currentAccolades / 1000);
    status.unity_points2         = static_cast<uint16>(facts.previousAccolades / 1000);
    status.unity_chat_color_flag = facts.unityChat ? 1 : 0;
    status.mastery_info          = {};
    status.mastery_exp_now       = 0;
    status.mastery_exp_next      = 0;

    return packet;
}

} // namespace clistatushelpers
