#pragma once

namespace rangedactorfinalizehelpers
{

struct RangedActorFinalization
{
    bool claimTarget;
    bool removeAmmo;
    bool stripAllDetectable;
    bool updateTargetLastAttacked;

    constexpr auto operator==(const RangedActorFinalization&) const -> bool = default;
};

// ResolveRangedActorFinalization captures the actor-type branches at the tail
// of CBattleEntity::OnRangedAttack. Character cleanup wins if flags overlap.
inline auto ResolveRangedActorFinalization(const bool isChar, const bool isTrust) -> RangedActorFinalization
{
    return {
        .claimTarget              = isChar || isTrust,
        .removeAmmo               = isChar,
        .stripAllDetectable       = !isChar,
        .updateTargetLastAttacked = !isChar,
    };
}

} // namespace rangedactorfinalizehelpers
