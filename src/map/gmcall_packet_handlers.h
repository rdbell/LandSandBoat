#pragma once

#include "packets/c2s/0x0d3_faq_gmcall.h"
#include "packets/c2s/0x0d5_ack_gmmsg.h"

#include <functional>
#include <utility>

namespace gmcall::handler
{

template <typename AddPacket, typename ProcessCall, typename IncrementHistory>
void HandleFAQGMCall(const GP_CLI_COMMAND_FAQ_GMCALL& packet, AddPacket&& addPacket, ProcessCall&& processCall, IncrementHistory&& incrementHistory)
{
    switch (static_cast<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(packet.type))
    {
        case GP_CLI_COMMAND_FAQ_GMCALL_TYPE::AddHistory:
        {
            // Extra client information sent after acknowledging a response is not collected.
            return;
        }
        case GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMNotice:
        {
            // Unknown usage.
            return;
        }
        case GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall:
        {
            if (std::invoke(std::forward<AddPacket>(addPacket), packet))
            {
                std::invoke(std::forward<ProcessCall>(processCall));
                std::invoke(std::forward<IncrementHistory>(incrementHistory));
            }
            return;
        }
    }
}

template <typename Acknowledge>
void HandleAckGMMsg(const GP_CLI_COMMAND_ACK_GMMSG&, Acknowledge&& acknowledge)
{
    std::invoke(std::forward<Acknowledge>(acknowledge));
}

} // namespace gmcall::handler
