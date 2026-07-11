#pragma once

#include "battle_transition_capacity.h"

namespace engagehelpers
{

struct EngageState
{
    uint8 animation;
    uint8 updateMask;

    constexpr auto operator==(const EngageState&) const -> bool = default;
};

inline auto ResolveState(const uint8 updateMask) -> EngageState
{
    return {
        .animation  = battletransitionhelpers::AnimationAttack,
        .updateMask = static_cast<uint8>(updateMask | battletransitionhelpers::UpdateHP),
    };
}

template <typename ApplyState, typename EmitListener>
inline void Apply(const uint8 updateMask, ApplyState&& applyState, EmitListener&& emitListener)
{
    applyState(ResolveState(updateMask));
    emitListener();
}

} // namespace engagehelpers
