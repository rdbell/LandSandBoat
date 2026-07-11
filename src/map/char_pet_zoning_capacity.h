#pragma once

namespace charpetzoninghelpers
{

template <typename KeepJug, typename SetPetID, typename SaveJugTimers, typename SaveVitals, typename SetRespawn>
inline void ApplyCapture(
    const bool isJug,
    const bool isAvatar,
    const bool isAutomaton,
    const bool isWyvern,
    const bool transientAvatar,
    KeepJug&& keepJug,
    SetPetID&& setPetID,
    SaveJugTimers&& saveJugTimers,
    SaveVitals&& saveVitals,
    SetRespawn&& setRespawn)
{
    setPetID();
    bool persistVitals = false;
    if (isJug)
    {
        if (keepJug())
        {
            saveJugTimers();
            persistVitals = !transientAvatar;
        }
    }
    else if (isAvatar)
    {
        persistVitals = !transientAvatar;
    }
    else if (isAutomaton || isWyvern)
    {
        persistVitals = true;
    }

    if (persistVitals)
    {
        saveVitals();
    }
    // LSB sets this even when the selected pet type did not persist vitals.
    setRespawn();
}

template <typename ClearLevel, typename ClearHP, typename ClearTP, typename ClearMP,
          typename ClearRespawn, typename ResetType, typename ClearJugSpawn, typename ClearJugDuration>
inline void ApplyReset(
    ClearLevel&& clearLevel,
    ClearHP&& clearHP,
    ClearTP&& clearTP,
    ClearMP&& clearMP,
    ClearRespawn&& clearRespawn,
    ResetType&& resetType,
    ClearJugSpawn&& clearJugSpawn,
    ClearJugDuration&& clearJugDuration)
{
    clearLevel();
    clearHP();
    clearTP();
    clearMP();
    clearRespawn();
    resetType();
    clearJugSpawn();
    clearJugDuration();
    // petID is purposefully not reset upstream.
}

template <typename KeepJug>
inline auto ShouldPersist(
    const bool hasLivePet,
    const bool respawnPet,
    const bool isWyvern,
    const bool isAvatar,
    const bool isAutomaton,
    const bool isJug,
    KeepJug&& keepJug) -> bool
{
    if (!hasLivePet && !respawnPet)
    {
        return false;
    }
    return isWyvern || isAvatar || isAutomaton || (isJug && keepJug());
}

} // namespace charpetzoninghelpers
