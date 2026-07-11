#pragma once

#include "common/ipc_structs.h"
#include "packets/c2s/0x0b7_assist_channel.h"

#include <functional>

namespace mapipc
{

template <typename Lookup>
void HandleAssistChannelEvent(const ipc::AssistChannelEvent& message, Lookup&& lookup)
{
    auto* receiver = std::invoke(lookup, message.receiverId);
    if (!receiver)
    {
        return;
    }

    switch (static_cast<GP_CLI_COMMAND_ASSIST_CHANNEL_KIND>(message.action))
    {
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList:
            receiver->aman().mute(message.senderId);
            break;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::RemoveFromMuteList:
            receiver->aman().unmute(message.senderId);
            break;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp:
            receiver->aman().addThumbsUp(message.senderId);
            break;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning:
            receiver->aman().addThumbsDown(message.senderId);
            break;
    }
}

} // namespace mapipc
