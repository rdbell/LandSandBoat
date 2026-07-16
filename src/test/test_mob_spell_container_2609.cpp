#include "test_mob_spell_container_2609.h"

#include "map/mob_spell_container_membership.h"

#include <iostream>

auto runMobSpellContainer2609SelfTests() -> bool
{
    bool ok     = true;
    auto expect = [&](const bool value, const char* name)
    {
        if (!value)
        {
            std::cerr << "mob spell 2609 failed: " << name << '\n';
        }
        ok = value && ok;
    };

    mob_spell_category c{};
    expect(ClassifyMobSpellMembership({ true, true, true, false, false, false, false, false }, c) && c == mob_spell_category::GA,
           "aoe precedence");
    expect(ClassifyMobSpellMembership({ false, true, true, false, false, false, false, false }, c) && c == mob_spell_category::Severe,
           "severe precedence");
    expect(!ClassifyMobSpellMembership({}, c), "unclassified");

    mob_spell_membership m;
    expect(!m.add(1, {}, false) && !m.has(), "missing no op");
    expect(!m.add(1, {}, true) && m.has(), "unclassified sets has spells");
    m.clear();
    m.add(1, { false, false, false, false, false, false, false, true }, true);
    m.add(1, { false, false, false, false, false, false, false, true }, true);
    m.add(2, { false, false, true, false, false, false, false, false }, true);
    expect(m.list(mob_spell_category::Buff).size() == 2, "duplicates");

    m.remove(1);
    expect(m.list(mob_spell_category::Buff).empty() && m.list(mob_spell_category::Severe).size() == 1 && !m.has(), "severe remove quirk");

    m.clear();
    expect(m.list(mob_spell_category::Severe).empty() && !m.has(), "clear");
    return ok;
}
