#pragma once
#include "map/enums/party_kind.h"

namespace groupbreakup
{

enum class Action
{
    None,
    DisbandParty,
    DissolveAlliance
};

struct Input
{
    PartyKind kind{};
    bool      hasParty{};
    bool      partyLeader{};
    bool      hasAlliance{};
    bool      allianceLeader{};
};

// Select returns the graph operation only when GROUP_BREAKUP's membership and leadership prerequisites hold.
auto Select(Input input) -> Action;

} // namespace groupbreakup
