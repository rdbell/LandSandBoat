#pragma once
namespace targetfindoffensivehelpers
{
// ShouldRejectOffensiveAlly mirrors the TARGET_ENEMY same-allegiance gate.
constexpr auto ShouldRejectOffensiveAlly(const bool targetEnemy, const bool targetPlayerParty, const bool sameAllegiance) -> bool
{
    return targetEnemy && !targetPlayerParty && sameAllegiance;
}
} // namespace targetfindoffensivehelpers
