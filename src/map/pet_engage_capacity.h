#pragma once

// Pure AttackTarget / RetreatToMaster gates from petutils.
// Parity: internal/petutils/engage.go (slice 1627).

namespace petengagehelpers
{

inline auto CanAttackTarget(const bool hasMaster, const bool hasPet, const bool hasTarget) -> bool
{
    return hasMaster && hasPet && hasTarget;
}

inline auto ShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

inline auto CanRetreatToMaster(const bool hasMaster, const bool hasPet) -> bool
{
    return hasMaster && hasPet;
}

inline auto ShouldPetDisengage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace petengagehelpers
