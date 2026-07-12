#pragma once

#include <functional>
#include <utility>

namespace chareventactivationhelpers
{

template <typename PromoteCurrent, typename ResetPreparation, typename SetCutsceneSubstate,
          typename IsHealing, typename ClearHealing, typename HasPet, typename DisengagePet,
          typename TargetIsNpc, typename PauseNpc, typename IsCutscene, typename SetLocked>
inline void ActivateState(
    PromoteCurrent&&      promoteCurrent,
    ResetPreparation&&    resetPreparation,
    SetCutsceneSubstate&& setCutsceneSubstate,
    IsHealing&&           isHealing,
    ClearHealing&&        clearHealing,
    HasPet&&              hasPet,
    DisengagePet&&        disengagePet,
    TargetIsNpc&&         targetIsNpc,
    PauseNpc&&            pauseNpc,
    IsCutscene&&          isCutscene,
    SetLocked&&           setLocked)
{
    std::invoke(std::forward<PromoteCurrent>(promoteCurrent));
    std::invoke(std::forward<ResetPreparation>(resetPreparation));
    std::invoke(std::forward<SetCutsceneSubstate>(setCutsceneSubstate));
    if (std::invoke(std::forward<IsHealing>(isHealing)))
    {
        std::invoke(std::forward<ClearHealing>(clearHealing));
    }
    if (std::invoke(std::forward<HasPet>(hasPet)))
    {
        std::invoke(std::forward<DisengagePet>(disengagePet));
    }
    if (std::invoke(std::forward<TargetIsNpc>(targetIsNpc)))
    {
        std::invoke(std::forward<PauseNpc>(pauseNpc));
    }
    std::invoke(std::forward<SetLocked>(setLocked), std::invoke(std::forward<IsCutscene>(isCutscene)));
}

} // namespace chareventactivationhelpers
