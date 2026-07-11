#pragma once

#include "common/ipc_structs.h"

#include <cstddef>
#include <functional>

namespace mapipc
{

template <typename Lookup, typename RandomWord, typename RandomHash>
void HandleAccountLogin(const ipc::AccountLogin& message, Lookup&& lookup, RandomWord&& randomWord, RandomHash&& randomHash)
{
    auto* session = std::invoke(lookup, message.accountId);
    if (!session)
    {
        return;
    }

    session->forceLinkDead = true;

    for (auto& value : session->blowfish.key)
    {
        value = std::invoke(randomWord);
    }
    for (auto& value : session->prev_blowfish.key)
    {
        value = std::invoke(randomWord);
    }
    for (auto& value : session->blowfish.P)
    {
        value = std::invoke(randomWord);
    }
    for (auto& value : session->prev_blowfish.P)
    {
        value = std::invoke(randomWord);
    }
    for (auto& value : session->blowfish.hash)
    {
        value = static_cast<uint8>(std::invoke(randomHash) % 255);
    }
    for (auto& value : session->prev_blowfish.hash)
    {
        value = static_cast<uint8>(std::invoke(randomHash) % 255);
    }
    for (std::size_t box = 0; box < 4; ++box)
    {
        for (auto& value : session->blowfish.S[box])
        {
            value = std::invoke(randomWord);
        }
        for (auto& value : session->prev_blowfish.S[box])
        {
            value = std::invoke(randomWord);
        }
    }
}

} // namespace mapipc
