#pragma once

#include <functional>
#include <utility>

namespace charcombattransitionhelpers
{

template <typename RelinquishClaim, typename SendAssist, typename CheckTargetLatents>
inline void ChangeTarget(
    RelinquishClaim&& relinquishClaim,
    SendAssist&&      sendAssist,
    CheckTargetLatents&& checkTargetLatents)
{
    std::invoke(std::forward<RelinquishClaim>(relinquishClaim));
    std::invoke(std::forward<SendAssist>(sendAssist));
    std::invoke(std::forward<CheckTargetLatents>(checkTargetLatents));
}

template <typename Counter, typename BaseEngage, typename CheckTargetLatents>
inline void Engage(
    Counter&          battlesFought,
    BaseEngage&&      baseEngage,
    CheckTargetLatents&& checkTargetLatents)
{
    std::invoke(std::forward<BaseEngage>(baseEngage));
    std::invoke(std::forward<CheckTargetLatents>(checkTargetLatents));
    ++battlesFought;
}

template <typename RelinquishClaim, typename BaseDisengage, typename HasError,
          typename PushError, typename CheckWeaponDrawLatents>
inline void Disengage(
    RelinquishClaim&& relinquishClaim,
    BaseDisengage&&   baseDisengage,
    HasError&&        hasError,
    PushError&&       pushError,
    CheckWeaponDrawLatents&& checkWeaponDrawLatents)
{
    std::invoke(std::forward<RelinquishClaim>(relinquishClaim));
    std::invoke(std::forward<BaseDisengage>(baseDisengage));
    if (std::invoke(std::forward<HasError>(hasError)))
    {
        std::invoke(std::forward<PushError>(pushError));
    }
    std::invoke(std::forward<CheckWeaponDrawLatents>(checkWeaponDrawLatents), false);
}

} // namespace charcombattransitionhelpers
