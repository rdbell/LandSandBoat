/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_ipc_dispatch_1248.h"

#include "common/ipc.h"

#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

namespace
{

class RecordingIPCHandler final : public ipc::IPCMessageHandlerBase<RecordingIPCHandler>
{
public:
    template <typename T>
    void record(const IPP& ipp, const T& message)
    {
        ++calls;
        name        = ipc::toStringV<T>;
        endpointRaw = ipp.getRawIPP();
        if constexpr (std::is_same_v<T, ipc::AccountLogin>)
        {
            accountId = message.accountId;
        }
    }

#define RECORD_IPC_HANDLER(name)                                                                                               \
    void handleMessage_##name(const IPP& ipp, const ipc::name& message)                                                        \
    {                                                                                                                          \
        record(ipp, message);                                                                                                  \
    }

    RECORD_IPC_HANDLER(EmptyStruct)
    RECORD_IPC_HANDLER(AccountLogin)
    RECORD_IPC_HANDLER(CharZone)
    RECORD_IPC_HANDLER(CharVarUpdate)
    RECORD_IPC_HANDLER(ChatMessageTell)
    RECORD_IPC_HANDLER(ChatMessageParty)
    RECORD_IPC_HANDLER(ChatMessageAlliance)
    RECORD_IPC_HANDLER(ChatMessageLinkshell)
    RECORD_IPC_HANDLER(ChatMessageUnity)
    RECORD_IPC_HANDLER(ChatMessageYell)
    RECORD_IPC_HANDLER(ChatMessageAssist)
    RECORD_IPC_HANDLER(ChatMessageServerMessage)
    RECORD_IPC_HANDLER(ChatMessageCustom)
    RECORD_IPC_HANDLER(PartyInvite)
    RECORD_IPC_HANDLER(PartyInviteResponse)
    RECORD_IPC_HANDLER(PartyReload)
    RECORD_IPC_HANDLER(PartyDisband)
    RECORD_IPC_HANDLER(AllianceReload)
    RECORD_IPC_HANDLER(AllianceDissolve)
    RECORD_IPC_HANDLER(PlayerKick)
    RECORD_IPC_HANDLER(MessageStandard)
    RECORD_IPC_HANDLER(MessageSystem)
    RECORD_IPC_HANDLER(LinkshellRankChange)
    RECORD_IPC_HANDLER(LinkshellRemove)
    RECORD_IPC_HANDLER(LinkshellSetMessage)
    RECORD_IPC_HANDLER(LuaFunction)
    RECORD_IPC_HANDLER(KillSession)
    RECORD_IPC_HANDLER(ConquestEvent)
    RECORD_IPC_HANDLER(BesiegedEvent)
    RECORD_IPC_HANDLER(CampaignEvent)
    RECORD_IPC_HANDLER(ColonizationEvent)
    RECORD_IPC_HANDLER(EntityInformationRequest)
    RECORD_IPC_HANDLER(EntityInformationResponse)
    RECORD_IPC_HANDLER(SendPlayerToLocation)
    RECORD_IPC_HANDLER(AssistChannelEvent)
    RECORD_IPC_HANDLER(GMCallRequest)
    RECORD_IPC_HANDLER(GMCallResponse)

#undef RECORD_IPC_HANDLER

    void handleUnknownMessage(const IPP& ipp, const std::span<uint8_t> message)
    {
        ++calls;
        name        = "Unknown";
        endpointRaw = ipp.getRawIPP();
        unknown.assign(message.begin(), message.end());
    }

    int                calls{};
    std::string        name{};
    uint64             endpointRaw{};
    uint32             accountId{};
    std::vector<uint8> unknown{};
};

auto expect(bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "IPC dispatch 1248 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testKnownMessage() -> bool
{
    bool ok = true;

    RecordingIPCHandler handler;
    const IPP           endpoint(0x0100007F, 54231);
    auto                frame = ipc::toBytesWithHeader(ipc::AccountLogin{ .accountId = 0x00345678 });

    handler.handleMessage(endpoint, frame);

    ok = expect(handler.calls == 1, "known handler call count") && ok;
    ok = expect(handler.name == "AccountLogin", "known handler name") && ok;
    ok = expect(handler.endpointRaw == endpoint.getRawIPP(), "known endpoint") && ok;
    ok = expect(handler.accountId == 0x00345678, "known decoded payload") && ok;
    ok = expect(handler.unknown.empty(), "known message does not use unknown handler") && ok;
    return ok;
}

auto testMalformedKnownMessage() -> bool
{
    RecordingIPCHandler   handler;
    const IPP             endpoint(0x0200007F, 54232);
    std::vector<uint8> frame{ static_cast<uint8>(ipc::MessageType::AccountLogin) };

    handler.handleMessage(endpoint, frame);

    return expect(handler.calls == 0, "malformed known message invokes no handler");
}

auto testUnknownMessage() -> bool
{
    bool ok = true;

    RecordingIPCHandler   handler;
    const IPP             endpoint(0x0300007F, 54233);
    std::vector<uint8> frame{ 0xFF, 0x10, 0x20 };

    handler.handleMessage(endpoint, frame);

    ok = expect(handler.calls == 1, "unknown handler call count") && ok;
    ok = expect(handler.name == "Unknown", "unknown handler name") && ok;
    ok = expect(handler.endpointRaw == endpoint.getRawIPP(), "unknown endpoint") && ok;
    ok = expect(handler.unknown == frame, "unknown raw frame") && ok;
    return ok;
}

} // namespace

auto runIPCDispatch1248SelfTests() -> bool
{
    return testKnownMessage() && testMalformedKnownMessage() && testUnknownMessage();
}
