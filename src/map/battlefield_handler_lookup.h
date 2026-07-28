#pragma once

// Pure CBattlefieldHandler::GetBattlefield lookup selection.
namespace battlefieldhandlerhelpers
{

// ShouldSelectBattlefield chooses the membership relation used by
// GetBattlefield. Registered-player lookup only applies to player entities;
// every other lookup uses entered-entity membership.
inline auto ShouldSelectBattlefield(const bool checkRegistered, const bool entityIsPlayer,
                                   const bool isRegistered, const bool containsEntity) -> bool
{
    return checkRegistered && entityIsPlayer ? isRegistered : containsEntity;
}

} // namespace battlefieldhandlerhelpers
