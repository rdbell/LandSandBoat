#pragma once

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace chartrustrosterhelpers
{

template <typename Trust, typename IsSpawned, typename Despawn, typename RequestReload>
inline void Remove(
    std::vector<Trust*>& trusts,
    Trust*               trust,
    IsSpawned&&          isSpawned,
    Despawn&&            despawn,
    RequestReload&&      requestReload)
{
    if (!std::invoke(std::forward<IsSpawned>(isSpawned), trust))
    {
        return;
    }

    const auto trustIt = std::find(trusts.begin(), trusts.end(), trust);
    if (trustIt != trusts.end())
    {
        std::invoke(std::forward<Despawn>(despawn), trust);
        trusts.erase(trustIt);
    }

    std::invoke(std::forward<RequestReload>(requestReload));
}

template <typename Trust, typename Despawn, typename RequestReload>
inline void Clear(std::vector<Trust*>& trusts, Despawn&& despawn, RequestReload&& requestReload)
{
    for (auto* trust : trusts)
    {
        std::invoke(despawn, trust);
    }
    trusts.clear();

    std::invoke(std::forward<RequestReload>(requestReload));
}

} // namespace chartrustrosterhelpers
