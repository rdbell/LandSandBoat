#pragma once

#include <functional>
#include <utility>

namespace charposttickrefreshhelpers
{

template <typename BasePostTick, typename ReloadRequested, typename ReloadParty,
          typename SendStatus, typename SendSync, typename SendExtendedJobs,
          typename SendStatusIcons, typename HasParty, typename PushPartyEffects>
inline void Apply(
    bool&             effectsChanged,
    BasePostTick&&    basePostTick,
    ReloadRequested&& reloadRequested,
    ReloadParty&&     reloadParty,
    SendStatus&&      sendStatus,
    SendSync&&        sendSync,
    SendExtendedJobs&& sendExtendedJobs,
    SendStatusIcons&&  sendStatusIcons,
    HasParty&&         hasParty,
    PushPartyEffects&& pushPartyEffects)
{
    std::invoke(std::forward<BasePostTick>(basePostTick));

    if (std::invoke(std::forward<ReloadRequested>(reloadRequested)))
    {
        std::invoke(std::forward<ReloadParty>(reloadParty));
    }

    if (!effectsChanged)
    {
        return;
    }

    std::invoke(std::forward<SendStatus>(sendStatus));
    std::invoke(std::forward<SendSync>(sendSync));
    std::invoke(std::forward<SendExtendedJobs>(sendExtendedJobs));
    std::invoke(std::forward<SendStatusIcons>(sendStatusIcons));
    if (std::invoke(std::forward<HasParty>(hasParty)))
    {
        std::invoke(std::forward<PushPartyEffects>(pushPartyEffects));
    }
    effectsChanged = false;
}

} // namespace charposttickrefreshhelpers
