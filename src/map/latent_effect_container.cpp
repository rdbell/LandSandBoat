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

#include "latent_effect_container.h"
#include "latent_capacity.h"
#include "latent_food_selection.h"
#include "latent_hp_selection.h"
#include "latent_job_level_selection.h"
#include "latent_mp_selection.h"
#include "latent_party_member_plan.h"
#include "latent_roll_song_selection.h"
#include "latent_status_selection.h"
#include "latent_target_selection.h"
#include "latent_time_selection.h"
#include "latent_tp_selection.h"
#include "latent_weapon_break_selection.h"
#include "latent_weapon_draw_plan.h"
#include "latent_weather_plan.h"
#include "latent_ws_plan.h"
#include "latent_zone_selection.h"

#include "ai/ai_container.h"
#include "conquest_system.h"
#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "entities/trust_entity.h"
#include "items/item_weapon.h"
#include "latent_effect.h"
#include "modifier.h"
#include "status_effect_container.h"
#include "utils/battleutils.h"
#include "utils/zoneutils.h"

#include "time_server.h"

CLatentEffectContainer::CLatentEffectContainer(CCharEntity* PEntity)
: m_POwner(PEntity)
{
}

/************************************************************************
 *                                                                       *
 * Adds new latent effect to the character.                              *
 *                                                                       *
 ************************************************************************/

void CLatentEffectContainer::AddLatentEffects(std::vector<CItemEquipment::itemLatent>& latentList, uint8 reqLvl, uint8 slot)
{
    for (auto& latent : latentList)
    {
        if (m_POwner->GetMLevel() >= reqLvl || latent.ConditionsValue == static_cast<uint16>(xi::Latent::JobLevelAbove))
        {
            m_LatentEffectList.emplace_back(m_POwner, latent.ConditionsID, latent.ConditionsValue, slot, latent.ModValue, latent.ModPower);
        }
    }
}

/************************************************************************
 *                                                                       *
 * Removes all latent effects associated with a specified slot           *
 *                                                                       *
 ************************************************************************/

void CLatentEffectContainer::DelLatentEffects(uint8 reqLvl, uint8 slot)
{
    m_LatentEffectList.erase(
        std::remove_if(
            m_LatentEffectList.begin(),
            m_LatentEffectList.end(),
            [slot](auto& latent)
            {
                return latent.GetSlot() == slot;
            }),
        m_LatentEffectList.end());
}

/************************************************************************
 *                                                                       *
 * Returns true if no latents for slot are inactive                      *
 *                                                                       *
 ************************************************************************/

bool CLatentEffectContainer::HasAllLatentsActive(uint8 slot)
{
    auto allActive = true;
    for (auto iter = m_LatentEffectList.begin(); iter != m_LatentEffectList.end(); ++iter)
    {
        CLatentEffect& latent = *iter;
        // Dual-wire pure slot-inactive gate (slice 2835). Production continues
        // scanning after finding inactive (does not early-return).
        if (latenthelpers::ShouldMarkNotAllActive(latent.IsActivated(), latent.GetSlot() == slot))
        {
            allActive = false;
        }
    }
    return allActive;
}

void CLatentEffectContainer::AddLatentEffect(xi::Latent conditionID, uint16 conditionValue, Mod modID, int16 modValue)
{
    m_LatentEffectList.emplace_back(m_POwner, conditionID, conditionValue, MAX_SLOTTYPE, modID, modValue);
}

auto CLatentEffectContainer::DelLatentEffect(xi::Latent conditionID, uint16 conditionValue, Mod modID, int16 modValue) -> bool
{
    // Find and remove the first instance of the latent matching the parameters
    for (auto iter = m_LatentEffectList.begin(); iter != m_LatentEffectList.end(); ++iter)
    {
        CLatentEffect& latent = *iter;
        if (latent.GetConditionsID() == conditionID && latent.GetConditionsValue() == conditionValue && latent.GetModValue() == modID &&
            latent.GetModPower() == modValue)
        {
            m_LatentEffectList.erase(iter);
            return true;
        }
    }
    return false;
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by HP and activates them if      *
 * the conditions are met.                                               *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsHP()
{
    // TODO: hook into this from anywhere HP changes
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessHpLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by TP and activates them if      *
 * the conditions are met.                                               *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsTP()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessTpLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are occur during WS and activates them        *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsWS(bool isDuringWs)
{
    ProcessLatentEffects(
        [this, isDuringWs](CLatentEffect& latentEffect)
        {
            switch (latenthelpers::DetermineDuringWsLatentAction(latentEffect.GetConditionsID(), isDuringWs))
            {
                case latenthelpers::DuringWsLatentAction::Activate:
                    return latentEffect.Activate();
                case latenthelpers::DuringWsLatentAction::Deactivate:
                    return latentEffect.Deactivate();
                case latenthelpers::DuringWsLatentAction::Ignore:
                default:
                    return false;
            }
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by MP and activates them if     *
 * the conditions are met.                                              *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsMP()
{
    // TODO: hook into this from anywhere MP changes
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessMpLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents for a given slot (ie. on equip)                   *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsEquip(uint8 slot)
{
    ProcessLatentEffects(
        [this, slot](CLatentEffect& latentEffect)
        {
            if (latentEffect.GetSlot() == slot)
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by drawn weapon and activates   *
 * them if the conditions are met.                                      *
 *                                                                       *
 ************************************************************************/

// easy: when animationType changes to ANIMATION_ATTACK or to something else
void CLatentEffectContainer::CheckLatentsWeaponDraw(bool drawn)
{
    ProcessLatentEffects(
        [this, drawn](CLatentEffect& latentEffect)
        {
            switch (latenthelpers::DetermineWeaponDrawLatentAction(
                latentEffect.GetConditionsID(), drawn, m_POwner->health.hp, m_POwner->health.mp, latentEffect.GetConditionsValue()))
            {
                case latenthelpers::WeaponDrawLatentAction::Activate:
                    return latentEffect.Activate();
                case latenthelpers::WeaponDrawLatentAction::Deactivate:
                    return latentEffect.Deactivate();
                case latenthelpers::WeaponDrawLatentAction::Ignore:
                default:
                    return false;
            }
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by status effects and activates *
 * them if the conditions are met.                                      *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsStatusEffect()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessStatusLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks latents that are affected by food effects. Usage:             *
 * LATENT_FOOD_ACTIVE: (49,foodItemId)                                  *
 * LATENT_NO_FOOD_ACTIVE: (14,0)                                        *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsFoodEffect()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessFoodLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by rolls or songs and activates  *
 * them if the conditions are met.                                       *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsRollSong()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessRollSongLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by day or moon and activates     *
 * them if the conditions are met.                                       *
 *                                                                       *
 ************************************************************************/

// probably call this at 00:00 vana time only
void CLatentEffectContainer::CheckLatentsDay()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessTimeLatent(latenthelpers::LatentTimeRefresh::Day, latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }

            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks latents affected by the moon phase and activates them          *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsMoonPhase()
{
    TracyZoneScoped;

    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessTimeLatent(latenthelpers::LatentTimeRefresh::MoonPhase, latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }

            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks latents that are affected by the day of the week and           *
 * activates them if the conditions are met.                             *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsWeekDay()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessTimeLatent(latenthelpers::LatentTimeRefresh::WeekDay, latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks latents that are affected the hour and activates them          *
 * if the conditions are met.                                            *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsHours()
{
    TracyZoneScoped;

    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessTimeLatent(latenthelpers::LatentTimeRefresh::Hour, latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }

            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by party members and             *
 * activates them if the conditions are met.                             *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsPartyMembers(size_t members, size_t trustCount)
{
    ProcessLatentEffects(
        [this, members, trustCount](CLatentEffect& latentEffect)
        {
            size_t totalMembers = members + trustCount;
            auto   inZone       = 0;

            if (latentEffect.GetConditionsID() == xi::Latent::PartyMembersInZone && latentEffect.GetConditionsValue() <= totalMembers)
            {
                for (size_t m = 0; m < members; ++m)
                {
                    auto* PMember = dynamic_cast<CCharEntity*>(m_POwner->PParty->members.at(m));
                    if (PMember != nullptr && PMember->getZone() == m_POwner->getZone())
                    {
                        inZone++;
                    }
                }

                auto* PLeader = dynamic_cast<CCharEntity*>(m_POwner->PParty->GetLeader());
                if (PLeader != nullptr && m_POwner->getZone() == PLeader->getZone())
                {
                    inZone = inZone + static_cast<int>(trustCount);
                }
            }

            switch (latenthelpers::DeterminePartyMemberLatentAction(latentEffect.GetConditionsID(), latentEffect.GetConditionsValue(), totalMembers, inZone))
            {
                case latenthelpers::PartyMemberLatentAction::Activate:
                    return latentEffect.Activate();
                case latenthelpers::PartyMemberLatentAction::Deactivate:
                    return latentEffect.Deactivate();
                case latenthelpers::PartyMemberLatentAction::Ignore:
                default:
                    return false;
            }
        });
}

void CLatentEffectContainer::CheckLatentsPartyJobs()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latentEffect.GetConditionsID() == xi::Latent::JobInParty)
            {
                return ProcessLatentEffect(latentEffect);
            }

            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by an avatar in party and        *
 * activates them if the conditions are met.                             *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsPartyAvatar()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latentEffect.GetConditionsID() == xi::Latent::AvatarInParty)
            {
                return ProcessLatentEffect(latentEffect);
            }

            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by job level and                 *
 * activates them if the conditions are met.                             *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsJobLevel()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessJobLevelLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by players pet type and          *
 * activates them if the conditions are met.                             *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsPetType()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latentEffect.GetConditionsID() == xi::Latent::PetId)
            {
                return ProcessLatentEffect(latentEffect);
            }

            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by time of vana day and          *
 * activates them if the conditions are met.                             *
 *                                                                       *
 ************************************************************************/

// will probably only call this at transition points in the day
void CLatentEffectContainer::CheckLatentsTime()
{
    // todo: this isn't called anywhere
}

/************************************************************************
 *                                                                       *
 * Checks all latents that are affected by weapon skill points           *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsWeaponBreak(uint8 slot)
{
    ProcessLatentEffects(
        [this, slot](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessWeaponBreakLatent(latentEffect.GetConditionsID(), latentEffect.GetConditionsValue(), slot))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents regarding current zone                             *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsZone()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessZoneLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

/************************************************************************
 *                                                                       *
 * Checks all latents regarding current weather                          *
 *                                                                       *
 ************************************************************************/
void CLatentEffectContainer::CheckLatentsWeather()
{
    uint16 zoneId = m_POwner->getZone();
    CZone* PZone  = zoneutils::GetZone(zoneId);

    if (PZone == nullptr)
    {
        ShowWarning("PZone was null for Zone ID %d.", zoneId);
        return;
    }

    CheckLatentsWeather(PZone->weather().current());
}

void CLatentEffectContainer::CheckLatentsWeather(Weather weather)
{
    ProcessLatentEffects(
        [this, weather](CLatentEffect& latent)
        {
            const auto applyAction = [&latent](const latenthelpers::WeatherLatentAction action) {
                switch (action)
                {
                    case latenthelpers::WeatherLatentAction::Activate:
                        return latent.Activate();
                    case latenthelpers::WeatherLatentAction::Deactivate:
                        return latent.Deactivate();
                    case latenthelpers::WeatherLatentAction::Ignore:
                    default:
                        return false;
                }
            };

            if (latent.GetConditionsID() == xi::Latent::WeatherElement)
            {
                const auto resolvedWeather = battleutils::GetWeather((CBattleEntity*)m_POwner, false, weather);
                const auto element         = zoneutils::GetWeatherElement(resolvedWeather);
                return applyAction(latenthelpers::DetermineWeatherLatentAction(
                    latent.GetConditionsID(), latent.GetConditionsValue(), static_cast<uint16>(resolvedWeather), static_cast<uint16>(element)));
            }
            else if (latent.GetConditionsID() == xi::Latent::WeatherCondition)
            {
                const auto resolvedWeather = battleutils::GetWeather((CBattleEntity*)m_POwner, false, weather);
                return applyAction(latenthelpers::DetermineWeatherLatentAction(
                    latent.GetConditionsID(), latent.GetConditionsValue(), static_cast<uint16>(resolvedWeather), 0));
            }
            return false;
        });
}

void CLatentEffectContainer::CheckLatentsTargetChange()
{
    ProcessLatentEffects(
        [this](CLatentEffect& latentEffect)
        {
            if (latenthelpers::ShouldProcessTargetLatent(latentEffect.GetConditionsID()))
            {
                return ProcessLatentEffect(latentEffect);
            }
            return false;
        });
}

// Process the latent effects container and apply a logic function responsible for
// filtering the appropriate latents to be activated/deactivated and finally update
// health post looping if at least one logic function returned true
void CLatentEffectContainer::ProcessLatentEffects(const std::function<bool(CLatentEffect&)>& logic)
{
    auto update = false;

    for (auto& latent : m_LatentEffectList)
    {
        if (logic(latent))
        {
            update = true;
        }
    }

    if (latenthelpers::ProcessLatentListWantsHealthUpdate(update))
    {
        m_POwner->UpdateHealth();
    }
}

// Processes a single CLatentEffect* and finds the expression to evaluate for
// activation/deactivation and attempts to apply
auto CLatentEffectContainer::ProcessLatentEffect(CLatentEffect& latentEffect, bool isDuringWs) -> bool
{
    TracyZoneScoped;

    // Our default case un-finds our latent prevent us from toggling a latent we don't have programmed
    auto expression  = false;
    auto latentFound = true;

    const uint16 playerZoneID = m_POwner != nullptr ? m_POwner->getZone() : 0;
    if (latenthelpers::ShouldRejectProcessLatent(m_POwner == nullptr, playerZoneID == 0))
    {
        return false;
    }

    vanadiel_time::time_point vanaTime = vanadiel_time::now();

    // find the latent type from the enum and find the expression to tests againts
    switch (latentEffect.GetConditionsID())
    {
        case xi::Latent::HpUnderPercent:
            expression = latenthelpers::EvaluateHpUnderPercent(
                m_POwner->health.hp, m_POwner->health.maxhp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::HpOverPercent:
            expression = latenthelpers::EvaluateHpOverPercent(
                m_POwner->health.hp, m_POwner->health.maxhp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::HpUnderTpUnder100:
            expression = latenthelpers::EvaluateHpUnderTpUnder100(
                m_POwner->health.hp, m_POwner->health.maxhp, m_POwner->health.tp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::HpOverTpUnder100:
            expression = latenthelpers::EvaluateHpOverTpUnder100(
                m_POwner->health.hp, m_POwner->health.maxhp, m_POwner->health.tp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::MpUnderPercent:
            expression = latenthelpers::EvaluateMpUnderPercent(
                m_POwner->health.mp, m_POwner->health.maxmp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::MpUnder:
            expression = latenthelpers::EvaluateMpUnder(m_POwner->health.mp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::TpUnder:
            expression = latenthelpers::EvaluateTpUnder(m_POwner->health.tp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::TpOver:
            expression = latenthelpers::EvaluateTpOver(m_POwner->health.tp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::Subjob:
            expression = latenthelpers::EvaluateSubjob(m_POwner->GetSJob(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::PetId:
            expression = latenthelpers::EvaluatePetID(
                m_POwner->PPet != nullptr,
                m_POwner->PPet != nullptr && m_POwner->PPet->objtype == TYPE_PET,
                m_POwner->PPet != nullptr && m_POwner->PPet->objtype == TYPE_PET
                    ? static_cast<CPetEntity*>(m_POwner->PPet)->petID()
                    : 0,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::WeaponDrawn:
            expression = latenthelpers::EvaluateWeaponDrawn(m_POwner->animation == ANIMATION_ATTACK);
            break;
        case xi::Latent::WeaponSheathed:
            expression = latenthelpers::EvaluateWeaponSheathed(m_POwner->animation == ANIMATION_ATTACK);
            break;
        case xi::Latent::SignetBonus:
        {
            CBattleEntity* PTarget = m_POwner->GetBattleTarget();
            expression             = latenthelpers::EvaluateSignetBonus(
                PTarget != nullptr,
                PTarget != nullptr && m_POwner->GetMLevel() >= PTarget->GetMLevel(),
                m_POwner->loc.zone != nullptr && m_POwner->loc.zone->GetRegionID() < REGION_TYPE::WEST_AHT_URHGAN);
            break;
        }
        case xi::Latent::SanctionRegenBonus:
            expression = latenthelpers::EvaluateSanctionRegionHPUnder(
                m_POwner->loc.zone != nullptr &&
                    m_POwner->loc.zone->GetRegionID() >= REGION_TYPE::WEST_AHT_URHGAN &&
                    m_POwner->loc.zone->GetRegionID() <= REGION_TYPE::ALZADAAL,
                m_POwner->health.hp,
                m_POwner->health.maxhp,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::SanctionRefreshBonus:
            expression = latenthelpers::EvaluateSanctionRegionMPUnder(
                m_POwner->loc.zone != nullptr &&
                    m_POwner->loc.zone->GetRegionID() >= REGION_TYPE::WEST_AHT_URHGAN &&
                    m_POwner->loc.zone->GetRegionID() <= REGION_TYPE::ALZADAAL,
                m_POwner->health.mp,
                m_POwner->health.maxmp,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::SigilRegenBonus:
            expression = latenthelpers::EvaluateSigilRegionHPUnder(
                m_POwner->loc.zone != nullptr &&
                    m_POwner->loc.zone->GetRegionID() >= REGION_TYPE::RONFAURE_FRONT &&
                    m_POwner->loc.zone->GetRegionID() <= REGION_TYPE::VALDEAUNIA_FRONT,
                m_POwner->health.hp,
                m_POwner->health.maxhp,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::SigilRefreshBonus:
            expression = latenthelpers::EvaluateSigilRegionMPUnder(
                m_POwner->loc.zone != nullptr &&
                    m_POwner->loc.zone->GetRegionID() >= REGION_TYPE::RONFAURE_FRONT &&
                    m_POwner->loc.zone->GetRegionID() <= REGION_TYPE::VALDEAUNIA_FRONT,
                m_POwner->health.mp,
                m_POwner->health.maxmp,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::StatusEffectActive:
            expression = latenthelpers::EvaluateStatusEffectActive(
                m_POwner->StatusEffectContainer->HasStatusEffect(static_cast<xi::StatusEffect>(latentEffect.GetConditionsValue())));
            break;
        case xi::Latent::NoFoodActive:
            expression = latenthelpers::EvaluateNoFoodActive(
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Food));
            break;
        case xi::Latent::PartyMembers:
        {
            size_t partyCount = 0;
            size_t trustCount = 0;
            auto*  PParty     = m_POwner->PParty;
            auto*  PLeader    = PParty ? dynamic_cast<CCharEntity*>(PParty->GetLeader()) : nullptr;
            if (PLeader)
            {
                trustCount = PLeader->PTrusts.size();
                partyCount = PParty->members.size();
            }

            expression = latenthelpers::EvaluatePartyMembers(latentEffect.GetConditionsValue(), partyCount, trustCount);
            break;
        }
        case xi::Latent::PartyMembersInZone:
        {
            auto inZone = 0;
            if (m_POwner->PParty && dynamic_cast<CCharEntity*>(m_POwner->PParty->GetLeader()))
            {
                for (auto* member : m_POwner->PParty->members)
                {
                    if (member->getZone() == m_POwner->getZone())
                    {
                        ++inZone;
                    }
                }

                auto PLeader = (CCharEntity*)m_POwner->PParty->GetLeader();
                if (m_POwner->getZone() == PLeader->getZone())
                {
                    inZone = inZone + static_cast<int>(PLeader->PTrusts.size());
                }
            }

            expression = latenthelpers::EvaluatePartyMembersInZone(latentEffect.GetConditionsValue(), inZone);
            break;
        }
        case xi::Latent::AvatarInParty:
            if (m_POwner->PParty != nullptr)
            {
                for (auto* member : m_POwner->PParty->members)
                {
                    if (member->PPet != nullptr && member->PPet->objtype == TYPE_PET)
                    {
                        auto* PPet = static_cast<CPetEntity*>(member->PPet);
                        if (latenthelpers::EvaluateAvatarMatch(
                                latenthelpers::IsLiveAvatar(PPet->isDead(), PPet->petID()),
                                PPet->petID(),
                                latentEffect.GetConditionsValue()))
                        {
                            expression = true;
                            break;
                        }
                    }
                }
            }
            else if (m_POwner->PParty == nullptr && m_POwner->PPet != nullptr)
            {
                auto* PPet = (CPetEntity*)m_POwner->PPet;
                expression = latenthelpers::EvaluateAvatarMatch(
                    latenthelpers::IsLiveAvatar(PPet->isDead(), PPet->petID()),
                    PPet->petID(),
                    latentEffect.GetConditionsValue());
            }
            break;
        case xi::Latent::JobInParty:
            if (m_POwner->PParty != nullptr)
            {
                for (auto* member : m_POwner->PParty->members)
                {
                    if (latenthelpers::EvaluateJobInPartyMember(
                            member->id != m_POwner->id,
                            member->GetMJob() == latentEffect.GetConditionsValue()))
                    {
                        expression = true;
                        break;
                    }
                }

                auto leader = (CCharEntity*)m_POwner->PParty->GetLeader();

                if (leader == nullptr)
                {
                    expression = false;
                    break;
                }

                for (auto* trust : leader->PTrusts)
                {
                    if (trust->GetMJob() == latentEffect.GetConditionsValue())
                    {
                        expression = true;
                        break;
                    }
                }
            }
            break;
        case xi::Latent::Zone:
            expression = latenthelpers::EvaluateZone(m_POwner->getZone(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::SynthTrainee:
        {
            const bool anyCraftImagery =
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::FishingImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::WoodworkingImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::SmithingImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::GoldsmithingImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::ClothcraftImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::LeathercraftImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::BonecraftImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AlchemyImagery) ||
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::CookingImagery);
            expression = latenthelpers::EvaluateSynthTrainee(
                static_cast<uint16>(m_POwner->RealSkills.skill[latentEffect.GetConditionsValue()]),
                anyCraftImagery);
            break;
        }
        case xi::Latent::SongRollActive:
            expression = latenthelpers::EvaluateSongRollActive(
                m_POwner->StatusEffectContainer->HasStatusEffectByFlag(xi::StatusEffectFlag::Roll | xi::StatusEffectFlag::Song));
            break;
        case xi::Latent::TimeOfDay:
        {
            const uint32 VanadielHour = vanadiel_time::get_hour(vanaTime);
            expression                = latenthelpers::EvaluateTimeOfDay(VanadielHour, latentEffect.GetConditionsValue());
            break;
        }
        case xi::Latent::HourOfDay:
        {
            const uint32 VanadielHour = vanadiel_time::get_hour(vanaTime);
            expression                = latenthelpers::EvaluateHourOfDay(VanadielHour, latentEffect.GetConditionsValue());
            break;
        }
        case xi::Latent::Firesday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), FIRESDAY);
            break;
        case xi::Latent::Earthsday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), EARTHSDAY);
            break;
        case xi::Latent::Watersday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), WATERSDAY);
            break;
        case xi::Latent::Windsday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), WINDSDAY);
            break;
        case xi::Latent::Darksday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), DARKSDAY);
            break;
        case xi::Latent::Iceday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), ICEDAY);
            break;
        case xi::Latent::Lightningsday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), LIGHTNINGDAY);
            break;
        case xi::Latent::Lightsday:
            expression = latenthelpers::EvaluateWeekdayMatch(vanadiel_time::get_weekday(vanaTime), LIGHTSDAY);
            break;
        case xi::Latent::MoonPhase:
        {
            const uint32 MoonPhase     = vanadiel_time::moon::get_phase(vanaTime);
            const uint32 MoonDirection = vanadiel_time::moon::get_direction(vanaTime); // 1 = waning, 2 = waxing, 0 = neither
            expression                 = latenthelpers::EvaluateMoonPhase(MoonPhase, MoonDirection, latentEffect.GetConditionsValue());
            break;
        }
        case xi::Latent::JobMultiple:
            expression = latenthelpers::EvaluateJobMultiple(m_POwner->GetMLevel(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::JobMultipleAtNight:
            expression = latenthelpers::EvaluateJobMultipleAtNight(
                m_POwner->GetMLevel(),
                latentEffect.GetConditionsValue(),
                vanadiel_time::get_totd(vanaTime) == vanadiel_time::TOTD::NIGHT);
            break;
        case xi::Latent::WeaponDrawnHpUnder:
            expression = latenthelpers::EvaluateWeaponDrawnHpAbsolute(
                m_POwner->animation == ANIMATION_ATTACK,
                m_POwner->health.hp,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::MpUnderVisibleGear:
            // TODO: figure out if this is actually right
            // CItemEquipment* head = (CItemEquipment*)(m_POwner->getEquip(SLOT_HEAD));
            // CItemEquipment* body = (CItemEquipment*)(m_POwner->getEquip(SLOT_BODY));
            // CItemEquipment* hands = (CItemEquipment*)(m_POwner->getEquip(SLOT_HANDS));
            // CItemEquipment* legs = (CItemEquipment*)(m_POwner->getEquip(SLOT_LEGS));
            // CItemEquipment* feet = (CItemEquipment*)(m_POwner->getEquip(SLOT_FEET));

            // int32 visibleMp = 0;
            // visibleMp += (head ? head->getModifier(Mod::MP) : 0);
            // visibleMp += (body ? body->getModifier(Mod::MP) : 0);
            // visibleMp += (hands ? hands->getModifier(Mod::MP) : 0);
            // visibleMp += (legs ? legs->getModifier(Mod::MP) : 0);
            // visibleMp += (feet ? feet->getModifier(Mod::MP) : 0);

            // TODO: add mp percent too
            // if ((float)( mp / ((m_POwner->health.mp - m_POwner->health.modmp) + (m_POwner->PMeritPoints->GetMerit(MERIT_MAX_MP)->count * 10 ) +
            //    visibleMp) ) <= m_LatentEffectList.at(i)->GetConditionsValue())
            //{
            //    m_LatentEffectList.at(i)->Activate();
            //}
            // else
            //{
            //    m_LatentEffectList.at(i)->Deactivate();
            //}
            break;
        case xi::Latent::HpOverVisibleGear:
            // TODO: figure out if this is actually right
            // CItemEquipment* head = (CItemEquipment*)(m_POwner->getEquip(SLOT_HEAD));
            // CItemEquipment* body = (CItemEquipment*)(m_POwner->getEquip(SLOT_BODY));
            // CItemEquipment* hands = (CItemEquipment*)(m_POwner->getEquip(SLOT_HANDS));
            // CItemEquipment* legs = (CItemEquipment*)(m_POwner->getEquip(SLOT_LEGS));
            // CItemEquipment* feet = (CItemEquipment*)(m_POwner->getEquip(SLOT_FEET));

            // int32 visibleHp = 0;
            // visibleHp += (head ? head->getModifier(Mod::HP) : 0);
            // visibleHp += (body ? body->getModifier(Mod::HP) : 0);
            // visibleHp += (hands ? hands->getModifier(Mod::HP) : 0);
            // visibleHp += (legs ? legs->getModifier(Mod::HP) : 0);
            // visibleHp += (feet ? feet->getModifier(Mod::HP) : 0);

            // TODO: add mp percent too
            // if ((float)( hp / ((m_POwner->health.hp - m_POwner->health.modhp) + (m_POwner->PMeritPoints->GetMerit(MERIT_MAX_HP)->count * 10 ) +
            //    visibleHp) ) <= m_LatentEffectList.at(i)->GetConditionsValue())
            //{
            //    m_LatentEffectList.at(i)->Activate();
            //}
            // else
            //{
            //    m_LatentEffectList.at(i)->Deactivate();
            //}
            break;
        case xi::Latent::WeaponBroken:
        {
            auto  slot = latentEffect.GetSlot();
            auto* item = (CItemWeapon*)m_POwner->getEquip((SLOTTYPE)slot);
            switch (slot)
            {
                case SLOT_MAIN:
                case SLOT_SUB:
                case SLOT_RANGED:
                    expression = latenthelpers::EvaluateWeaponBroken(true, item != nullptr, item != nullptr && item->isUnlocked());
                    break;
            }
            break;
        }
        case xi::Latent::InDynamis:
            expression = latenthelpers::EvaluateInFlag(m_POwner->isInDynamis());
            break;
        case xi::Latent::InAssault:
            expression = latenthelpers::EvaluateInFlag(m_POwner->isInAssault());
            break;
        case xi::Latent::InAdoulin:
            expression = latenthelpers::EvaluateInFlag(m_POwner->isInAdoulin());
            break;
        case xi::Latent::InGarrison:
            expression = latenthelpers::EvaluateInGarrison(
                m_POwner->isInGarrison(), m_POwner->GetMLevel(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::FoodActive:
            expression = latenthelpers::EvaluateFoodActive(
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Food),
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Food) &&
                    m_POwner->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Food)->GetSourceTypeParam() ==
                        latentEffect.GetConditionsValue());
            break;
        case xi::Latent::JobLevelBelow:
            expression = latenthelpers::EvaluateJobLevelBelow(m_POwner->GetMLevel(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::JobLevelAbove:
            expression = latenthelpers::EvaluateJobLevelAbove(m_POwner->GetMLevel(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::WeatherCondition:
            expression = latenthelpers::EvaluateWeatherMatch(
                static_cast<uint16_t>(battleutils::GetWeather((CBattleEntity*)m_POwner, false)),
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::WeatherElement:
            expression = latenthelpers::EvaluateWeatherElementMatch(
                static_cast<uint16>(zoneutils::GetWeatherElement(battleutils::GetWeather((CBattleEntity*)m_POwner, false))),
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::NationControl:
        {
            // playerZoneId represents the player's destination if they're zoning.
            // Otherwise, it represents their current zone.
            auto region                   = zoneutils::GetCurrentRegion(playerZoneID);
            auto hasSignet                = m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Signet);
            auto hasSanction              = m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Sanction);
            auto hasSigil                 = m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Sigil);
            auto regionAlwaysOutOfControl = zoneutils::IsAlwaysOutOfNationControl(region);
            const bool inConquest         = region < REGION_TYPE::WEST_AHT_URHGAN;
            const bool hasAnySigil        = hasSignet || hasSanction || hasSigil;
            switch (latentEffect.GetConditionsValue())
            {
                case 0:
                    // under own nation's control
                    expression = latenthelpers::EvaluateNationControlUnder(
                        inConquest,
                        conquest::GetRegionOwner(region) == m_POwner->profile.nation,
                        hasAnySigil);
                    break;
                case 1:
                    // outside of own nation's control
                    expression = latenthelpers::EvaluateNationControlOutside(
                        inConquest,
                        regionAlwaysOutOfControl || m_POwner->profile.nation != conquest::GetRegionOwner(region),
                        hasAnySigil);
                    break;
            }
            break;
        }
        case xi::Latent::NationCitizen:
        {
            expression = latenthelpers::EvaluateNationCitizen(m_POwner->profile.nation, latentEffect.GetConditionsValue());
            break;
        }
        case xi::Latent::ZoneHomeNation:
        {
            auto nationRegion = static_cast<REGION_TYPE>(latentEffect.GetConditionsValue());
            auto region       = zoneutils::GetCurrentRegion(playerZoneID);

            switch (nationRegion)
            {
                case REGION_TYPE::SANDORIA:
                    expression = latenthelpers::EvaluateZoneHomeNation(m_POwner->profile.nation, 0, region == nationRegion);
                    break;
                case REGION_TYPE::BASTOK:
                    expression = latenthelpers::EvaluateZoneHomeNation(m_POwner->profile.nation, 1, region == nationRegion);
                    break;
                case REGION_TYPE::WINDURST:
                    expression = latenthelpers::EvaluateZoneHomeNation(m_POwner->profile.nation, 2, region == nationRegion);
                    break;
                default:
                    break;
            }
            break;
        }
        case xi::Latent::MpOver:
            expression = latenthelpers::EvaluateMpOver(m_POwner->health.mp, latentEffect.GetConditionsValue());
            break;
        case xi::Latent::WeaponDrawnMpOver:
            expression = latenthelpers::EvaluateWeaponDrawnMPOver(
                m_POwner->animation == ANIMATION_ATTACK,
                m_POwner->health.mp,
                latentEffect.GetConditionsValue());
            break;
        case xi::Latent::ElevenRollActive:
            expression = latenthelpers::EvaluateElevenRollActive(
                m_POwner->StatusEffectContainer->CheckForElevenRoll());
            break;
        case xi::Latent::VsEcosystem:
            if (CBattleEntity* PTarget = m_POwner->GetBattleTarget())
            {
                expression = latenthelpers::EvaluateVsTargetValue(
                    true, static_cast<uint16>(PTarget->m_EcoSystem), latentEffect.GetConditionsValue());
            }
            break;
        case xi::Latent::VsSpecies:
            if (CBattleEntity* PTarget = m_POwner->GetBattleTarget())
            {
                CMobEntity* PMob = dynamic_cast<CMobEntity*>(PTarget);
                if (PMob)
                {
                    expression = latenthelpers::EvaluateVsTargetValue(
                        true, PMob->m_Species, latentEffect.GetConditionsValue());
                }
            }
            break;
        case xi::Latent::VsFamily:
            if (CBattleEntity* PTarget = m_POwner->GetBattleTarget())
            {
                CMobEntity* PMob = dynamic_cast<CMobEntity*>(PTarget);
                if (PMob)
                {
                    expression = latenthelpers::EvaluateVsTargetValue(
                        true, PMob->m_Family, latentEffect.GetConditionsValue());
                }
            }
            break;
        case xi::Latent::Mainjob:
            expression = latenthelpers::EvaluateMainjob(m_POwner->GetMJob(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::EquippedInSlot:
            expression = latenthelpers::EvaluateEquippedInSlot(latentEffect.GetSlot(), latentEffect.GetConditionsValue());
            break;
        case xi::Latent::DuringWs:
            expression = latenthelpers::EvaluateDuringWS(isDuringWs);
            break;
        default:
            latentFound = false;
            ShowWarning("Latent ID %d unhandled in ProcessLatentEffect", static_cast<uint16>(latentEffect.GetConditionsID()));
            break;
    }

    // if we did not hit the default case, attempt to apply the latent effect based on the expression
    if (latenthelpers::ShouldApplyLatentExpression(latentFound))
    {
        return ApplyLatentEffect(latentEffect, expression);
    }
    return false;
}

// Activates a latent effect if true otherwise deactivates the latent effect
bool CLatentEffectContainer::ApplyLatentEffect(CLatentEffect& effect, bool expression)
{
    if (latenthelpers::ApplyLatentWantsActivate(expression))
    {
        return effect.Activate();
    }
    else
    {
        return effect.Deactivate();
    }
}
