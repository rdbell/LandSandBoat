#pragma once

#include <functional>
#include <string>

namespace worldipc
{

template <typename Message, typename Lookup, typename Send>
void RerouteMessageToCharName(const std::string& charName, const Message& message, Lookup&& lookup, Send&& send)
{
    const auto endpoint = std::invoke(lookup, charName);
    if (!endpoint)
    {
        return;
    }
    std::invoke(send, *endpoint, message);
}

} // namespace worldipc
