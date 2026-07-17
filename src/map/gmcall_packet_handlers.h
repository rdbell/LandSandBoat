#pragma once

#include "packets/c2s/0x0d3_faq_gmcall.h"
#include "packets/c2s/0x0d5_ack_gmmsg.h"

#include <functional>
#include <utility>

namespace gmcall::handler
{

// FAQGMCallDisposition is the pure GP_CLI_COMMAND_FAQ_GMCALL type branch.
// Host callbacks (AddPacket / ProcessCall / IncrementHistory) stay outside.
enum class FAQGMCallDisposition : uint8_t
{
    Ignore,        // AddHistory, GMNotice, or unknown type — no host work
    ProcessGMCall, // GMCall: try AddPacket; if success, ProcessCall + IncrementHistory
};

// PlanFAQGMCallType mirrors the FAQ_GMCALL process type switch:
// - AddHistory  -> Ignore (post-acknowledgement blocks are not collected)
// - GMNotice    -> Ignore (unknown usage)
// - GMCall      -> ProcessGMCall
// - other/unknown bit patterns -> Ignore (matches original fall-through no-op)
inline auto PlanFAQGMCallType(const GP_CLI_COMMAND_FAQ_GMCALL_TYPE packetType) -> FAQGMCallDisposition
{
    switch (packetType)
    {
        case GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall:
            return FAQGMCallDisposition::ProcessGMCall;
        case GP_CLI_COMMAND_FAQ_GMCALL_TYPE::AddHistory:
        case GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMNotice:
            return FAQGMCallDisposition::Ignore;
    }
    return FAQGMCallDisposition::Ignore;
}

template <typename AddPacket, typename ProcessCall, typename IncrementHistory>
void HandleFAQGMCall(const GP_CLI_COMMAND_FAQ_GMCALL& packet, AddPacket&& addPacket, ProcessCall&& processCall, IncrementHistory&& incrementHistory)
{
    switch (PlanFAQGMCallType(static_cast<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(packet.type)))
    {
        case FAQGMCallDisposition::Ignore:
            // AddHistory: extra client information after acknowledging a response is not collected.
            // GMNotice: unknown usage.
            return;
        case FAQGMCallDisposition::ProcessGMCall:
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
