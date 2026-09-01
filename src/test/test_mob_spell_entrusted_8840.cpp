#include "test_mob_spell_entrusted_8840.h"

#include "map/entities/battle_entity.h"
#include "map/mob_spell_container.h"
#include "map/spell.h"
#include "omega_self_test_registry.h"

#include <iostream>

auto runMobSpellEntrusted8840SelfTests() -> bool
{
    CMobSpellContainer container(nullptr);
    CBattleEntity       master;

    const auto expect = [&](const uint8 job, const Maybe<SpellID>& wanted) {
        master.SetMJob(job);
        return container.GetBestEntrustedSpell(&master) == wanted;
    };

    const bool ok =
        expect(JOB_WAR, SpellID::Indi_Frailty) &&
        expect(JOB_DNC, SpellID::Indi_Frailty) &&
        expect(JOB_WHM, SpellID::Indi_Acumen) &&
        expect(JOB_SMN, SpellID::Indi_Acumen) &&
        expect(JOB_BLM, SpellID::Indi_Refresh) &&
        expect(JOB_RUN, SpellID::Indi_Refresh) &&
        expect(JOB_NIN, SpellID::Indi_Regen) &&
        expect(JOB_GEO, std::nullopt) &&
        expect(0, std::nullopt);
    if (!ok)
    {
        std::cerr << "mob spell entrusted 8840 self-tests failed\n";
    }
    return ok;
}

OMEGA_REGISTER_SELF_TEST("mob-spell-entrusted-8840", runMobSpellEntrusted8840SelfTests);
