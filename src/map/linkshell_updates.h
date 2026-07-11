#pragma once

#include "common/ipc_structs.h"
#include "items/item_linkshell.h"

#include <functional>
#include <optional>
#include <utility>

namespace mapipc
{

template <typename Lookup>
void HandleLinkshellRankChange(const ipc::LinkshellRankChange& message, Lookup&& lookup)
{
    if (auto* linkshell = std::invoke(lookup, message.linkshellId))
    {
        linkshell->ChangeMemberRank(message.memberName, message.requesterRank, message.newRank);
    }
}

inline auto CanRemoveLinkshellMember(const uint8 requesterRank, const uint8 targetRank) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL ||
           (requesterRank == LSTYPE_PEARLSACK && targetRank == LSTYPE_LINKPEARL);
}

template <typename CharacterLookup, typename MembershipLookup, typename EquippedRankLookup>
void HandleLinkshellRemove(const ipc::LinkshellRemove& message, CharacterLookup&& characterLookup, MembershipLookup&& membershipLookup, EquippedRankLookup&& equippedRankLookup)
{
    auto* character = std::invoke(characterLookup, message.victimName);
    if (!character)
    {
        return;
    }

    auto* membership = std::invoke(membershipLookup, character, 1);
    int   slot       = 1;
    if (!membership || membership->getID() != message.linkshellId)
    {
        membership = std::invoke(membershipLookup, character, 2);
        slot       = 2;
    }

    if (!membership || membership->getID() != message.linkshellId)
    {
        return;
    }

    const auto targetRank = std::invoke(equippedRankLookup, character, slot);
    if (targetRank && CanRemoveLinkshellMember(message.requesterRank, *targetRank))
    {
        membership->RemoveMemberByName(message.victimName, message.requesterRank);
    }
}

template <typename Lookup, typename Deliver>
void HandleLinkshellSetMessage(const ipc::LinkshellSetMessage& message, Lookup&& lookup, Deliver&& deliver)
{
    if (auto* linkshell = std::invoke(lookup, message.linkshellId))
    {
        std::invoke(deliver, linkshell, message);
    }
}

} // namespace mapipc
