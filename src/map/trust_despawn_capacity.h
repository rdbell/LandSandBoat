#pragma once

namespace trustdespawnhelpers
{

template <typename HasHP, typename OnMobDespawn, typename FadeOut, typename EmitListener>
inline void Apply(HasHP&& hasHP, OnMobDespawn&& onMobDespawn, FadeOut&& fadeOut, EmitListener&& emitListener)
{
    if (hasHP())
    {
        onMobDespawn();
    }
    fadeOut();
    emitListener();
}

} // namespace trustdespawnhelpers
