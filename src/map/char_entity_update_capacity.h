#pragma once

#include <functional>
#include <utility>

namespace charentityupdatehelpers
{

template <typename UpdateCharacter, typename UpdateEntity, typename NewCharacter, typename NewEntity>
inline void Apply(const bool hasPendingPacket,
                  const bool isCharacter,
                  UpdateCharacter&& updateCharacter,
                  UpdateEntity&&    updateEntity,
                  NewCharacter&&    newCharacter,
                  NewEntity&&       newEntity)
{
    if (hasPendingPacket)
    {
        if (isCharacter)
        {
            std::invoke(std::forward<UpdateCharacter>(updateCharacter));
        }
        else
        {
            std::invoke(std::forward<UpdateEntity>(updateEntity));
        }
        return;
    }

    if (isCharacter)
    {
        std::invoke(std::forward<NewCharacter>(newCharacter));
    }
    else
    {
        std::invoke(std::forward<NewEntity>(newEntity));
    }
}

} // namespace charentityupdatehelpers
