#pragma once

#include <cstdint>
#include <functional>

namespace worldipc
{

template <typename Message, typename Lookup, typename Send>
void RerouteMessageToLinkshellMembers(std::uint32_t linkshellId, const Message& message, Lookup&& lookup, Send&& send)
{
    const auto endpoints = std::invoke(lookup, linkshellId);
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
