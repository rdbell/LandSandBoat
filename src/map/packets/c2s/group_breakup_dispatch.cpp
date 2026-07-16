#include "group_breakup_dispatch.h"

auto groupbreakup::Select(const Input input) -> Action
{
    if (!input.hasParty || !input.partyLeader)
        return Action::None;
    switch (input.kind)
    {
        case PartyKind::Party:
            return input.hasAlliance ? Action::None : Action::DisbandParty;
        case PartyKind::Alliance:
            return input.hasAlliance && input.allianceLeader ? Action::DissolveAlliance : Action::None;
        default:
            return Action::None;
    }
}
