#pragma once

#include "common/cbasetypes.h"

namespace mapnetworkingincominghelpers
{
struct Facts
{
    bool  hasSession;
    int32 decryptCount;
    bool  hasCharacter;
    bool  pendingZone;
    bool  parseReturnedNonZero;
    bool  shuttingDown;
};

struct Plan
{
    bool returnNoSession;
    bool callParse;
    bool callSendParse;
    bool rebuildZonePacket;
    bool send;
    bool storePacketCache;
    bool destroySession;

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(const Facts& facts) -> Plan
{
    if (!facts.hasSession)
    {
        return { true, false, false, false, false, false, false };
    }

    Plan plan{ false, false, false, false, false, false, facts.shuttingDown };
    if (facts.decryptCount == -1)
    {
        return plan;
    }

    plan.callParse         = facts.decryptCount == 0 && facts.hasCharacter;
    plan.callSendParse     = plan.callParse && !facts.parseReturnedNonZero;
    plan.rebuildZonePacket = facts.decryptCount == 1 && facts.pendingZone;
    plan.send              = true;
    plan.storePacketCache  = true;
    return plan;
}
} // namespace mapnetworkingincominghelpers
