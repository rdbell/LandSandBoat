#pragma once

namespace petcontrollerdeaggro
{

inline auto ShouldDeaggro(bool hasTarget, bool targetDead, bool targetMounted, bool sameZone,
                          bool sameConfrontation, bool sameBattle) -> bool
{
    return !hasTarget || targetDead || targetMounted || !sameZone || !sameConfrontation || !sameBattle;
}

} // namespace petcontrollerdeaggro
