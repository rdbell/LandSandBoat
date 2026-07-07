#pragma once

#include "common/cbasetypes.h"

struct SearchEntity;

auto SearchRankForNation(uint8 nation, uint8 rankSandoria, uint8 rankBastok, uint8 rankWindurst) -> uint8;
void NormalizeSearchPlayerForList(SearchEntity& player, uint32 settingsInt, uint32 partyId);
void NormalizeSearchPartyMemberForList(SearchEntity& player, uint32 settingsInt, uint32 partyId);
void NormalizeSearchLinkshellMemberForList(SearchEntity& player, uint32 settingsInt, uint32 partyId);
