#include "test_mob_spell_indi_8880.h"

#include "map/mob_spell_container_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

auto runMobSpellIndi8880SelfTests() -> bool
{
    using mobspellhelpers::ResolveBestIndiSpell;

    const auto expect = [](const Maybe<SpellID>& got, const Maybe<SpellID>& want, const char* label) {
        if (got != want)
        {
            std::cerr << "mob spell Indi 8880 self-test failed: " << label << '\n';
            return false;
        }
        return true;
    };

    bool ok = true;
    ok      = expect(ResolveBestIndiSpell(JOB_WAR, true, false, 99), SpellID::Indi_Precision, "WAR accuracy") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WAR, false, false, 99), SpellID::Indi_Fury, "WAR attack") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_BLM, false, true, 99), SpellID::Indi_Focus, "BLM accuracy") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_BLM, false, false, 99), SpellID::Indi_Acumen, "BLM magic attack") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_GEO, false, false, 99), SpellID::Indi_Refresh, "GEO refresh") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_NIN, false, false, 99), SpellID::Indi_Haste, "NIN haste") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WAR, false, false, 19), std::nullopt, "below level 20") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WAR, false, false, 20), SpellID::Indi_Regen, "level 20 sub-choice") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WHM, false, false, 29), SpellID::Indi_Regen, "refresh job below 30") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WHM, false, false, 30), SpellID::Indi_Refresh, "refresh job level 30") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WHM, false, false, 92), SpellID::Indi_Refresh, "below level 93") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_WHM, false, false, 93), SpellID::Indi_Refresh, "level 93 primary") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_NON, false, false, 19), std::nullopt, "unknown job below level 20") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_NON, false, false, 20), SpellID::Indi_Regen, "unknown job level 20") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_NON, false, false, 92), SpellID::Indi_Regen, "unknown job level 92") && ok;
    ok      = expect(ResolveBestIndiSpell(JOB_NON, false, false, 93), std::nullopt, "unknown job level 93") && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("mob-spell-indi-8880", runMobSpellIndi8880SelfTests);
