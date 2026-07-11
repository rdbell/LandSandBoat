#pragma once

namespace petdeathhelpers
{

inline auto ShouldDespawnForZoning(
    const bool hpPositive,
    const bool hasMaster,
    const bool masterIsPlayer,
    const bool respawnPet) -> bool
{
    return hpPositive && hasMaster && masterIsPlayer && respawnPet;
}

inline auto ShouldDetachPlayerMaster(
    const bool hasMaster,
    const bool masterPetIsSelf,
    const bool masterIsPlayer) -> bool
{
    return hasMaster && masterPetIsSelf && masterIsPlayer;
}

template <typename ClearStates, typename Despawn, typename EnterDeath,
          typename EmitLuaDeath, typename FinalizeBase, typename DetachMaster>
inline void Apply(
    const bool despawnForZoning,
    const bool detachPlayerMaster,
    ClearStates&& clearStates,
    Despawn&& despawn,
    EnterDeath&& enterDeath,
    EmitLuaDeath&& emitLuaDeath,
    FinalizeBase&& finalizeBase,
    DetachMaster&& detachMaster)
{
    clearStates();
    if (despawnForZoning)
    {
        despawn();
    }
    else
    {
        enterDeath();
    }
    emitLuaDeath();
    finalizeBase();
    if (detachPlayerMaster)
    {
        detachMaster();
    }
}

} // namespace petdeathhelpers
