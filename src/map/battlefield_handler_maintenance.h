#pragma once

#include <cstddef>

// Pure CBattlefieldHandler capacity and orphan-maintenance decisions.
namespace battlefieldhandlerhelpers
{

// HasReachedMaxCapacity mirrors m_Battlefields.size() >= m_MaxBattlefields.
inline auto HasReachedMaxCapacity(const std::size_t activeBattlefields, const std::size_t maxBattlefields) -> bool
{
    return activeBattlefields >= maxBattlefields;
}

struct OrphanedPlayerPlan
{
    bool keepPending{};
    bool kickAndClear{};
    bool remove{};

    auto operator==(const OrphanedPlayerPlan&) const -> bool = default;
};

// PlanOrphanedPlayer mirrors HandleBattlefields' orphan loop. Before expiry
// the entry remains; on/after expiry it is removed, and a present character
// receives the kick and confrontation-clear host effects.
inline auto PlanOrphanedPlayer(const bool beforeExpiry, const bool characterPresent) -> OrphanedPlayerPlan
{
    if (beforeExpiry)
    {
        return { true, false, false };
    }
    return { false, characterPresent, true };
}

} // namespace battlefieldhandlerhelpers
