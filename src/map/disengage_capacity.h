#pragma once

#include "common/cbasetypes.h"

namespace disengagehelpers
{

constexpr uint8 AnimationNone   = 0;
constexpr uint8 AnimationAttack = 1;
constexpr uint8 UpdateHP        = 0x04;

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
