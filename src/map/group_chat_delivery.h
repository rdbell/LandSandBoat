#pragma once

#include "common/ipc_structs.h"
#include "enums/chat_message_type.h"

#include <functional>
#include <utility>

namespace mapipc
{

template <typename Lookup, typename Deliver>
void HandleChatMessageParty(const ipc::ChatMessageParty& message, Lookup&& lookup, Deliver&& deliver)
{
    if (auto* party = std::invoke(lookup, message.partyId))
    {
        std::invoke(deliver, party, message);
    }
}

template <typename Lookup, typename Deliver>
void HandleChatMessageAlliance(const ipc::ChatMessageAlliance& message, Lookup&& lookup, Deliver&& deliver)
{
    if (auto* alliance = std::invoke(lookup, message.allianceId))
    {
        std::invoke(deliver, alliance, message);
    }
}

template <typename Lookup, typename Deliver>
void HandleChatMessageLinkshell(const ipc::ChatMessageLinkshell& message, Lookup&& lookup, Deliver&& deliver)
{
    if (auto* linkshell = std::invoke(lookup, message.linkshellId))
    {
        std::invoke(deliver, linkshell, message, MESSAGE_LINKSHELL);
    }
}

template <typename Lookup, typename Deliver>
void HandleChatMessageUnity(const ipc::ChatMessageUnity& message, Lookup&& lookup, Deliver&& deliver)
{
    if (auto* unityChat = std::invoke(lookup, message.unityLeaderId))
    {
        std::invoke(deliver, unityChat, message);
    }
}

} // namespace mapipc
