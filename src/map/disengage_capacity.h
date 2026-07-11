#pragma once

#include "battle_transition_capacity.h"

namespace disengagehelpers
{

constexpr uint8 AnimationNone   = battletransitionhelpers::AnimationNone;
constexpr uint8 AnimationAttack = battletransitionhelpers::AnimationAttack;
constexpr uint8 UpdateHP        = battletransitionhelpers::UpdateHP;

struct DisengageState
{
    uint16 battleTarget;
    uint8  animation;
    uint8  updateMask;

    constexpr auto operator==(const DisengageState&) const -> bool = default;
};

// ResolveDisengageState captures CBattleEntity::OnDisengage mutations before
// the host emits its DISENGAGE listener.
inline auto ResolveDisengageState(const uint8 animation, const uint8 updateMask) -> DisengageState
{
    return {
        .battleTarget = 0,
        .animation    = animation == AnimationAttack ? AnimationNone : animation,
        .updateMask   = static_cast<uint8>(updateMask | UpdateHP),
    };
}

} // namespace disengagehelpers
