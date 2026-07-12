#pragma once

#include <functional>
#include <utility>

namespace charactionboundaryhelpers
{

template <typename HasSpell, typename BaseCanUse>
inline bool CanUseSpell(HasSpell&& hasSpell, BaseCanUse&& baseCanUse)
{
    return std::invoke(std::forward<HasSpell>(hasSpell)) &&
           std::invoke(std::forward<BaseCanUse>(baseCanUse));
}

template <typename Now, typename SetLastAttack, typename BaseAttack>
inline bool Attack(Now&& now, SetLastAttack&& setLastAttack, BaseAttack&& baseAttack)
{
    auto attackTime = std::invoke(std::forward<Now>(now));
    std::invoke(std::forward<SetLastAttack>(setLastAttack), attackTime);
    return std::invoke(std::forward<BaseAttack>(baseAttack));
}

template <typename BaseInterrupted, typename HasError, typename TakeError, typename PushError>
inline void CastInterrupted(
    BaseInterrupted&& baseInterrupted,
    HasError&&       hasError,
    TakeError&&      takeError,
    PushError&&      pushError)
{
    std::invoke(std::forward<BaseInterrupted>(baseInterrupted));
    if (!std::invoke(std::forward<HasError>(hasError)))
    {
        return;
    }

    auto message = std::invoke(std::forward<TakeError>(takeError));
    if (message)
    {
        std::invoke(std::forward<PushError>(pushError), std::move(message));
    }
}

} // namespace charactionboundaryhelpers
