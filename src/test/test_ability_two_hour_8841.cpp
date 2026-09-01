#include "test_ability_two_hour_8841.h"

#include "map/ability.h"
#include "omega_self_test_registry.h"

#include <iostream>
#include <utility>

auto runAbilityTwoHour8841SelfTests() -> bool
{
    const std::pair<JOBTYPE, uint16> cases[] = {
        { JOB_WAR, ABILITY_MIGHTY_STRIKES }, { JOB_MNK, ABILITY_HUNDRED_FISTS },
        { JOB_WHM, ABILITY_BENEDICTION },   { JOB_BLM, ABILITY_MANAFONT },
        { JOB_RDM, ABILITY_CHAINSPELL },    { JOB_THF, ABILITY_PERFECT_DODGE },
        { JOB_PLD, ABILITY_INVINCIBLE },    { JOB_DRK, ABILITY_BLOOD_WEAPON },
        { JOB_BST, ABILITY_FAMILIAR },      { JOB_BRD, ABILITY_SOUL_VOICE },
        { JOB_RNG, ABILITY_EAGLE_EYE_SHOT }, { JOB_SAM, ABILITY_MEIKYO_SHISUI },
        { JOB_NIN, ABILITY_MIJIN_GAKURE },  { JOB_DRG, ABILITY_SPIRIT_SURGE },
        { JOB_SMN, ABILITY_ASTRAL_FLOW },   { JOB_BLU, ABILITY_AZURE_LORE },
        { JOB_COR, ABILITY_WILD_CARD },     { JOB_PUP, ABILITY_OVERDRIVE },
        { JOB_DNC, ABILITY_TRANCE },        { JOB_SCH, ABILITY_TABULA_RASA },
        { JOB_GEO, ABILITY_BOLSTER },       { JOB_RUN, ABILITY_ELEMENTAL_SFORZO },
    };
    for (const auto& [job, id] : cases)
    {
        const auto* ability = ability::GetTwoHourAbility(job);
        if (ability == nullptr || ability->getID() != id)
        {
            std::cerr << "two-hour ability mismatch for job " << static_cast<int>(job) << '\n';
            return false;
        }
    }
    return ability::GetTwoHourAbility(JOB_NON) == nullptr;
}

OMEGA_REGISTER_SELF_TEST("ability-two-hour-8841", runAbilityTwoHour8841SelfTests);
