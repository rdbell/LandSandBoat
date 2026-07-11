#pragma once

#include "common/ipc_structs.h"
#include "entities/base_entity.h"

#include <functional>
#include <optional>

namespace mapipc
{

struct EntityInformationPosition
{
    float x{};
    float y{};
    float z{};
};

struct EntityInformationTarget
{
    uint32      id{};
    ENTITYTYPE  objType{ TYPE_NONE };
    STATUS_TYPE status{ STATUS_TYPE::DISAPPEAR };
    bool        hasZone{};
    uint16      zoneId{};
    float       x{};
    float       y{};
    float       z{};
    uint8       rot{};
    uint32      moghouseId{};
};

template <typename CharacterLookup, typename EntityLookup, typename SpawnLookup, typename Send, typename Warn>
void HandleEntityInformationRequest(
    const ipc::EntityInformationRequest& message,
    CharacterLookup&&                    characterLookup,
    EntityLookup&&                       entityLookup,
    SpawnLookup&&                        spawnLookup,
    Send&&                               send,
    Warn&&                               warn)
{
    const auto target = message.entityType & TYPE_PC ? std::invoke(characterLookup, message.targetId) : std::invoke(entityLookup, message.targetId);

    if (!target || !target->hasZone)
    {
        std::invoke(warn, message.targetId);
        return;
    }

    const bool isSpawned = target->status != STATUS_TYPE::DISAPPEAR;

    EntityInformationPosition position{};
    if ((message.entityType & TYPE_MOB) && !isSpawned)
    {
        if (const auto spawnPosition = std::invoke(spawnLookup, target->id))
        {
            position = *spawnPosition;
        }
    }
    else
    {
        position = { target->x, target->y, target->z };
    }

    std::invoke(
        send,
        ipc::EntityInformationResponse{
            .requesterId = message.requesterId,
            .targetId    = message.targetId,
            .entityType  = message.entityType,
            .warp        = message.warp && isSpawned,
            .zoneId      = target->zoneId,
            .x           = position.x,
            .y           = position.y,
            .z           = position.z,
            .rot         = target->rot,
            .moghouseId  = target->objType == TYPE_PC ? target->moghouseId : 0,
        });
}

} // namespace mapipc
