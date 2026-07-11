#pragma once

namespace mobdeathrewardhelpers
{

inline auto HasValidLastAttacker(const bool found, const bool entityIDMatches) -> bool
{
    return found && entityIDMatches;
}

template <typename EmitDefeat, typename EmitFall, typename DistributeRewards,
          typename ClearOwner, typename ResetTreasureHunter, typename ResetGilfinder>
inline void Apply(
    const bool mobDead,
    const bool validLastAttacker,
    EmitDefeat&& emitDefeat,
    EmitFall&& emitFall,
    DistributeRewards&& distributeRewards,
    ClearOwner&& clearOwner,
    ResetTreasureHunter&& resetTreasureHunter,
    ResetGilfinder&& resetGilfinder)
{
    if (!mobDead)
    {
        return;
    }

    if (validLastAttacker)
    {
        emitDefeat();
    }
    else
    {
        emitFall();
    }
    distributeRewards();
    clearOwner();
    resetTreasureHunter();
    resetGilfinder();
}

} // namespace mobdeathrewardhelpers
