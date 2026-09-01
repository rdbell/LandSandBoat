#pragma once

#include <cstddef>
#include <cstdint>

#include "common/mmo.h"
#include "entities/battle_entity.h"
#include "spell.h"

namespace mobspellhelpers
{

// ResolveStormDaySpell mirrors CMobSpellContainer::GetStormDay after the host
// resolves the current Vana'diel day element. GetBestAvailable remains
// host-owned and is injected so availability and spell-tier selection keep
// their production behavior.
template <typename SelectBestAvailable>
inline auto ResolveStormDaySpell(const std::size_t dayElement, SelectBestAvailable&& selectBestAvailable) -> Maybe<SpellID>
{
    switch (dayElement)
    {
        case 1: // ELEMENT_FIRE
            return selectBestAvailable(SPELLFAMILY_FIRESTORM);
        case 2: // ELEMENT_ICE
            return selectBestAvailable(SPELLFAMILY_HAILSTORM);
        case 3: // ELEMENT_WIND
            return selectBestAvailable(SPELLFAMILY_WINDSTORM);
        case 4: // ELEMENT_EARTH
            return selectBestAvailable(SPELLFAMILY_SANDSTORM);
        case 5: // ELEMENT_THUNDER
            return selectBestAvailable(SPELLFAMILY_THUNDERSTORM);
        case 6: // ELEMENT_WATER
            return selectBestAvailable(SPELLFAMILY_RAINSTORM);
        case 7: // ELEMENT_LIGHT
            return selectBestAvailable(SPELLFAMILY_AURORASTORM);
        case 8: // ELEMENT_DARK
            return selectBestAvailable(SPELLFAMILY_VOIDSTORM);
        default:
            return std::nullopt;
    }
}

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
