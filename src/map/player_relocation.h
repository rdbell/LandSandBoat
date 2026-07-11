#pragma once

#include "common/ipc_structs.h"
#include "entities/base_entity.h"

#include <cstdint>
#include <functional>

namespace mapipc
{

enum class RelocationRequest : uint8
{
    Warp,
    ZoneChange,
};

struct PlayerRelocation
{
    uint16 zoneId{};
    float  x{};
    float  y{};
    float  z{};
    uint8  rot{};
    uint32 moghouseId{};
};

template <typename Player>
void ApplyPlayerRelocation(Player& player, const PlayerRelocation& relocation, const RelocationRequest request)
{
    player.loc.p.x         = relocation.x;
    player.loc.p.y         = relocation.y;
    player.loc.p.z         = relocation.z;
    player.loc.p.rotation  = relocation.rot;
    player.loc.destination = relocation.zoneId;

    player.m_moghouseID = relocation.moghouseId;
    player.loc.boundary  = 0;
    player.updatemask    = 0;

    player.status    = STATUS_TYPE::DISAPPEAR;
    player.animation = ANIMATION_NONE;

    player.clearPacketList();

    if (request == RelocationRequest::Warp)
    {
        player.requestedWarp = true;
    }
    else
    {
        player.requestedZoneChange = true;
    }

    if (player.shouldPetPersistThroughZoning())
    {
        player.setPetZoningInfo();
    }
}

template <typename Lookup>
void HandleSendPlayerToLocation(const ipc::SendPlayerToLocation& message, Lookup&& lookup)
{
    auto* player = std::invoke(lookup, message.targetId);
    if (!player || !player->loc.zone)
    {
        return;
    }

    ApplyPlayerRelocation(
        *player,
        PlayerRelocation{
            .zoneId     = message.zoneId,
            .x          = message.x,
            .y          = message.y,
            .z          = message.z,
            .rot        = message.rot,
            .moghouseId = message.moghouseId,
        },
        RelocationRequest::Warp);
}

template <typename Lookup>
void HandleEntityInformationResponse(const ipc::EntityInformationResponse& message, Lookup&& lookup)
{
    auto* player = std::invoke(lookup, message.requesterId);
    if (!player || !player->loc.zone || !message.warp)
    {
        return;
    }

    ApplyPlayerRelocation(
        *player,
        PlayerRelocation{
            .zoneId     = message.zoneId,
            .x          = message.x,
            .y          = message.y,
            .z          = message.z,
            .rot        = message.rot,
            .moghouseId = message.moghouseId,
        },
        RelocationRequest::ZoneChange);
}

} // namespace mapipc
