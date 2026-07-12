#pragma once

#include <cstdint>
#include <functional>

namespace chartargetresolverhelpers
{

enum class Decision : std::uint8_t
{
    Accept,
    Blocked,
    CannotOnThatTarget,
    AlreadyClaimed,
    CannotAttack,
};

template <typename AidBlocked, typename IsMobOwner, typename FallbackNoAssist>
inline auto Apply(const bool baseFound,
                  const bool targetPlayer,
                  const bool targetAlive,
                  const std::uint16_t targetFlags,
                  AidBlocked&& aidBlocked,
                  IsMobOwner&& isMobOwner,
                  FallbackNoAssist&& fallbackNoAssist) -> Decision
{
    if (baseFound)
    {
        if (targetPlayer && std::invoke(aidBlocked))
        {
            return Decision::Blocked;
        }
        if (std::invoke(isMobOwner))
        {
            return targetAlive || (targetFlags & 0x0020) != 0 ? Decision::Accept : Decision::CannotOnThatTarget;
        }
        return Decision::AlreadyClaimed;
    }
    return std::invoke(fallbackNoAssist) ? Decision::CannotOnThatTarget : Decision::CannotAttack;
}

} // namespace chartargetresolverhelpers
