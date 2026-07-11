#pragma once

#include "common/ipc_structs.h"

#include <functional>
#include <utility>

namespace mapipc
{

template <typename ForEachMember, typename CharacterLookup>
void ReloadGroupMembers(const uint32 groupId, ForEachMember&& forEachMember, CharacterLookup&& characterLookup)
{
    std::invoke(
        forEachMember,
        groupId,
        [&](const uint32 characterId)
        {
            if (auto* player = std::invoke(characterLookup, characterId))
            {
                player->ReloadPartyInc();
            }
        });
}

template <typename ForEachMember, typename CharacterLookup>
void HandlePartyReload(const ipc::PartyReload& message, ForEachMember&& forEachMember, CharacterLookup&& characterLookup)
{
    ReloadGroupMembers(message.partyId, std::forward<ForEachMember>(forEachMember), std::forward<CharacterLookup>(characterLookup));
}

template <typename ForEachMember, typename CharacterLookup>
void HandleAllianceReload(const ipc::AllianceReload& message, ForEachMember&& forEachMember, CharacterLookup&& characterLookup)
{
    ReloadGroupMembers(message.allianceId, std::forward<ForEachMember>(forEachMember), std::forward<CharacterLookup>(characterLookup));
}

template <typename Lookup>
void HandlePartyDisband(const ipc::PartyDisband& message, Lookup&& lookup)
{
    if (auto* party = std::invoke(lookup, message.partyId))
    {
        party->DisbandParty(false);
    }
}

template <typename Lookup>
void HandleAllianceDissolve(const ipc::AllianceDissolve& message, Lookup&& lookup)
{
    if (auto* alliance = std::invoke(lookup, message.allianceId))
    {
        alliance->dissolveAlliance(false);
    }
}

} // namespace mapipc
