#pragma once

namespace trustcontrollerdespawn
{

// Apply preserves CTrustController::Despawn's mutation and dispatch order.
template <typename ClearMaster, typename SetDespawnAnimation, typename BaseDespawn>
inline void Apply(ClearMaster&& clearMaster, SetDespawnAnimation&& setDespawnAnimation, BaseDespawn&& baseDespawn)
{
    clearMaster();
    setDespawnAnimation();
    baseDespawn();
}

} // namespace trustcontrollerdespawn
