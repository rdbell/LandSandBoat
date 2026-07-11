#pragma once

#include "common/cbasetypes.h"

namespace petzoningrestorehelpers
{

inline auto RestoredTP(const float persistedTP) -> uint16
{
    return static_cast<uint16>(persistedTP);
}

template <typename WarnUnspawned, typename SetTP, typename SetHP, typename SetMP,
          typename SetJugDuration, typename SetJugSpawnTime>
inline void Apply(
    const bool spawned,
    const bool hasCharacterMaster,
    const bool jugPet,
    WarnUnspawned&& warnUnspawned,
    SetTP&& setTP,
    SetHP&& setHP,
    SetMP&& setMP,
    SetJugDuration&& setJugDuration,
    SetJugSpawnTime&& setJugSpawnTime)
{
    if (!spawned)
    {
        warnUnspawned();
        return;
    }
    if (!hasCharacterMaster)
    {
        return;
    }
    setTP();
    setHP();
    setMP();
    if (jugPet)
    {
        setJugDuration();
        setJugSpawnTime();
    }
}

} // namespace petzoningrestorehelpers
