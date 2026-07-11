#pragma once

namespace deathfinalizehelpers
{

template <typename ClearClaims, typename EmitWithKiller, typename EmitWithoutKiller, typename ClearTarget>
inline void Apply(
    const bool hasKiller,
    ClearClaims&& clearClaims,
    EmitWithKiller&& emitWithKiller,
    EmitWithoutKiller&& emitWithoutKiller,
    ClearTarget&& clearTarget)
{
    if (hasKiller)
    {
        clearClaims();
        emitWithKiller();
    }
    else
    {
        emitWithoutKiller();
    }

    clearTarget();
}

} // namespace deathfinalizehelpers
