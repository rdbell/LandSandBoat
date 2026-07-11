#pragma once

namespace despawnfinalizehelpers
{

template <typename FadeOut, typename EmitListener>
inline void Apply(FadeOut&& fadeOut, EmitListener&& emitListener)
{
    fadeOut();
    emitListener();
}

} // namespace despawnfinalizehelpers
