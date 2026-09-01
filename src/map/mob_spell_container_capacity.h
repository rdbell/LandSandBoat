#pragma once

#include <cstdint>

#include "common/mmo.h"
#include "entities/battle_entity.h"
#include "spell.h"

namespace mobspellhelpers
{

// ResolveBestIndiSpell mirrors the final selection policy in
// CMobSpellContainer::GetBestIndiSpell. Hit-rate and entity modifier reads
// remain host-owned; callers provide the resulting accuracy decisions and
// main level.
inline auto ResolveBestIndiSpell(const std::uint8_t mainJob,
                                 const bool          accuracyBuffNeeded,
                                 const bool          magicAccuracyBuffNeeded,
                                 const std::uint8_t  mainLevel) -> Maybe<SpellID>
{
    Maybe<SpellID> choice    = std::nullopt;
    Maybe<SpellID> subChoice = SpellID::Indi_Regen;

    switch (static_cast<JOBTYPE>(mainJob))
    {
        case JOB_WAR:
        case JOB_MNK:
        case JOB_THF:
        case JOB_DRK:
        case JOB_BST:
        case JOB_RNG:
        case JOB_SAM:
        case JOB_DRG:
        case JOB_BLU:
        case JOB_COR:
        case JOB_PUP:
        case JOB_DNC:
            choice    = accuracyBuffNeeded ? SpellID::Indi_Precision : SpellID::Indi_Fury;
            subChoice = SpellID::Indi_Regen;
            break;
        case JOB_WHM:
        case JOB_BRD:
        case JOB_SMN:
        case JOB_GEO:
            choice    = SpellID::Indi_Refresh;
            subChoice = SpellID::Indi_Refresh;
            break;
        case JOB_BLM:
        case JOB_RDM:
        case JOB_SCH:
            choice    = magicAccuracyBuffNeeded ? SpellID::Indi_Focus : SpellID::Indi_Acumen;
            subChoice = SpellID::Indi_Refresh;
            break;
        case JOB_PLD:
        case JOB_RUN:
        case JOB_NIN:
            choice    = SpellID::Indi_Haste;
            subChoice = SpellID::Indi_Regen;
            break;
        default:
            break;
    }

    if (mainLevel < 20)
    {
        choice = std::nullopt;
    }
    else if (mainLevel < 93)
    {
        choice = subChoice;
        if (subChoice == SpellID::Indi_Refresh && mainLevel < 30)
        {
            choice = SpellID::Indi_Regen;
        }
    }

    return choice;
}

} // namespace mobspellhelpers
