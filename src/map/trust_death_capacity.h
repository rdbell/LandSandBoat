#pragma once

namespace trustdeathhelpers
{

template <typename EmitLuaDeath, typename ClearEnmity, typename ClearStates,
          typename EnterDeath, typename RemoveTrust, typename ClearOwner, typename FinalizeBase>
inline void Apply(
    EmitLuaDeath&& emitLuaDeath,
    ClearEnmity&& clearEnmity,
    ClearStates&& clearStates,
    EnterDeath&& enterDeath,
    RemoveTrust&& removeTrust,
    ClearOwner&& clearOwner,
    FinalizeBase&& finalizeBase)
{
    emitLuaDeath();
    clearEnmity();
    clearStates();
    enterDeath();
    removeTrust();
    clearOwner();
    finalizeBase();
}

} // namespace trustdeathhelpers
