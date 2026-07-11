#pragma once

namespace mobdeathhelpers
{

inline auto ShouldKillSummonerPet(const bool hasPet, const bool petAlive, const bool mainJobSummoner) -> bool
{
    return hasPet && petAlive && mainJobSummoner;
}

inline auto ShouldDetachPlayerMaster(const bool hasMaster, const bool masterPetIsMob, const bool masterIsPlayer) -> bool
{
    return hasMaster && masterPetIsMob && masterIsPlayer;
}

template <typename BattlefieldDeath, typename ClearEnmity, typename ClearStates, typename KillPet,
          typename EnterDeath, typename FinalizeBase, typename QueueRewards, typename DetachMaster>
inline void Apply(
    const bool hasBattlefield,
    const bool killSummonerPet,
    const bool detachPlayerMaster,
    BattlefieldDeath&& battlefieldDeath,
    ClearEnmity&& clearEnmity,
    ClearStates&& clearStates,
    KillPet&& killPet,
    EnterDeath&& enterDeath,
    FinalizeBase&& finalizeBase,
    QueueRewards&& queueRewards,
    DetachMaster&& detachMaster)
{
    if (hasBattlefield)
    {
        battlefieldDeath();
    }
    clearEnmity();
    clearStates();
    if (killSummonerPet)
    {
        killPet();
    }
    enterDeath();
    finalizeBase();
    queueRewards();
    if (detachPlayerMaster)
    {
        detachMaster();
    }
}

} // namespace mobdeathhelpers
