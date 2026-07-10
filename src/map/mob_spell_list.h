/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

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

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "common/cbasetypes.h"

#include "spell.h"

#define MAX_MOBSPELLLIST_ID 5000

typedef struct
{
    SpellID spellId;
    uint16  min_level;
    uint16  max_level;
} MobSpell_t;

class CMobSpellList
{
public:
    CMobSpellList(uint16 listId);

    auto getId() const -> uint16;

    void AddSpell(SpellID spellId, uint16 minLvl, uint16 maxLvl);
    auto GetSpellMinLevel(SpellID spellId) const -> uint16;
    auto GetEligibleSpells(uint16 level) const -> std::vector<SpellID>;

    // main spell list
    std::vector<MobSpell_t> m_spellList;

private:
    uint16 m_listId{};
};

namespace mobSpellList
{

struct MobSpellListRow
{
    uint16  spellListId;
    SpellID spellId;
    uint16  minLevel;
    uint16  maxLevel;
};

// Pure registry seam shared by database loading and focused parity tests.
class CMobSpellListRegistry
{
public:
    void EnsureEmptyList();
    auto AddRow(const MobSpellListRow& row) -> bool;
    auto Get(uint16 mobSpellListId) const -> CMobSpellList*;

private:
    std::unordered_map<uint16, std::unique_ptr<CMobSpellList>> spellLists_;
};

void LoadMobSpellList();

auto GetMobSpellList(uint16 mobSpellListId) -> CMobSpellList*;

}; // namespace mobSpellList
