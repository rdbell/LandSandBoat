#pragma once

#include "common/cbasetypes.h"

struct SearchEntity;

void NormalizeSearchPlayerForList(SearchEntity& player, uint32 settingsInt, uint32 partyId);
void NormalizeSearchPartyMemberForList(SearchEntity& player, uint32 settingsInt, uint32 partyId);
void NormalizeSearchLinkshellMemberForList(SearchEntity& player, uint32 settingsInt, uint32 partyId);
