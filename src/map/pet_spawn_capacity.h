#pragma once

namespace petspawnhelpers
{

inline auto ShouldInitializeElemental(
    const bool hasMaster,
    const bool masterIsPlayer,
    const bool elementalEcosystem) -> bool
{
    return hasMaster && masterIsPlayer && elementalEcosystem;
}

template <typename SetMagicDelay, typename SetMagicCooldown, typename LoadSpells,
          typename SetJugSpawnTime, typename SpawnBase, typename EmitLuaSpawn>
inline void Apply(
    const bool initializeElemental,
    const bool jugPet,
    SetMagicDelay&& setMagicDelay,
    SetMagicCooldown&& setMagicCooldown,
    LoadSpells&& loadSpells,
    SetJugSpawnTime&& setJugSpawnTime,
    SpawnBase&& spawnBase,
    EmitLuaSpawn&& emitLuaSpawn)
{
    if (initializeElemental)
    {
        setMagicDelay();
        setMagicCooldown();
        loadSpells();
    }
    if (jugPet)
    {
        setJugSpawnTime();
    }
    spawnBase();
    emitLuaSpawn();
}

} // namespace petspawnhelpers
