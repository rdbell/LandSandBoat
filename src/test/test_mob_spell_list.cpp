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

#include "test_mob_spell_list.h"

#include "map/mob_spell_list.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace
{

static_assert(MAX_MOBSPELLLIST_ID == 5000);
static_assert(std::is_same_v<std::underlying_type_t<SpellID>, uint16>);
static_assert(std::is_same_v<decltype(MobSpell_t{}.min_level), uint16>);
static_assert(std::is_same_v<decltype(MobSpell_t{}.max_level), uint16>);
static_assert(std::is_same_v<decltype(mobSpellList::MobSpellListRow{}.spellListId), uint16>);

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "mob spell-list self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testListStorageAndLookup() -> bool
{
    constexpr auto spellA = static_cast<SpellID>(100);
    constexpr auto spellB = static_cast<SpellID>(200);

    auto list = CMobSpellList(0x1234);
    list.AddSpell(spellA, 5, 20);
    list.AddSpell(spellB, 1, 99);
    list.AddSpell(spellA, 7, 40);
    list.AddSpell(static_cast<SpellID>(300), 50, 10); // Stored without range validation.

    bool ok = true;
    ok      = expect(list.getId() == 0x1234, "list ID") && ok;
    ok      = expect(list.m_spellList.size() == 4, "entry count including duplicate") && ok;
    ok      = expect(list.m_spellList[0].spellId == spellA && list.m_spellList[0].min_level == 5 && list.m_spellList[0].max_level == 20, "first entry") && ok;
    ok      = expect(list.m_spellList[1].spellId == spellB && list.m_spellList[1].min_level == 1 && list.m_spellList[1].max_level == 99, "second entry") && ok;
    ok      = expect(list.m_spellList[2].spellId == spellA && list.m_spellList[2].min_level == 7 && list.m_spellList[2].max_level == 40, "duplicate entry order") && ok;
    ok      = expect(list.m_spellList[3].min_level == 50 && list.m_spellList[3].max_level == 10, "unvalidated level range") && ok;
    ok      = expect(list.GetSpellMinLevel(spellA) == 5, "first-match minimum level") && ok;
    ok      = expect(list.GetSpellMinLevel(spellB) == 1, "second spell minimum level") && ok;
    ok      = expect(list.GetSpellMinLevel(static_cast<SpellID>(999)) == 255, "missing spell sentinel") && ok;
    return ok;
}

auto testRegistryRowLoadingAndBounds() -> bool
{
    constexpr auto spellA = static_cast<SpellID>(10);
    constexpr auto spellB = static_cast<SpellID>(20);

    auto registry = mobSpellList::CMobSpellListRegistry{};
    registry.EnsureEmptyList();

    bool ok = true;
    ok      = expect(registry.Get(0) != nullptr && registry.Get(0)->getId() == 0 && registry.Get(0)->m_spellList.empty(), "empty list zero") && ok;
    ok      = expect(registry.AddRow({ 42, spellA, 9, 30 }), "known row accepted") && ok;
    ok      = expect(registry.AddRow({ 7, spellB, 1, 2 }), "interleaved row accepted") && ok;
    ok      = expect(registry.AddRow({ 42, spellA, 11, 40 }), "duplicate row accepted") && ok;
    ok      = expect(registry.AddRow({ MAX_MOBSPELLLIST_ID - 1, spellB, 3, 4 }), "upper in-range list ID accepted") && ok;
    ok      = expect(!registry.AddRow({ MAX_MOBSPELLLIST_ID, spellA, 5, 6 }), "exclusive upper list ID rejected") && ok;
    ok      = expect(!registry.AddRow({ std::numeric_limits<uint16>::max(), spellA, 7, 8 }), "maximum uint16 list ID rejected") && ok;

    auto* list = registry.Get(42);
    ok         = expect(list != nullptr && list->getId() == 42, "known registry lookup") && ok;
    ok         = expect(list != nullptr && list->m_spellList.size() == 2, "registry duplicate count") && ok;
    ok         = expect(list != nullptr && list->m_spellList[0].min_level == 9 && list->m_spellList[1].min_level == 11, "registry row order") && ok;
    ok         = expect(registry.Get(MAX_MOBSPELLLIST_ID - 1) != nullptr, "upper in-range lookup") && ok;
    ok         = expect(registry.Get(41) == nullptr, "unknown registry lookup") && ok;
    ok         = expect(registry.Get(MAX_MOBSPELLLIST_ID) == nullptr, "rejected boundary lookup") && ok;

    registry.AddRow({ 0, spellA, 1, 1 });
    registry.EnsureEmptyList();
    ok = expect(registry.Get(0) != nullptr && registry.Get(0)->m_spellList.empty(), "reloading resets list zero") && ok;
    ok = expect(registry.Get(42) == list && registry.Get(42)->m_spellList.size() == 2, "reloading preserves nonzero lists") && ok;

    ok = expect(registry.AddRow({ 42, spellA, 13, 50 }), "second-batch duplicate accepted") && ok;
    ok = expect(registry.AddRow({ 42, spellB, 2, 60 }), "second-batch row accepted") && ok;
    ok = expect(registry.Get(42) == list, "second-batch append preserves nonzero pointer") && ok;
    ok = expect(list->m_spellList.size() == 4, "second-batch append count") && ok;
    ok = expect(list->m_spellList[2].spellId == spellA && list->m_spellList[2].min_level == 13, "second-batch duplicate order") && ok;
    ok = expect(list->m_spellList[3].spellId == spellB && list->m_spellList[3].min_level == 2, "second-batch row order") && ok;
    ok = expect(list->GetSpellMinLevel(spellA) == 9, "second-batch first-match lookup") && ok;
    return ok;
}

} // namespace

auto runMobSpellListSelfTests() -> bool
{
    bool ok = true;
    ok      = testListStorageAndLookup() && ok;
    ok      = testRegistryRowLoadingAndBounds() && ok;
    return ok;
}
