#pragma once

namespace trustspawnhelpers
{

template <typename SpawnBase, typename EmitLuaSpawn, typename UpdateHealth,
          typename RefillHP, typename RefillMP, typename MarkHealthUpdate, typename SendNamePacket>
inline void Apply(
    SpawnBase&& spawnBase,
    EmitLuaSpawn&& emitLuaSpawn,
    UpdateHealth&& updateHealth,
    RefillHP&& refillHP,
    RefillMP&& refillMP,
    MarkHealthUpdate&& markHealthUpdate,
    SendNamePacket&& sendNamePacket)
{
    spawnBase();
    emitLuaSpawn();
    updateHealth();
    refillHP();
    refillMP();
    markHealthUpdate();
    sendNamePacket();
}

} // namespace trustspawnhelpers
