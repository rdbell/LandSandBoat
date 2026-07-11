#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace mapipc
{

struct ChatMessageTellRecipientState
{
    bool valid{};
    bool hidden{};
};

template <typename InPrison>
auto InspectChatMessageTellRecipient(const bool disappeared, InPrison&& inPrison, const bool hidden) -> ChatMessageTellRecipientState
{
    if (disappeared || std::invoke(inPrison))
    {
        return {};
    }
    return {
        .valid  = true,
        .hidden = hidden,
    };
}

template <typename Lookup, typename Inspect, typename BlockHidden, typename Away, typename Feedback, typename Deliver>
void HandleChatMessageTell(const ipc::ChatMessageTell& message, Lookup&& lookup, Inspect&& inspect, BlockHidden&& blockHidden,
                           Away&& away, Feedback&& feedback, Deliver&& deliver)
{
    const auto sendFeedback = [&](const MsgStd code)
    {
        std::invoke(feedback, ipc::MessageStandard{
                                  .recipientId = message.senderId,
                                  .message     = code,
                              });
    };

    auto* character = std::invoke(lookup, message.recipientName);
    if (!character)
    {
        sendFeedback(MsgStd::TellNotReceivedOffline);
        return;
    }

    const auto state = std::invoke(inspect, character);
    if (!state.valid)
    {
        sendFeedback(MsgStd::TellNotReceivedOffline);
        return;
    }

    const auto gmSent = message.gmLevel > 0;
    if (std::invoke(blockHidden) && state.hidden && !gmSent)
    {
        sendFeedback(MsgStd::TellNotReceivedOffline);
    }
    else if (std::invoke(away, character) && !gmSent)
    {
        sendFeedback(MsgStd::TellNotReceivedAway);
    }
    else
    {
        std::invoke(deliver, character, message);
    }
}

} // namespace mapipc
