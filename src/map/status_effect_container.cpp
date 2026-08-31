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

The StatusEffeectContainer manages status effects on battleentities.

When a status effect is gained twice on a player. It can do one or more of the following:

1 Overwrite if equal or higher (protect)
2 Overwrite if higher (blind)
3 Can only have one of type (physical shield, magic shield)
4 Overwrite if equal or stronger than negative (defense boost, defense down)

*/

#include "common/logging.h"
#include "common/timer.h"

#include <array>
#include <cstring>

#include "data/loader.h"
#include "lua/luautils.h"

#include "ai/ai_container.h"
#include "ai/states/inactive_state.h"
#include "ai/states/mobskill_state.h"

#include "enmity_container.h"
#include "entities/automaton_entity.h"
#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "entities/mob_entity.h"
#include "entities/trust_entity.h"
#include "latent_effect_container.h"
#include "notoriety_container.h"
#include "status_effect_container.h"
#include "status_effect_capacity.h"

#include "enums/msg_std.h"
#include "map_engine.h"
#include "packets/s2c/0x029_battle_message.h"
#include "utils/battleutils.h"
#include "utils/charutils.h"
#include "utils/itemutils.h"
#include "utils/petutils.h"
#include "utils/puppetutils.h"

namespace effects
{

// Default effect of statuses are overwrite if equal or higher
struct EffectParams_t
{
    xi::StatusEffectFlag Flag{ xi::StatusEffectFlag::None };
    std::string          Name{};
    uint16               Type{ 0 };                                     // type will erase all other effects that match. Examples: En- spells, Spikes.
    xi::StatusEffect     NegativeId{ 0 };                               // Negative means the new effect can only land if the negative id is weaker. Example: Haste, Slow
    xi::EffectOverwrite  Overwrite{ xi::EffectOverwrite::EqualHigher }; // only overwrite its self if the new effect is equal or higher / higher than current. Example: Protect, Blind
    xi::StatusEffect     BlockId{ 0 };                                  // If this status effect is on the user, it will not take effect. Example: lullaby will not take effect with sleep I
    xi::StatusEffect     RemoveId{ 0 };                                 // Will always remove this effect when landing
    uint8                Element{ 0 };                                  // status effect element, used in resistances
    timer::duration      MinDuration{ 0s };                             // minimum duration. IE: stun cannot last less than 1 second
    uint16               SortKey{ 0 };                                  // Order in which the status effect should be displayed for the player
    MsgStd               WearOffMessageId{ MsgStd::EffectWearsOff };    // Message ID for when effect wears off
};

std::array<EffectParams_t, MAX_EFFECTID> EffectsParams;

void LoadEffectsParameters()
{
    for (uint16 i = 0; i < MAX_EFFECTID; ++i)
    {
        EffectsParams[static_cast<uint16>(i)].Flag = xi::StatusEffectFlag::None;
    }

    for (const auto& [id, data] : LoadStatusEffects())
    {
        if (id >= MAX_EFFECTID)
        {
            continue;
        }

        EffectsParams[static_cast<uint16>(id)].Name             = data.Name;
        EffectsParams[static_cast<uint16>(id)].Flag             = data.Flags;
        EffectsParams[static_cast<uint16>(id)].Type             = static_cast<uint16>(data.ExclusionGroup);
        EffectsParams[static_cast<uint16>(id)].NegativeId       = static_cast<xi::StatusEffect>(data.Negative);
        EffectsParams[static_cast<uint16>(id)].Overwrite        = data.Overwrite;
        EffectsParams[static_cast<uint16>(id)].BlockId          = static_cast<xi::StatusEffect>(data.Block);
        EffectsParams[static_cast<uint16>(id)].RemoveId         = static_cast<xi::StatusEffect>(data.Remove);
        EffectsParams[static_cast<uint16>(id)].Element          = static_cast<uint8>(data.Element);
        EffectsParams[static_cast<uint16>(id)].MinDuration      = std::chrono::seconds(data.MinDuration);
        EffectsParams[static_cast<uint16>(id)].SortKey          = data.SortKey == 0 ? 10000 : data.SortKey;
        EffectsParams[static_cast<uint16>(id)].WearOffMessageId = data.WearOffMessageId == 0 ? MsgStd::EffectWearsOff : static_cast<MsgStd>(data.WearOffMessageId);

        auto filename = fmt::format("./scripts/effects/{}.lua", EffectsParams[static_cast<uint16>(id)].Name);
        luautils::LoadLuaObjectFromFile(filename);
    }
}

// hacky way to get element from status effect
uint16 GetEffectElement(uint16 effect)
{
    return EffectsParams[static_cast<uint16>(effect)].Element;
}

std::string GetEffectName(uint16 effect)
{
    return EffectsParams[static_cast<uint16>(effect)].Name;
}

} // namespace effects

bool isSortedByStartTime(uint16 effectId)
{
    return static_cast<uint16>(effectId) >= static_cast<uint16>(xi::StatusEffect::FireManeuver) && static_cast<uint16>(effectId) <= static_cast<uint16>(xi::StatusEffect::DarkManeuver);
}

bool statusOrdering(const std::unique_ptr<CStatusEffect>& AStatus, const std::unique_ptr<CStatusEffect>& BStatus)
{
    // Sort by overall status effect ordering, if they have different sort keys
    uint16 ASortKey = effects::EffectsParams[static_cast<uint16>(AStatus->GetStatusID())].SortKey;
    uint16 BSortKey = effects::EffectsParams[static_cast<uint16>(BStatus->GetStatusID())].SortKey;
    if (ASortKey != BSortKey)
    {
        return ASortKey < BSortKey;
    }

    // Sort by song/roll slot
    if (AStatus->GetEffectSlot() != BStatus->GetEffectSlot())
    {
        return AStatus->GetEffectSlot() < BStatus->GetEffectSlot();
    }

    // Sort by start time
    if (isSortedByStartTime(static_cast<uint16>(AStatus->GetStatusID())) && isSortedByStartTime(static_cast<uint16>(BStatus->GetStatusID())))
    {
        auto diff = timer::count_milliseconds(AStatus->GetStartTime() - BStatus->GetStartTime());
        if (diff != 0)
        {
            return diff > 0;
        }
    }

    // Fall-back to sort by status effect ID, in case no other ordering is applied
    return AStatus->GetStatusID() < BStatus->GetStatusID();
}

CStatusEffectContainer::CStatusEffectContainer(CBattleEntity* PEntity)
: m_StatusEffectSet(statusOrdering)
{
    m_POwner = PEntity;

    if (m_POwner == nullptr)
    {
        ShowWarning("m_POwner was null.");
        return;
    }

    std::memset(m_StatusIcons, 0xFF, sizeof(m_StatusIcons));
}

CStatusEffectContainer::~CStatusEffectContainer()
{
    // The owned unique_ptrs in m_StatusEffectSet free their effects automatically.
}

auto CStatusEffectContainer::GetEffectsCount(xi::StatusEffect ID) -> uint8
{
    uint8 count = 0;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == ID && !PStatusEffect->isDeleted())
        {
            count++;
        }
    }
    return count;
}

auto CStatusEffectContainer::GetEffectsCountWithFlag(xi::StatusEffectFlag flag) -> uint8
{
    uint8 count = 0;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->HasEffectFlag(flag) && PStatusEffect->GetDuration() > 0s && !PStatusEffect->isDeleted())
        {
            count++;
        }
    }
    return count;
}

uint8 CStatusEffectContainer::GetLowestFreeSlot()
{
    uint8 lowestFreeSlot = 1;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetEffectSlot() == lowestFreeSlot && !PStatusEffect->isDeleted())
        {
            lowestFreeSlot++;
        }
        else if (PStatusEffect->GetEffectSlot() > lowestFreeSlot)
        {
            // Can break since the set is sorted by slot number,
            // and if we're past the lowest free one, we've found it already
            break;
        }
    }
    return lowestFreeSlot;
}

bool CStatusEffectContainer::CanGainStatusEffect(CStatusEffect* PStatusEffect)
{
    xi::StatusEffect statusEffect = PStatusEffect->GetStatusID();
    // check for immunities first
    switch (statusEffect)
    {
        case xi::StatusEffect::SleepI:
        case xi::StatusEffect::SleepIi:
        case xi::StatusEffect::Lullaby:
        {
            const uint16 subPower = PStatusEffect->GetSubPower();
            if (statuseffecthelpers::ShouldRejectSleepImmunity(
                    true,
                    statuseffecthelpers::IsLightSleepSubPower(subPower, ELEMENT_LIGHT),
                    statuseffecthelpers::IsDarkSleepSubPower(subPower, ELEMENT_DARK),
                    m_POwner->hasImmunity(IMMUNITY_LIGHT_SLEEP),
                    m_POwner->hasImmunity(IMMUNITY_DARK_SLEEP)))
            {
                return false;
            }

            break;
        }
        case xi::StatusEffect::Weight:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_GRAVITY)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Bind:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_BIND)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Stun:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_STUN)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Silence:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_SILENCE)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Paralysis:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_PARALYZE)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Blindness:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_BLIND)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Slow:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_SLOW)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Poison:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_POISON)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Elegy:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_ELEGY)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Requiem:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_REQUIEM)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Terror:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_TERROR)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::Petrification:
            if (statuseffecthelpers::ShouldRejectSimpleImmunity(m_POwner->hasImmunity(IMMUNITY_PETRIFY)))
            {
                return false;
            }
            break;
        case xi::StatusEffect::BlazeSpikes:
        case xi::StatusEffect::IceSpikes:
        case xi::StatusEffect::ShockSpikes:
        case xi::StatusEffect::Reprisal:
        case xi::StatusEffect::DelugeSpikes:
        case xi::StatusEffect::GaleSpikes:
        case xi::StatusEffect::GlintSpikes:
        case xi::StatusEffect::DamageSpikes:
        case xi::StatusEffect::DreadSpikes:
        case xi::StatusEffect::ClodSpikes:
        {
            const auto PAftermath = this->GetStatusEffect(xi::StatusEffect::Aftermath);
            // Geirskogul aftermath edge case
            if (statuseffecthelpers::ShouldBlockSpikesDueToAftermath(
                    PAftermath != nullptr,
                    PAftermath != nullptr ? PAftermath->GetPower() : 0))
            {
                return false;
            }
            break;
        }
        default:
            break;
    }

    // make sure pets can't be charmed
    if (statuseffecthelpers::ShouldBlockCharmOnPet(
            statusEffect == xi::StatusEffect::CharmI || statusEffect == xi::StatusEffect::CharmIi,
            m_POwner->PMaster != nullptr))
    {
        return false;
    }

    // check if a status effect blocks this
    xi::StatusEffect blockId = effects::EffectsParams[static_cast<uint16>(statusEffect)].BlockId;
    if (statuseffecthelpers::ShouldBlockByBlockId(static_cast<uint16>(blockId), HasStatusEffect(blockId)))
    {
        return false;
    }

    // check if negative is strong enough to stop this
    xi::StatusEffect negativeId = effects::EffectsParams[static_cast<uint16>(statusEffect)].NegativeId;
    if (static_cast<uint16>(negativeId) != 0)
    {
        CStatusEffect* negativeEffect = GetStatusEffect(negativeId);

        if (statuseffecthelpers::HasNegativeEffect(static_cast<uint16>(negativeId), negativeEffect != nullptr))
        {
            if (statuseffecthelpers::IsHasteVsSlowRemote(
                    statusEffect == xi::StatusEffect::Haste,
                    negativeEffect->GetStatusID() == xi::StatusEffect::Slow,
                    negativeEffect->GetSubPower()))
            {
                // slow i remote
                return true;
            }

            return statuseffecthelpers::CanGainVsNegative(
                PStatusEffect->GetTier(),
                PStatusEffect->GetPower(),
                negativeEffect->GetTier(),
                negativeEffect->GetPower(),
                PStatusEffect->GetTier() != 0 && negativeEffect->GetTier() != 0,
                statusEffect > negativeId);
        }
    }

    CStatusEffect* existingEffect = GetStatusEffect(statusEffect);

    // check overwrite
    if (statuseffecthelpers::HasExistingEffect(existingEffect != nullptr))
    {
        const auto overwrite = static_cast<uint8>(effects::EffectsParams[static_cast<uint16>(statusEffect)].Overwrite);
        return statuseffecthelpers::CanGainOverwrite(
            overwrite,
            PStatusEffect->GetTier(),
            PStatusEffect->GetPower(),
            existingEffect->GetTier(),
            existingEffect->GetPower());
    }

    return statuseffecthelpers::CanGainWhenNoExisting();
}

void CStatusEffectContainer::OverwriteStatusEffect(CStatusEffect* StatusEffect)
{
    TracyZoneScoped;

    xi::StatusEffect statusEffect = StatusEffect->GetStatusID();
    // remove effect
    xi::EffectOverwrite overwrite = effects::EffectsParams[static_cast<uint16>(statusEffect)].Overwrite;
    if (statuseffecthelpers::ShouldDeleteOnOverwrite(static_cast<uint8>(overwrite)))
    {
        DelStatusEffectSilent(statusEffect);
    }

    // remove effect by id
    xi::StatusEffect removeId = effects::EffectsParams[static_cast<uint16>(statusEffect)].RemoveId;
    if (statuseffecthelpers::ShouldRemoveLinkedId(static_cast<uint16>(removeId), static_cast<uint16>(xi::StatusEffect::Ko)))
    {
        DelStatusEffectSilent(removeId);
    }

    // remove negative effect
    xi::StatusEffect negativeId = effects::EffectsParams[static_cast<uint16>(statusEffect)].NegativeId;
    if (negativeId > xi::StatusEffect::Ko)
    {
        DelStatusEffectSilent(negativeId);
    }
}

/**************************************************************************
 *                                                                         *
 *  Adding a status effect to the container                                *
 *  If I'm not mistaken, then the max. possible number of effects is 32    *
 *                                                                         *
 **************************************************************************/

bool CStatusEffectContainer::AddStatusEffect(std::unique_ptr<CStatusEffect> PStatusEffectPtr, EffectNotice notice)
{
    if (statuseffecthelpers::ShouldRejectNullStatusEffect(PStatusEffectPtr == nullptr))
    {
        ShowWarning("status_effect_container::AddStatusEffect Status effect given was nullptr!");
        return false;
    }

    TracyZoneScoped;

    // Observing pointer into the owned effect; the container takes ownership on insert below.
    // If the effect is not gained, PStatusEffectPtr frees it when this function returns.
    CStatusEffect* PStatusEffect = PStatusEffectPtr.get();

    uint16 statusId = static_cast<uint16>(PStatusEffect->GetStatusID());

    if (statuseffecthelpers::ShouldRejectEffectIDOutOfRange(statusId, statuseffecthelpers::MaxEffectID))
    {
        ShowWarning("status_effect_container::AddStatusEffect statusId given is OVER limit %d", statusId);
        return false;
    }

    if (CanGainStatusEffect(PStatusEffect))
    {
        // check for minimum duration
        if (statuseffecthelpers::ShouldClampMinDuration(
                PStatusEffect->GetDuration().count(),
                effects::EffectsParams[static_cast<uint16>(statusId)].MinDuration.count()))
        {
            PStatusEffect->SetDuration(effects::EffectsParams[static_cast<uint16>(statusId)].MinDuration);
        }

        // remove clean up other effects
        OverwriteStatusEffect(PStatusEffect);

        SetEffectParams(PStatusEffect);

        // remove effects with same type
        DelStatusEffectsByType(PStatusEffect->GetEffectType());

        PStatusEffect->SetStartTime(timer::now());

        m_StatusEffectSet.insert(std::move(PStatusEffectPtr));

        HandleEffectGainSideEffects(PStatusEffect);

        luautils::OnEffectGain(m_POwner, PStatusEffect);
        m_POwner->PAI->EventHandler.triggerListener("EFFECT_GAIN", m_POwner, PStatusEffect);

        // Set owner after triggering all "effect gain" lua actions, to ensure effect:addMod() doesn't double up mod powers on the entity
        PStatusEffect->SetOwner(m_POwner);

        m_POwner->addModifiers(&PStatusEffect->modList());

        if (statuseffecthelpers::ShouldCheckManeuverAttachments(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                m_POwner->objtype == TYPE_PC))
        {
            puppetutils::CheckAttachmentsForManeuver((CCharEntity*)m_POwner, PStatusEffect->GetStatusID(), true);
        }

        if (statuseffecthelpers::ShouldUpdateHealthOnGain(m_POwner->health.maxhp != 0)) // make sure we're not in the middle of logging in
        {
            m_POwner->UpdateHealth();
        }

        if (m_POwner->objtype == TYPE_PC)
        {
            CCharEntity* PChar = (CCharEntity*)m_POwner;

            if (statuseffecthelpers::ShouldUpdateStatusIconsOnGain(true, PStatusEffect->GetIcon()))
            {
                UpdateStatusIcons();
            }

            if (statuseffecthelpers::ShouldCheckLatentsOnGain(true, m_POwner->health.maxhp != 0)) // make sure we're not in the middle of logging in
            {
                // check for latents
                PChar->PLatentEffectContainer->CheckLatentsFoodEffect();
                PChar->PLatentEffectContainer->CheckLatentsStatusEffect();
                PChar->PLatentEffectContainer->CheckLatentsRollSong();
                PChar->UpdateHealth();
            }
        }
        m_POwner->updatemask |= UPDATE_HP;

        return true;
    }

    return false;
}

/************************************************************************
 *                                                                       *
 *  Effects in all methods are removed equally, put this code in         *
 *  separate function. We remove icons only from CharEntity.             *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::DeleteStatusEffects()
{
    TracyZoneScoped;
    if (m_POwner != nullptr)
    {
        TracyZoneString(m_POwner->getName());
    }

    bool update_icons    = false;
    bool effects_removed = false;
    for (auto effect_iter = m_StatusEffectSet.begin(); effect_iter != m_StatusEffectSet.end();)
    {
        CStatusEffect* PStatusEffect = effect_iter->get();
        if (PStatusEffect->isDeleted())
        {
            if (PStatusEffect->GetIcon() != 0)
            {
                update_icons = true;
            }
            // erase() frees the owned effect.
            effect_iter     = m_StatusEffectSet.erase(effect_iter);
            effects_removed = true;
        }
        else
        {
            ++effect_iter;
        }
    }

    if (effects_removed && m_POwner != nullptr)
    {
        if (m_POwner->objtype == TYPE_PC)
        {
            CCharEntity* PChar = (CCharEntity*)m_POwner;

            if (update_icons)
            {
                UpdateStatusIcons();
            }

            // check for latents
            PChar->PLatentEffectContainer->CheckLatentsFoodEffect();
            PChar->PLatentEffectContainer->CheckLatentsStatusEffect();
            PChar->PLatentEffectContainer->CheckLatentsRollSong();
        }
        m_POwner->UpdateHealth();
    }
}

void CStatusEffectContainer::RemoveStatusEffect(CStatusEffect* PStatusEffect, const EffectNotice notice)
{
    TracyZoneScoped;

    if (statuseffecthelpers::ShouldMarkDeleted(PStatusEffect->isDeleted()))
    {
        PStatusEffect->markDeleted();
        luautils::OnEffectLose(m_POwner, PStatusEffect);
        m_POwner->PAI->EventHandler.triggerListener("EFFECT_LOSE", m_POwner, PStatusEffect);

        m_POwner->delModifiers(&PStatusEffect->modList());
        if (m_POwner->objtype == TYPE_PC)
        {
            auto* PChar = static_cast<CCharEntity*>(m_POwner);

            if (statuseffecthelpers::ShouldNotifyLossMessage(
                    notice == EffectNotice::Silent,
                    PStatusEffect->GetIcon(),
                    PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::NoLossMessage)))
            {
                const auto effectId  = PStatusEffect->GetStatusID();
                const uint16 catalogWearOff = static_cast<uint16>(effectId) < MAX_EFFECTID
                    ? static_cast<uint16>(effects::EffectsParams[static_cast<uint16>(effectId)].WearOffMessageId)
                    : static_cast<uint16>(MsgStd::EffectWearsOff);
                const auto messageId = statuseffecthelpers::WearOffMessageOrDefault(
                    static_cast<uint16>(effectId),
                    statuseffecthelpers::MaxEffectID,
                    catalogWearOff,
                    static_cast<uint16>(MsgStd::EffectWearsOff));

                // Notify owner that they lost their buff.
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, PStatusEffect->GetIcon(), 0, static_cast<MsgStd>(messageId));

                // Notify origin entity if they are in the same zone, and we are in their spawn list.
                const auto originId = PStatusEffect->GetOriginID();
                if (statuseffecthelpers::ShouldNotifyOriginOnLoss(originId, PChar->id, true) && m_POwner->loc.zone)
                {
                    auto* POriginEntity = m_POwner->loc.zone->GetCharByID(originId);
                    if (POriginEntity && charutils::hasEntitySpawned(POriginEntity, PChar))
                    {
                        POriginEntity->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(POriginEntity, PChar, PStatusEffect->GetIcon(), 0, static_cast<MsgStd>(messageId));
                    }
                }
            }

            if (statuseffecthelpers::ShouldCheckManeuverAttachments(
                    static_cast<uint16>(PStatusEffect->GetStatusID()),
                    true))
            {
                puppetutils::CheckAttachmentsForManeuver(static_cast<CCharEntity*>(m_POwner), PStatusEffect->GetStatusID(), false);
            }
        }
        else
        {
            if (statuseffecthelpers::ShouldNotifyNonPCLoss(
                    statuseffecthelpers::ShouldNotifyLossMessage(
                        notice == EffectNotice::Silent,
                        PStatusEffect->GetIcon(),
                        PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::NoLossMessage)),
                    m_POwner->isDead()))
            {
                const auto effectId  = PStatusEffect->GetStatusID();
                const uint16 catalogWearOff = static_cast<uint16>(effectId) < MAX_EFFECTID
                    ? static_cast<uint16>(effects::EffectsParams[static_cast<uint16>(effectId)].WearOffMessageId)
                    : static_cast<uint16>(MsgStd::EffectWearsOff);
                const auto messageId = statuseffecthelpers::WearOffMessageOrDefault(
                    static_cast<uint16>(effectId),
                    statuseffecthelpers::MaxEffectID,
                    catalogWearOff,
                    static_cast<uint16>(MsgStd::EffectWearsOff));

                // Notify origin entity if they are in the same zone, and we are in their spawn list.
                const auto originId = PStatusEffect->GetOriginID();
                if (statuseffecthelpers::ShouldNotifyOriginOnLoss(originId, 0, false) && m_POwner->loc.zone)
                {
                    auto* POriginEntity = m_POwner->loc.zone->GetCharByID(originId);
                    if (POriginEntity && charutils::hasEntitySpawned(POriginEntity, m_POwner))
                    {
                        POriginEntity->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(POriginEntity, m_POwner, PStatusEffect->GetIcon(), 0, static_cast<MsgStd>(messageId));
                    }
                }
            }
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Remove the status effect by its main and additional types.           *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::DelStatusEffect(xi::StatusEffect StatusID) -> bool
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::MatchesActiveStatusID(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(StatusID),
                PStatusEffect->isDeleted()))
        {
            RemoveStatusEffect(PStatusEffect.get());
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::DelStatusEffectSilent(xi::StatusEffect StatusID) -> bool
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == StatusID && !PStatusEffect->isDeleted())
        {
            RemoveStatusEffect(PStatusEffect.get(), EffectNotice::Silent);
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::DelStatusEffect(xi::StatusEffect StatusID, uint16 SubID) -> bool
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::MatchesDelBySubID(static_cast<uint16>(PStatusEffect->GetStatusID()), static_cast<uint16>(StatusID), PStatusEffect->GetSubID(), SubID, PStatusEffect->isDeleted()))
        {
            RemoveStatusEffect(PStatusEffect.get());
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::DelStatusEffectBySource(xi::StatusEffect StatusID, EffectSourceType sourceType, uint16 sourceTypeParam) -> bool
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::MatchesDelBySource(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(StatusID),
                PStatusEffect->GetSourceType(),
                static_cast<uint16>(sourceType),
                PStatusEffect->GetSourceTypeParam(),
                sourceTypeParam,
                PStatusEffect->isDeleted()))
        {
            RemoveStatusEffect(PStatusEffect.get());
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::DelStatusEffectByTier(xi::StatusEffect StatusID, uint16 tier) -> bool
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::MatchesDelByTier(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(StatusID),
                PStatusEffect->GetTier(),
                tier,
                PStatusEffect->isDeleted()))
        {
            RemoveStatusEffect(PStatusEffect.get(), EffectNotice::Silent);
            return true;
        }
    }
    return false;
}

/************************************************************************
 *                                                                       *
 *  Deletes all status effects without sending messages                  *
 *                                                                       *
 ************************************************************************/
void CStatusEffectContainer::KillAllStatusEffect()
{
    TracyZoneScoped;

    for (auto effect_iter = m_StatusEffectSet.begin(); effect_iter != m_StatusEffectSet.end();)
    {
        CStatusEffect* PStatusEffect = effect_iter->get();
        if (statuseffecthelpers::ShouldKillTimedEffect(PStatusEffect->GetDuration() != 0s))
        {
            luautils::OnEffectLose(m_POwner, PStatusEffect);

            m_POwner->delModifiers(&PStatusEffect->modList());

            // erase() frees the owned effect.
            effect_iter = m_StatusEffectSet.erase(effect_iter);
        }
        else
        {
            ++effect_iter;
        }
    }
    m_POwner->UpdateHealth();
}

void CStatusEffectContainer::HandleEffectGainSideEffects(CStatusEffect* StatusEffect)
{
    TracyZoneScoped;

    xi::StatusEffect effect = StatusEffect->GetStatusID();

    if (statuseffecthelpers::ShouldRunGainSideEffects(m_POwner->isAlive()))
    {
        // this should actually go into a char charm AI
        if (statuseffecthelpers::ShouldDespawnPetOnCharm(
                m_POwner->objtype == TYPE_PC,
                statuseffecthelpers::IsCharmStatusID(static_cast<uint16>(effect)),
                m_POwner->PPet != nullptr))
        {
            petutils::DespawnPet(m_POwner);
        }

        if (statuseffecthelpers::ShouldRewriteSleepIcon(
                HasPreventActionEffect(false),
                static_cast<uint16>(effect)))
        {
            // change icon of sleep II and lullaby. Apparently they don't stop player movement.
            StatusEffect->SetIcon(static_cast<uint16>(xi::StatusEffect::SleepI));
        }
    }
}

void CStatusEffectContainer::DelStatusEffectsByIcon(const uint16 BuffNo)
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::CanClientCancelIcon(
                PStatusEffect->GetIcon(),
                BuffNo,
                PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::NoCancel)))
        {
            // This covers all effects that client can remove. Function not used for anything the server removes.
            RemoveStatusEffect(PStatusEffect.get());
        }
    }
}

void CStatusEffectContainer::DelStatusEffectsByType(uint16 Type)
{
    if (statuseffecthelpers::ShouldRejectZeroEffectType(Type))
    {
        return;
    }

    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::MatchesEffectType(PStatusEffect->GetEffectType(), Type))
        {
            RemoveStatusEffect(PStatusEffect.get(), EffectNotice::Silent);
        }
    }
}

void CStatusEffectContainer::DelStatusEffectsByFlag(xi::StatusEffectFlag flag, EffectNotice notice)
{
    TracyZoneScoped;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::MatchesFlagForDelete(PStatusEffect->HasEffectFlag(flag)))
        {
            // If this is an NM/Mob Nightmare sleep, it can be removed explictly by a cure
            // see mobskills/nightmare.lua for full explanation
            if (statuseffecthelpers::ShouldSkipNightmareSleepOnDamageFlag(
                    (flag & xi::StatusEffectFlag::Damage) != xi::StatusEffectFlag::None,
                    static_cast<uint16>(PStatusEffect->GetStatusID()),
                    PStatusEffect->GetTier()))
            {
                continue;
            }

            RemoveStatusEffect(PStatusEffect.get(), notice);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Remove the first added negative effect with the erase flag.          *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::EraseStatusEffect() -> xi::StatusEffect
{
    TracyZoneScoped;

    std::vector<CStatusEffect*> erasableList;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsErasableCandidate(
                PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::Erasable),
                PStatusEffect->GetDuration() > 0s,
                PStatusEffect->isDeleted()))
        {
            erasableList.emplace_back(PStatusEffect.get());
        }
    }
    if (statuseffecthelpers::HasRemovableCandidates(erasableList.size()))
    {
        auto             rndIdx = xirand::GetRandomNumber(erasableList.size());
        xi::StatusEffect result = erasableList.at(rndIdx)->GetStatusID();
        RemoveStatusEffect(erasableList.at(rndIdx));
        return result;
    }
    return xi::StatusEffect::None;
}

auto CStatusEffectContainer::HealingWaltz() -> xi::StatusEffect
{
    TracyZoneScoped;

    std::vector<CStatusEffect*> waltzableList;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsWaltzableCandidate(
                PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::Waltzable),
                PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::Erasable),
                PStatusEffect->GetDuration() > 0s,
                PStatusEffect->isDeleted()))
        {
            waltzableList.emplace_back(PStatusEffect.get());
        }
    }
    if (statuseffecthelpers::HasRemovableCandidates(waltzableList.size()))
    {
        auto             rndIdx = xirand::GetRandomNumber(waltzableList.size());
        xi::StatusEffect result = waltzableList.at(rndIdx)->GetStatusID();
        RemoveStatusEffect(waltzableList.at(rndIdx));
        return result;
    }
    return xi::StatusEffect::None;
}

// Erases all negative status effects
// returns number of erased effects

uint8 CStatusEffectContainer::EraseAllStatusEffect()
{
    TracyZoneScoped;

    uint8 count = 0;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsErasableCandidate(
                PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::Erasable),
                PStatusEffect->GetDuration() > 0s,
                PStatusEffect->isDeleted()))
        {
            RemoveStatusEffect(PStatusEffect.get());
            count++;
        }
    }
    return count;
}

/************************************************************************
 *                                                                       *
 *  Remove the first added positive effect with the dispel flag.         *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::DispelStatusEffect(xi::StatusEffectFlag flag) -> xi::StatusEffect
{
    TracyZoneScoped;

    std::vector<CStatusEffect*> dispelableList;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsFlagRemovableCandidate(
                PStatusEffect->HasEffectFlag(flag),
                PStatusEffect->GetDuration() > 0s,
                PStatusEffect->isDeleted()))
        {
            dispelableList.emplace_back(PStatusEffect.get());
        }
    }
    if (statuseffecthelpers::HasRemovableCandidates(dispelableList.size()))
    {
        auto             rndIdx = xirand::GetRandomNumber(dispelableList.size());
        xi::StatusEffect result = dispelableList.at(rndIdx)->GetStatusID();
        RemoveStatusEffect(dispelableList.at(rndIdx), EffectNotice::Silent);
        return result;
    }
    return xi::StatusEffect::None;
}

/*
Dispels all positive status effects
returns number of dispelled effects
*/
auto CStatusEffectContainer::DispelAllStatusEffect(xi::StatusEffectFlag flag) -> uint8
{
    uint8 count = 0;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsFlagRemovableCandidate(
                PStatusEffect->HasEffectFlag(flag),
                PStatusEffect->GetDuration() > 0s,
                PStatusEffect->isDeleted()))
        {
            RemoveStatusEffect(PStatusEffect.get(), EffectNotice::Silent);
            count++;
        }
    }
    return count;
}

/************************************************************************
 *                                                                       *
 *  Check for the presence of a status effect in the container           *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::HasStatusEffect(xi::StatusEffect StatusID) -> bool
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == StatusID && !PStatusEffect->isDeleted())
        {
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::HasStatusEffectByFlag(xi::StatusEffectFlag flag) -> bool
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->HasEffectFlag(flag) && !PStatusEffect->isDeleted())
        {
            return true;
        }
    }
    return false;
}

/************************************************************************
 *                                                                       *
 *  Applies a bard song effect (after checking restrictions)             *
 *  Returns true if the effect is applied, false otherwise.              *
 *                                                                       *
 ************************************************************************/

bool CStatusEffectContainer::ApplyBardEffect(CStatusEffect* PStatusEffect, uint8 maxSongs)
{
    // if all match tier/id/effect then overwrite

    // if tier/effect match then overwrite //but id doesn't, NO xi::StatusEffect
    // if targ has <2 of your songs on, then just apply
    // if targ has 2 of your songs, remove oldest one and apply this one.

    uint8          numOfEffects = 0;
    CStatusEffect* oldestSong   = nullptr;
    for (const auto& ExistingStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsBardSongID(static_cast<uint16>(ExistingStatusEffect->GetStatusID()))) // is an active brd effect
        {
            if (statuseffecthelpers::IsSameBardSongOverwrite(
                    PStatusEffect->GetTier(),
                    static_cast<uint16>(PStatusEffect->GetStatusID()),
                    ExistingStatusEffect->GetTier(),
                    static_cast<uint16>(ExistingStatusEffect->GetStatusID())))
            { // same tier/type, overwrite
                // OVERWRITE
                PStatusEffect->SetEffectSlot(ExistingStatusEffect->GetEffectSlot()); // use same slot as the one it replaces
                DelStatusEffectByTier(PStatusEffect->GetStatusID(), PStatusEffect->GetTier());
            }
            if (statuseffecthelpers::IsOwnBardSong(ExistingStatusEffect->GetSubID(), PStatusEffect->GetSubID()))
            { // YOUR BRD effect
                numOfEffects++;
                if (!oldestSong)
                {
                    oldestSong = ExistingStatusEffect.get();
                }
                else if (statuseffecthelpers::IsEarlierSongExpiry(
                             (ExistingStatusEffect->GetStartTime() + ExistingStatusEffect->GetDuration()).time_since_epoch().count(),
                             (oldestSong->GetStartTime() + oldestSong->GetDuration()).time_since_epoch().count()))
                {
                    oldestSong = ExistingStatusEffect.get();
                }
            }
        }
    }

    if (statuseffecthelpers::CanApplyBardWithoutReplace(numOfEffects, maxSongs))
    {
        if (statuseffecthelpers::ShouldAssignLowestFreeSlot(PStatusEffect->GetEffectSlot()))
        {
            // use lowest available slot, unless it's replacing an existing song
            PStatusEffect->SetEffectSlot(GetLowestFreeSlot());
        }
        AddStatusEffect(std::unique_ptr<CStatusEffect>(PStatusEffect));
        return true;
    }
    else if (statuseffecthelpers::CanApplyBardReplacingOldest(oldestSong != nullptr))
    {
        // overwrite oldest
        PStatusEffect->SetEffectSlot(oldestSong->GetEffectSlot());
        DelStatusEffectByTier(oldestSong->GetStatusID(), oldestSong->GetTier());
        AddStatusEffect(std::unique_ptr<CStatusEffect>(PStatusEffect));
        return true;
    }

    return false;
}

uint8 CStatusEffectContainer::GetActiveManeuverCount()
{
    return GetStatusEffectCountInIDRange(xi::StatusEffect::FireManeuver, xi::StatusEffect::DarkManeuver);
}

void CStatusEffectContainer::RemoveOldestManeuver()
{
    RemoveOldestStatusEffectInIDRange(xi::StatusEffect::FireManeuver, xi::StatusEffect::DarkManeuver);
}

void CStatusEffectContainer::RemoveAllManeuvers()
{
    RemoveAllStatusEffectsInIDRange(xi::StatusEffect::FireManeuver, xi::StatusEffect::DarkManeuver);
}

auto CStatusEffectContainer::GetAllRuneEffects() -> std::vector<xi::StatusEffect>
{
    return GetStatusEffectsInIDRange(xi::StatusEffect::Ignis, xi::StatusEffect::Tenebrae);
}

uint8 CStatusEffectContainer::GetActiveRuneCount()
{
    return GetStatusEffectCountInIDRange(xi::StatusEffect::Ignis, xi::StatusEffect::Tenebrae);
}

auto CStatusEffectContainer::GetHighestRuneEffect() -> xi::StatusEffect
{
    std::unordered_map<xi::StatusEffect, uint8> runeEffects;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldCountActiveInRange(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                statuseffecthelpers::RuneIDFirst,
                statuseffecthelpers::RuneIDLast,
                PStatusEffect->isDeleted()))
        {
            if (runeEffects.count(PStatusEffect->GetStatusID()) == 0)
            {
                runeEffects[PStatusEffect->GetStatusID()] = 1;
            }
            else
            {
                runeEffects.at(PStatusEffect->GetStatusID())++;
            }
        }
    }

    xi::StatusEffect highestRune      = xi::StatusEffect::None;
    uint8            highestRuneValue = 0;
    bool             hasCurrent       = false;

    for (auto iter = runeEffects.begin(); iter != runeEffects.end(); ++iter)
    {
        if (statuseffecthelpers::PreferHigherRuneCount(hasCurrent, highestRuneValue, iter->second))
        {
            highestRune      = iter->first;
            highestRuneValue = iter->second;
            hasCurrent       = true;
        }
    }

    return highestRune;
}

auto CStatusEffectContainer::GetNewestRuneEffect() -> xi::StatusEffect
{
    return GetNewestStatusEffectInIDRange(xi::StatusEffect::Ignis, xi::StatusEffect::Tenebrae);
}

void CStatusEffectContainer::RemoveNewestRune()
{
    RemoveNewestStatusEffectInIDRange(xi::StatusEffect::Ignis, xi::StatusEffect::Tenebrae);
}

void CStatusEffectContainer::RemoveOldestRune()
{
    RemoveOldestStatusEffectInIDRange(xi::StatusEffect::Ignis, xi::StatusEffect::Tenebrae);
}

void CStatusEffectContainer::RemoveAllRunes()
{
    RemoveAllStatusEffectsInIDRange(xi::StatusEffect::Ignis, xi::StatusEffect::Tenebrae);
}

/************************************************************************
 *                                                                       *
 *  Check for the presence of a status effect in a container with a      *
 *  unique subid                                                         *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::HasStatusEffect(xi::StatusEffect StatusID, uint16 SubID) -> bool
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == StatusID && PStatusEffect->GetSubID() == SubID && !PStatusEffect->isDeleted())
        {
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::HasStatusEffect(std::initializer_list<xi::StatusEffect> effects) -> bool
{
    for (auto&& effect_from_set : m_StatusEffectSet)
    {
        if (!effect_from_set->isDeleted())
        {
            for (auto&& effect_to_check : effects)
            {
                if (effect_to_check == effect_from_set->GetStatusID())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

auto CStatusEffectContainer::GetStatusEffect(xi::StatusEffect StatusID) -> CStatusEffect*
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == StatusID && !PStatusEffect->isDeleted())
        {
            return PStatusEffect.get();
        }
    }
    return nullptr;
}

auto CStatusEffectContainer::GetStatusEffect(xi::StatusEffect StatusID, uint32 SubID) -> CStatusEffect*
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == StatusID && PStatusEffect->GetSubID() == SubID && !PStatusEffect->isDeleted())
        {
            return PStatusEffect.get();
        }
    }
    return nullptr;
}

auto CStatusEffectContainer::GetStatusEffectBySource(xi::StatusEffect StatusID, EffectSourceType SourceType, uint16 SourceTypeParam) -> CStatusEffect*
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() == StatusID && PStatusEffect->GetSourceType() == SourceType && PStatusEffect->GetSourceTypeParam() == SourceTypeParam && !PStatusEffect->isDeleted())
        {
            return PStatusEffect.get();
        }
    }
    return nullptr;
}

/************************************************************************
 *                                                                       *
 * Dispels one effect and returns it (used in mob abilities)             *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::StealStatusEffect(xi::StatusEffectFlag flag, EffectNotice notice) -> std::unique_ptr<CStatusEffect>
{
    std::vector<CStatusEffect*> dispelableList;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->HasEffectFlag(flag) && PStatusEffect->GetDuration() > 0s && !PStatusEffect->isDeleted())
        {
            dispelableList.emplace_back(PStatusEffect.get());
        }
    }
    if (!dispelableList.empty())
    {
        auto rndIdx = xirand::GetRandomNumber(dispelableList.size());

        CStatusEffect* oldEffect = dispelableList.at(rndIdx);

        // make a copy the caller takes ownership of
        auto EffectCopy = std::make_unique<CStatusEffect>(
            oldEffect->GetStatusID(),
            oldEffect->GetIcon(),
            oldEffect->GetPower(),
            oldEffect->GetTickTime(),
            oldEffect->GetDuration(),
            oldEffect->GetSubID(),
            oldEffect->GetSubPower(),
            oldEffect->GetSubIcon(),
            oldEffect->GetTier(),
            oldEffect->GetEffectFlags(),
            oldEffect->GetSourceType(),
            oldEffect->GetSourceTypeParam(),
            oldEffect->GetOriginID());

        RemoveStatusEffect(oldEffect, notice);

        return EffectCopy;
    }
    return nullptr;
}

/************************************************************************
 *                                                                       *
 *  Recalculate the icons of all effects                                 *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::UpdateStatusIcons()
{
    if (m_POwner->objtype != TYPE_PC)
    {
        return;
    }

    auto* PChar = static_cast<CCharEntity*>(m_POwner);

    m_Flags = 0;
    std::memset(m_StatusIcons, static_cast<int>(xi::StatusEffect::None), sizeof(m_StatusIcons));

    uint8 count = 0;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        uint16 icon = PStatusEffect->GetIcon();

        if (icon != 0)
        {
            if (icon >= 256 && icon < 512)
            {
                m_Flags |= 1LL << (count * 2);
            }
            if (icon >= 512)
            {
                m_Flags |= 1LL << (count * 2 + 1);
            }
            // Note: it may be possible that having both bits set is for effects over 768, but there aren't
            // that many effects as of this writing
            m_StatusIcons[count] = (uint8)icon;

            if (++count == 32)
            {
                break;
            }
        }
    }
    PChar->m_EffectsChanged = true;

    if (PChar->PParty)
    {
        PChar->PParty->EffectsChanged();
    }
}

auto CStatusEffectContainer::GetStatusEffectsInIDRange(xi::StatusEffect start, xi::StatusEffect end) -> std::vector<xi::StatusEffect>
{
    std::vector<xi::StatusEffect> effectList;

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldCountActiveInRange(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(start),
                static_cast<uint16>(end),
                PStatusEffect->isDeleted()))
        {
            effectList.emplace_back(PStatusEffect->GetStatusID());
        }
    }
    return effectList;
}

auto CStatusEffectContainer::GetStatusEffectCountInIDRange(xi::StatusEffect start, xi::StatusEffect end) -> uint8
{
    uint8 count = 0;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldCountActiveInRange(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(start),
                static_cast<uint16>(end),
                PStatusEffect->isDeleted()))
        {
            count++;
        }
    }
    return count;
}

auto CStatusEffectContainer::GetNewestStatusEffectInIDRange(xi::StatusEffect start, xi::StatusEffect end) -> xi::StatusEffect
{
    CStatusEffect* newest = nullptr;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() >= start && PStatusEffect->GetStatusID() <= end && !PStatusEffect->isDeleted())
        {
            if (!newest || PStatusEffect->GetStartTime() > newest->GetStartTime())
            {
                newest = PStatusEffect.get();
            }
        }
    }
    if (newest)
    {
        return newest->GetStatusID();
    }
    return xi::StatusEffect::None;
}

void CStatusEffectContainer::RemoveOldestStatusEffectInIDRange(xi::StatusEffect start, xi::StatusEffect end)
{
    CStatusEffect* oldest = nullptr;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (PStatusEffect->GetStatusID() >= start && PStatusEffect->GetStatusID() <= end && !PStatusEffect->isDeleted())
        {
            if (!oldest || PStatusEffect->GetStartTime() < oldest->GetStartTime())
            {
                oldest = PStatusEffect.get();
            }
        }
    }
    if (oldest)
    {
        RemoveStatusEffect(oldest, EffectNotice::Silent);
    }
}

void CStatusEffectContainer::RemoveNewestStatusEffectInIDRange(xi::StatusEffect start, xi::StatusEffect end)
{
    CStatusEffect* newest = nullptr;
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldSelectNewerInRange(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(start),
                static_cast<uint16>(end),
                PStatusEffect->isDeleted(),
                newest != nullptr,
                PStatusEffect->GetStartTime().time_since_epoch().count(),
                newest ? newest->GetStartTime().time_since_epoch().count() : 0))
        {
            newest = PStatusEffect.get();
        }
    }
    if (newest)
    {
        RemoveStatusEffect(newest, EffectNotice::Silent);
    }
}

void CStatusEffectContainer::RemoveAllStatusEffectsInIDRange(xi::StatusEffect start, xi::StatusEffect end)
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldRemoveAllInRange(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                static_cast<uint16>(start),
                static_cast<uint16>(end)))
        {
            RemoveStatusEffect(PStatusEffect.get(), EffectNotice::Silent);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Install the name of the effect to work with scripts                  *
 *                                                                       *
 ************************************************************************/

auto CStatusEffectContainer::SetEffectParams(CStatusEffect* StatusEffect) -> void
{
    if (statuseffecthelpers::ShouldRejectEffectIDOutOfRange(
            static_cast<uint16>(StatusEffect->GetStatusID()),
            statuseffecthelpers::MaxEffectID))
    {
        ShowWarning("Status Effect ID (%d) exceeds MAX_EFFECTID", static_cast<uint16>(StatusEffect->GetStatusID()));
        return;
    }

    auto subType = StatusEffect->GetSubID();

    if (statuseffecthelpers::ShouldRejectNoneZeroSub(
            static_cast<uint16>(StatusEffect->GetStatusID()),
            subType,
            statuseffecthelpers::StatusIDNoneEffect))
    {
        ShowWarning("None-type Effect has SubID of 0");
        return;
    }

    std::string      name;
    xi::StatusEffect effect                = StatusEffect->GetStatusID();
    auto             effectSourceType      = StatusEffect->GetSourceType();
    auto             effectSourceTypeParam = StatusEffect->GetSourceTypeParam();

    // check if status effect is special case from a usable equipped item that grants enchantment
    bool effectFromItemEnchant = false;
    bool effectFromItemFood    = false;

    if (statuseffecthelpers::HasEffectSource(effectSourceType, effectSourceTypeParam))
    {
        if (statuseffecthelpers::IsEquippedItemSource(effectSourceType))
        {
            auto PItem = xi::items::lookup(effectSourceTypeParam);
            if (PItem != nullptr)
            {
                // get the item lua script and check if it has valid functions
                auto itemName     = statuseffecthelpers::FormatItemScriptName(PItem->getName());
                auto itemFullName = fmt::format("./scripts/{}.lua", itemName);
                auto onEffectGain = luautils::getCachedFileFunction(itemFullName, "onEffectGain");
                auto onEffectLose = luautils::getCachedFileFunction(itemFullName, "onEffectLose");

                effectFromItemEnchant = statuseffecthelpers::ShouldSetItemScriptName(onEffectGain.valid(), onEffectLose.valid());

                // if it does have valid functions then set the status effect name as the script (similar to actual status effects)
                if (effectFromItemEnchant)
                {
                    name = itemName;
                }
            }
        }
        else if (statuseffecthelpers::IsFoodSource(effectSourceType))
        {
            auto PItem = xi::items::lookup(StatusEffect->GetSourceTypeParam());
            if (PItem != nullptr)
            {
                // get the item lua script and check if it has valid functions
                auto itemName     = statuseffecthelpers::FormatItemScriptName(PItem->getName());
                auto itemFullName = fmt::format("./scripts/{}.lua", itemName);
                auto onEffectGain = luautils::getCachedFileFunction(itemFullName, "onEffectGain");
                auto onEffectLose = luautils::getCachedFileFunction(itemFullName, "onEffectLose");

                effectFromItemFood = statuseffecthelpers::ShouldSetItemScriptName(onEffectGain.valid(), onEffectLose.valid());

                // if it does have valid functions then set the status effect name as the script (similar to actual status effects)
                if (effectFromItemFood)
                {
                    name = itemName;
                }
            }
        }
    }

    // Effects that use /server/scripts/effects/ as their lua file source.
    const bool useEffectsPath = statuseffecthelpers::ShouldUseEffectsScriptPath(
        effectFromItemEnchant,
        effectFromItemFood,
        effect == xi::StatusEffect::Enchantment,
        effectSourceType == EffectSourceType::SOURCE_EQUIPPED_ITEM,
        effect == xi::StatusEffect::Food,
        effectSourceTypeParam);
    if (useEffectsPath)
    {
        name = statuseffecthelpers::FormatEffectsScriptName(effects::EffectsParams[static_cast<uint16>(effect)].Name);
    }
    // Is an effect from a usable item not caught above.
    // Known use cases: Enchantments without an effect source.
    else
    {
        const CItem* Ptem = xi::items::lookup(subType);
        if (statuseffecthelpers::ShouldUseItemSubTypeScript(useEffectsPath, subType, Ptem != nullptr))
        {
            name = statuseffecthelpers::FormatItemScriptName(Ptem->getName());
        }
    }

    StatusEffect->SetEffectName(name);
    StatusEffect->AddEffectFlag(effects::EffectsParams[static_cast<uint16>(effect)].Flag);
    StatusEffect->SetEffectType(effects::EffectsParams[static_cast<uint16>(effect)].Type);
}

/************************************************************************
 *                                                                       *
 *  Load character effects                                               *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::LoadStatusEffects()
{
    if (m_POwner->objtype != TYPE_PC)
    {
        ShowWarning("Non-PC calling function (%s).", m_POwner->getName());
        return;
    }

    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    std::vector<std::unique_ptr<CStatusEffect>> PEffectList;

    const auto rset = db::preparedStmt("SELECT effectid, icon, power, tick, "
                                       "duration, subid, subpower, tier, "
                                       "flags, timestamp, sourcetype, sourcetypeparam, originid "
                                       "FROM char_effects "
                                       "WHERE charid = ?",
                                       m_POwner->id);
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        const auto      flags    = rset->get<uint32>("flags");
        timer::duration duration = std::chrono::seconds(rset->get<uint32>("duration"));
        const auto      effectID = rset->get<xi::StatusEffect>("effectid");

        if (flags & static_cast<uint32>(xi::StatusEffectFlag::OfflineTick))
        {
            auto currentTime = timer::now();
            auto startTime   = timer::from_utc(earth_time::time_point(std::chrono::seconds(rset->get<uint32>("timestamp"))));
            auto endTime     = startTime + duration;
            if (currentTime < endTime)
            {
                duration = endTime - currentTime;
            }
            else if (effectID == xi::StatusEffect::Visitant)
            {
                // Visitant effect expired while offline, but there's other logic to handle.
                // Set duration to 1 so that it expires after zoning in, and the player is ejected.
                duration = 1s;
            }
            else
            {
                // Effect expired while offline
                continue;
            }
        }
        auto PStatusEffect =
            std::make_unique<CStatusEffect>(effectID,
                                            rset->get<uint16>("icon"),
                                            rset->get<uint16>("power"),
                                            std::chrono::seconds(rset->get<uint16>("tick")),
                                            duration,
                                            rset->get<uint16>("subid"),
                                            rset->get<uint16>("subpower"),
                                            0, // SubIcon (not persisted in char_effects)
                                            rset->get<uint16>("tier"),
                                            static_cast<xi::StatusEffectFlag>(flags),
                                            rset->get<uint16>("sourcetype"),
                                            rset->get<uint32>("sourcetypeparam"),
                                            rset->get<uint32>("originid"));

        // load shadows left
        if (statuseffecthelpers::ShouldLoadCopyImageUtsusemi(static_cast<uint16>(PStatusEffect->GetStatusID())))
        {
            m_POwner->setModifier(Mod::UTSUSEMI, PStatusEffect->GetSubPower());
        }
        else if (statuseffecthelpers::ShouldLoadBlinkMod(static_cast<uint16>(PStatusEffect->GetStatusID())))
        {
            m_POwner->setModifier(Mod::BLINK, PStatusEffect->GetPower());
        }

        PEffectList.emplace_back(std::move(PStatusEffect));
    }

    for (auto&& PStatusEffect : PEffectList)
    {
        AddStatusEffect(std::move(PStatusEffect));
    }

    m_POwner->UpdateHealth(); // after loading the effects, recalculate the maximum amount of HP/MP
}

/************************************************************************
 *                                                                       *
 *  Save temporary character effects                                     *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::SaveStatusEffects(bool logout)
{
    // Print entity name and bail out if entity isn't a player.
    if (statuseffecthelpers::ShouldRejectNonPCSave(m_POwner->objtype == TYPE_PC))
    {
        ShowDebug("Non-player entity %s (ID: %d) attempt to save Status Effect.", m_POwner->getName(), m_POwner->id);

        return;
    }

    db::preparedStmt("DELETE FROM char_effects WHERE charid = ?", m_POwner->id);

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        const auto durationSeconds     = timer::count_seconds(PStatusEffect->GetDuration());
        const auto realDurationSeconds = timer::count_seconds(PStatusEffect->GetStartTime() + PStatusEffect->GetDuration() - timer::now());

        // Pure per-effect disposition (slice 2793). Host owns strip + SQL.
        const auto plan = statuseffecthelpers::PlanSaveStatusEffect(
            PStatusEffect->isDeleted(),
            logout,
            PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::Logout),
            PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::OnZone),
            realDurationSeconds,
            durationSeconds,
            static_cast<uint16>(PStatusEffect->GetStatusID()));

        switch (plan.action)
        {
            case statuseffecthelpers::SaveEffectAction::SkipDeleted:
            case statuseffecthelpers::SaveEffectAction::DropNoPersist:
                continue;
            case statuseffecthelpers::SaveEffectAction::Strip:
                RemoveStatusEffect(PStatusEffect.get(), EffectNotice::Silent);
                continue;
            case statuseffecthelpers::SaveEffectAction::Persist:
                break;
        }

        // save power of utsusemi and blink / stoneskin (plan resync flags)
        if (plan.resyncUtsusemi)
        {
            PStatusEffect->SetSubPower(m_POwner->getMod(Mod::UTSUSEMI));
        }
        else if (plan.resyncBlink)
        {
            PStatusEffect->SetPower(m_POwner->getMod(Mod::BLINK));
        }
        else if (plan.resyncStoneskin)
        {
            PStatusEffect->SetPower(m_POwner->getMod(Mod::STONESKIN));
        }

        uint32 duration = statuseffecthelpers::ComputePersistedDurationSeconds(
            durationSeconds,
            realDurationSeconds,
            PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::OfflineTick));

        if (durationSeconds > 0 && duration == 0)
        {
            continue;
        }

        uint32 tick      = static_cast<uint32>(timer::count_seconds(PStatusEffect->GetTickTime()));
        auto   timestamp = earth_time::timestamp(timer::to_utc(PStatusEffect->GetStartTime()));

        db::preparedStmt("INSERT INTO char_effects (charid, effectid, icon, power, tick, duration, subid, subpower, tier, flags, timestamp, sourcetype, sourcetypeparam, originid) "
                         "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                         m_POwner->id,
                         static_cast<uint16>(PStatusEffect->GetStatusID()),
                         PStatusEffect->GetIcon(),
                         PStatusEffect->GetPower(),
                         tick,
                         duration,
                         PStatusEffect->GetSubID(),
                         PStatusEffect->GetSubPower(),
                         PStatusEffect->GetTier(),
                         static_cast<uint32>(PStatusEffect->GetEffectFlags()),
                         timestamp,
                         PStatusEffect->GetSourceType(),
                         PStatusEffect->GetSourceTypeParam(),
                         PStatusEffect->GetOriginID());
    }
    DeleteStatusEffects();
}

/************************************************************************
 *                                                                       *
 *  Expires status effects                                               *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::CheckEffectsExpiry(timer::time_point tick)
{
    if (m_POwner == nullptr)
    {
        ShowWarning("m_POwner was null.");
        return;
    }

    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldExpireEffect(
                PStatusEffect->GetDuration() != 0s,
                (PStatusEffect->GetStartTime() + PStatusEffect->GetDuration()).time_since_epoch().count(),
                tick.time_since_epoch().count()))
        {
            RemoveStatusEffect(PStatusEffect.get());
        }
    }
    DeleteStatusEffects();
}

void CStatusEffectContainer::HandleAura(CStatusEffect* PStatusEffect)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    CBattleEntity* PEntity    = m_POwner;
    AURA_TARGET    auraTarget = static_cast<AURA_TARGET>(PStatusEffect->GetTier());
    float          aura_range = statuseffecthelpers::ComputeAuraRange(PEntity->getMod(Mod::AURA_SIZE)); // Adding to this mod should be the value you want * 100

    if (statuseffecthelpers::ShouldUseMasterForAura(PEntity->objtype == TYPE_PET, PEntity->objtype == TYPE_TRUST))
    {
        PEntity = PEntity->PMaster;
    }

    if (PEntity->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(PEntity);
        if (auraTarget == AURA_TARGET::ALLIES)
        {
            PChar->ForPartyWithTrusts(
                [&](CBattleEntity* PMember)
                {
                    // Pure ally eligibility (slice 2796). Distance host-injected.
                    const bool   memberNull        = PMember == nullptr;
                    const bool   ownerZonePresent  = m_POwner->loc.zone != nullptr;
                    const bool   memberZonePresent = !memberNull && PMember->loc.zone != nullptr;
                    const uint16 ownerZoneID       = ownerZonePresent ? static_cast<uint16>(m_POwner->loc.zone->GetID()) : 0;
                    const uint16 memberZoneID      = memberZonePresent ? static_cast<uint16>(PMember->loc.zone->GetID()) : 0;
                    const bool   sameZone          = statuseffecthelpers::IsSameZoneForAura(
                        ownerZoneID, memberZoneID, ownerZonePresent, memberZonePresent);
                    const bool inRange = !memberNull && statuseffecthelpers::IsInAuraRange(
                                                           distance(m_POwner->loc.p, PMember->loc.p),
                                                           aura_range,
                                                           PMember->modelHitboxSize);
                    const bool isDead = !memberNull && PMember->isDead();

                    if (statuseffecthelpers::ShouldAcceptAuraAlly(memberNull, sameZone, inRange, isDead))
                    {
                        CStatusEffect* PEffect = PMember->StatusEffectContainer->GetStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()));

                        const bool hasAlwaysExpiringFlag =
                            PEffect != nullptr &&
                            (PEffect->GetEffectFlags() & xi::StatusEffectFlag::AlwaysExpiring) != xi::StatusEffectFlag::None;
                        if (statuseffecthelpers::ShouldRefreshAlwaysExpiringAura(PEffect != nullptr, hasAlwaysExpiringFlag))
                        {
                            // Pure plan (slice 2798). Host owns timer refresh + Lua power update.
                            const auto plan = statuseffecthelpers::PlanAuraExistingAlwaysExpiring(
                                hasAlwaysExpiringFlag,
                                PEffect->GetPower(),
                                PStatusEffect->GetSubPower());

                            if (plan.refreshStartTime)
                            {
                                PEffect->SetStartTime(timer::now());
                            }

                            // Effect updated, probably from Ecliptic Attrition
                            // Update status effect with new potency.
                            // Take care to design your "owning" effects such as the xi::StatusEffect::ColureActive to control the subpower, rather than the resulting effect ticking down.
                            // Otherwise odd things may happen
                            if (plan.updatePower)
                            {
                                luautils::OnEffectLose(PMember, PEffect);
                                PEffect->SetPower(PStatusEffect->GetSubPower());
                                luautils::OnEffectGain(PMember, PEffect);
                            }
                        }
                        else
                        {
                            uint16 icon = statuseffecthelpers::ResolveAuraSubIcon(PStatusEffect->GetSubIcon(), PStatusEffect->GetSubID());

                            PEffect = new CStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()), // Effect ID
                                                        icon,                                                     // Effect Icon
                                                        PStatusEffect->GetSubPower(),                             // Power
                                                        3s,                                                       // Tick
                                                        4s);                                                      // Duration
                            PEffect->AddEffectFlag(xi::StatusEffectFlag::NoLossMessage);
                            PEffect->AddEffectFlag(xi::StatusEffectFlag::AlwaysExpiring);
                            PMember->StatusEffectContainer->AddStatusEffect(std::unique_ptr<CStatusEffect>(PEffect), EffectNotice::Silent);
                        }
                    }
                });
        }
        else if (auraTarget == AURA_TARGET::ENEMIES)
        {
            for (CBattleEntity* PTarget : *PEntity->PNotorietyContainer)
            { // Check for trust here so negitive effects wont affect trust
                if (PTarget != nullptr &&
                    PTarget->loc.zone &&
                    PEntity->loc.zone &&
                    PTarget->objtype != TYPE_TRUST && PEntity->loc.zone->GetID() == PTarget->loc.zone->GetID() && distance(m_POwner->loc.p, PTarget->loc.p) <= aura_range + PTarget->modelHitboxSize &&
                    !PTarget->isDead())
                {
                    CStatusEffect* PEffect = PTarget->StatusEffectContainer->GetStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()));

                    const bool hasAlwaysExpiringFlag =
                        PEffect != nullptr &&
                        (PEffect->GetEffectFlags() & xi::StatusEffectFlag::AlwaysExpiring) != xi::StatusEffectFlag::None;
                    if (statuseffecthelpers::ShouldRefreshAlwaysExpiringAura(PEffect != nullptr, hasAlwaysExpiringFlag))
                    {
                        // Pure plan (slice 2798). Host owns timer refresh + Lua power update.
                        const auto plan = statuseffecthelpers::PlanAuraExistingAlwaysExpiring(
                            hasAlwaysExpiringFlag,
                            PEffect->GetPower(),
                            PStatusEffect->GetSubPower());

                        if (plan.refreshStartTime)
                        {
                            PEffect->SetStartTime(timer::now());
                        }

                        // Effect updated, probably from Ecliptic Attrition
                        // Update status effect with new potency.
                        // Take care to design your "owning" effects such as the xi::StatusEffect::ColureActive to control the subpower, rather than the resulting effect ticking down.
                        // Otherwise odd things may happen
                        if (plan.updatePower)
                        {
                            luautils::OnEffectLose(PTarget, PEffect);
                            PEffect->SetPower(PStatusEffect->GetSubPower());
                            luautils::OnEffectGain(PTarget, PEffect);
                        }
                    }
                    else
                    {
                        uint16 icon = statuseffecthelpers::ResolveAuraSubIcon(PStatusEffect->GetSubIcon(), PStatusEffect->GetSubID());

                        PEffect = new CStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()), // Effect ID
                                                    icon,                                                     // Effect Icon
                                                    PStatusEffect->GetSubPower(),                             // Power
                                                    3s,                                                       // Tick
                                                    4s);                                                      // Duration
                        PEffect->AddEffectFlag(xi::StatusEffectFlag::NoLossMessage);
                        PEffect->AddEffectFlag(xi::StatusEffectFlag::AlwaysExpiring);
                        PTarget->StatusEffectContainer->AddStatusEffect(std::unique_ptr<CStatusEffect>(PEffect), EffectNotice::Silent);
                    }
                }
            }
        }
    }
    else if (PEntity->objtype == TYPE_MOB)
    {
        if (auraTarget == AURA_TARGET::ALLIES)
        {
            PEntity->ForParty(
                [&](CBattleEntity* PMember)
                {
                    // Pure ally eligibility (slice 2796). Production compares
                    // PEntity zone ID after null-checking m_POwner zone; distance host-injected.
                    const bool   memberNull        = PMember == nullptr;
                    const bool   ownerZonePresent  = m_POwner->loc.zone != nullptr;
                    const bool   memberZonePresent = !memberNull && PMember->loc.zone != nullptr;
                    const uint16 ownerZoneID       = PEntity->loc.zone != nullptr ? static_cast<uint16>(PEntity->loc.zone->GetID()) : 0;
                    const uint16 memberZoneID      = memberZonePresent ? static_cast<uint16>(PMember->loc.zone->GetID()) : 0;
                    const bool   sameZone          = statuseffecthelpers::IsSameZoneForAura(
                        ownerZoneID, memberZoneID, ownerZonePresent, memberZonePresent);
                    const bool inRange = !memberNull && statuseffecthelpers::IsInAuraRange(
                                                           distance(m_POwner->loc.p, PMember->loc.p),
                                                           aura_range,
                                                           PMember->modelHitboxSize);
                    const bool isDead = !memberNull && PMember->isDead();

                    if (statuseffecthelpers::ShouldAcceptAuraAlly(memberNull, sameZone, inRange, isDead))
                    {
                        CStatusEffect* PEffect = PMember->StatusEffectContainer->GetStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()));

                        const bool hasAlwaysExpiringFlag =
                            PEffect != nullptr &&
                            (PEffect->GetEffectFlags() & xi::StatusEffectFlag::AlwaysExpiring) != xi::StatusEffectFlag::None;
                        if (statuseffecthelpers::ShouldRefreshAlwaysExpiringAura(PEffect != nullptr, hasAlwaysExpiringFlag))
                        {
                            // Pure plan (slice 2798). Host owns timer refresh + Lua power update.
                            const auto plan = statuseffecthelpers::PlanAuraExistingAlwaysExpiring(
                                hasAlwaysExpiringFlag,
                                PEffect->GetPower(),
                                PStatusEffect->GetSubPower());

                            if (plan.refreshStartTime)
                            {
                                PEffect->SetStartTime(timer::now());
                            }

                            // Effect updated, probably from Ecliptic Attrition
                            // Update status effect with new potency.
                            // Take care to design your "owning" effects such as the xi::StatusEffect::ColureActive to control the subpower, rather than the resulting effect ticking down.
                            // Otherwise odd things may happen
                            if (plan.updatePower)
                            {
                                luautils::OnEffectLose(PMember, PEffect);
                                PEffect->SetPower(PStatusEffect->GetSubPower());
                                luautils::OnEffectGain(PMember, PEffect);
                            }
                        }
                        else
                        {
                            uint16 icon = statuseffecthelpers::ResolveAuraSubIcon(PStatusEffect->GetSubIcon(), PStatusEffect->GetSubID());

                            PEffect = new CStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()), // Effect ID
                                                        icon,                                                     // Effect Icon
                                                        PStatusEffect->GetSubPower(),                             // Power
                                                        3s,                                                       // Tick
                                                        4s);                                                      // Duration
                            PEffect->AddEffectFlag(xi::StatusEffectFlag::NoLossMessage);
                            PEffect->AddEffectFlag(xi::StatusEffectFlag::AlwaysExpiring);
                            PMember->StatusEffectContainer->AddStatusEffect(std::unique_ptr<CStatusEffect>(PEffect), EffectNotice::Silent);
                        }
                    }
                });
        }
        else if (auraTarget == AURA_TARGET::ENEMIES)
        {
            auto* PMob       = static_cast<CMobEntity*>(PEntity);
            auto* enmityList = PMob->PEnmityContainer->GetEnmityList();
            for (auto& enmityPair : *enmityList)
            {
                auto* PTarget = enmityPair.second.PEnmityOwner;
                if (PTarget != nullptr &&
                    PTarget->loc.zone &&
                    PEntity->loc.zone &&
                    PEntity->loc.zone->GetID() == PTarget->loc.zone->GetID() && distance(m_POwner->loc.p, PTarget->loc.p) <= aura_range + PTarget->modelHitboxSize &&
                    !PTarget->isDead())
                {
                    CStatusEffect* PEffect = PTarget->StatusEffectContainer->GetStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()));

                    const bool hasAlwaysExpiringFlag =
                        PEffect != nullptr &&
                        (PEffect->GetEffectFlags() & xi::StatusEffectFlag::AlwaysExpiring) != xi::StatusEffectFlag::None;
                    if (statuseffecthelpers::ShouldRefreshAlwaysExpiringAura(PEffect != nullptr, hasAlwaysExpiringFlag))
                    {
                        // Pure plan (slice 2798). Host owns timer refresh + Lua power update.
                        const auto plan = statuseffecthelpers::PlanAuraExistingAlwaysExpiring(
                            hasAlwaysExpiringFlag,
                            PEffect->GetPower(),
                            PStatusEffect->GetSubPower());

                        if (plan.refreshStartTime)
                        {
                            PEffect->SetStartTime(timer::now());
                        }

                        // Effect updated, probably from Ecliptic Attrition
                        // Update status effect with new potency.
                        // Take care to design your "owning" effects such as the xi::StatusEffect::ColureActive to control the subpower, rather than the resulting effect ticking down.
                        // Otherwise odd things may happen
                        if (plan.updatePower)
                        {
                            luautils::OnEffectLose(PTarget, PEffect);
                            PEffect->SetPower(PStatusEffect->GetSubPower());
                            luautils::OnEffectGain(PTarget, PEffect);
                        }
                    }
                    else
                    {
                        uint16 icon = statuseffecthelpers::ResolveAuraSubIcon(PStatusEffect->GetSubIcon(), PStatusEffect->GetSubID());

                        PEffect = new CStatusEffect(static_cast<xi::StatusEffect>(PStatusEffect->GetSubID()), // Effect ID
                                                    icon,                                                     // Effect Icon
                                                    PStatusEffect->GetSubPower(),                             // Power
                                                    3s,                                                       // Tick
                                                    4s);                                                      // Duration
                        PEffect->AddEffectFlag(xi::StatusEffectFlag::NoLossMessage);
                        PEffect->AddEffectFlag(xi::StatusEffectFlag::AlwaysExpiring);
                        PTarget->StatusEffectContainer->AddStatusEffect(std::unique_ptr<CStatusEffect>(PEffect), EffectNotice::Silent);
                    }
                }
            }
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Run OnEffectTick for all status effects                              *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::TickEffects(timer::time_point tick)
{
    if (m_POwner == nullptr)
    {
        ShowWarning("CStatusEffectContainer::TickRegen() - m_POwner is null.");
        return;
    }

    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    if (!m_POwner->isDead())
    {
        for (const auto& PStatusEffect : m_StatusEffectSet)
        {
            const auto tickPeriod = PStatusEffect->GetTickTime();
            const auto elapsedThreshold =
                tickPeriod != 0s ? static_cast<uint32>((tick - PStatusEffect->GetStartTime()) / tickPeriod) : 0u;
            if (statuseffecthelpers::ShouldTickEffect(
                    tickPeriod != 0s,
                    PStatusEffect->GetElapsedTickCount(),
                    elapsedThreshold))
            {
                if (PStatusEffect->HasEffectFlag(xi::StatusEffectFlag::Aura))
                {
                    HandleAura(PStatusEffect.get());
                }
                PStatusEffect->IncrementElapsedTickCount();
                luautils::OnEffectTick(m_POwner, PStatusEffect.get());
            }
        }
    }
    DeleteStatusEffects();
    m_POwner->PAI->EventHandler.triggerListener("EFFECTS_TICK", m_POwner);
}

/************************************************************************
 *                                                                       *
 *  Tick regen/refresh/regain effects                                    *
 *                                                                       *
 ************************************************************************/

void CStatusEffectContainer::TickRegen(timer::time_point tick)
{
    if (m_POwner == nullptr)
    {
        ShowWarning("CStatusEffectContainer::TickRegen() - m_POwner is null.");
        return;
    }

    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    if (!m_POwner->isDead())
    {
        CCharEntity* PChar = nullptr;
        if (m_POwner->objtype == TYPE_PC)
        {
            PChar = (CCharEntity*)m_POwner;
        }

        int16 regen   = m_POwner->getMod(Mod::REGEN);
        int16 poison  = m_POwner->getMod(Mod::REGEN_DOWN);
        int16 refresh = m_POwner->getMod(Mod::REFRESH) - m_POwner->getMod(Mod::REFRESH_DOWN);
        int16 regain  = m_POwner->getMod(Mod::REGAIN) - m_POwner->getMod(Mod::REGAIN_DOWN);
        m_POwner->addHP(regen);

        if (poison)
        {
            int16 damage = battleutils::HandleStoneskin(m_POwner, poison);

            if (damage > 0)
            {
                DelStatusEffectSilent(xi::StatusEffect::Healing);
                m_POwner->takeDamage(damage);

                // If target has nightmare sleep. Don't break sleep from REGEN_DOWN damage
                // see mobskills/nightmare.lua for full explanation
                {
                    const auto* sleepEffect = m_POwner->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::SleepI);
                    if (statuseffecthelpers::ShouldBreakSleepFromRegenDown(
                            sleepEffect != nullptr,
                            sleepEffect != nullptr ? sleepEffect->GetTier() : 0))
                    {
                        WakeUp();
                    }
                }
            }
        }

        // Final perpetuation = (Base / Half_Factor +- Reductions Or Penalties) * Avatar_Favor_Factor -> Minimum perpetuation is 1 except with 2Hour. Then refresh is applied.
        if (statuseffecthelpers::ShouldApplyAvatarPerpetuationPath(
                m_POwner->getMod(Mod::AVATAR_PERPETUATION),
                m_POwner->objtype == TYPE_PC))
        {
            int16 perpetuationCost = m_POwner->getMod(Mod::AVATAR_PERPETUATION);

            if (m_POwner->PPet != nullptr && PChar != nullptr)
            {
                CPetEntity* PPet          = (CPetEntity*)m_POwner->PPet;
                ELEMENT     petElement    = static_cast<ELEMENT>(PPet->m_Element);
                bool        elementValid  = petElement >= ELEMENT_FIRE && petElement <= ELEMENT_DARK; // Check if the element is not 0 (None) or out of bounds
                uint8       petElementIdx = 0;
                ELEMENT     dayElement    = battleutils::GetDayElement();
                auto        weather       = battleutils::GetWeather(PChar, false);

                if (!elementValid)
                {
                    ShowWarning("CStatusEffectContainer::TickRegen() - Pet %s (PetID %u) has invalid element %u for avatar perpetuation. Check pet_list.sql.",
                                PPet->getName(),
                                PPet->petID(),
                                PPet->m_Element);
                }
                else
                {
                    petElementIdx = static_cast<uint8>(petElement) - 1;
                }

                static const Mod     strong[8]        = { Mod::FIRE_AFFINITY_PERP, Mod::ICE_AFFINITY_PERP, Mod::WIND_AFFINITY_PERP, Mod::EARTH_AFFINITY_PERP, Mod::THUNDER_AFFINITY_PERP, Mod::WATER_AFFINITY_PERP, Mod::LIGHT_AFFINITY_PERP, Mod::DARK_AFFINITY_PERP };
                static const Weather weatherStrong[8] = { Weather::HotSpell, Weather::Snow, Weather::Wind, Weather::DustStorm, Weather::Thunder, Weather::Rain, Weather::Auroras, Weather::Gloom };

                // Day / Weather elemental matches.
                bool dayMatch     = elementValid && dayElement == petElement;
                bool weatherMatch = elementValid && statuseffecthelpers::WeatherMatchesPetStrong(
                                                        static_cast<uint16>(weather),
                                                        static_cast<uint16>(weatherStrong[petElementIdx]));

                // Halve perpetuation cost before all regular reductions.
                bool halfFromCarby   = PChar->getMod(Mod::HALF_PERPETUATION_CARBUNCLE) != 0 && PPet->petID() == PETID_CARBUNCLE;
                bool halfFromDay     = PChar->getMod(Mod::HALF_PERPETUATION_DAY) != 0 && dayMatch;
                bool halfFromWeather = PChar->getMod(Mod::HALF_PERPETUATION_WEATHER) != 0 && weatherMatch;

                perpetuationCost = statuseffecthelpers::ApplyHalfPerpetuation(
                    perpetuationCost,
                    halfFromCarby || halfFromDay || halfFromWeather);

                // Apply regular / elemental / day / weather perpetuation reductions.
                perpetuationCost = statuseffecthelpers::AdjustPerpetuationAfterHalf(
                    perpetuationCost,
                    PChar->getMod(Mod::PERPETUATION_REDUCTION),
                    elementValid ? PChar->getMod(strong[petElementIdx]) : static_cast<int16>(0),
                    dayMatch ? PChar->getMod(Mod::DAY_REDUCTION) : static_cast<int16>(0),
                    weatherMatch ? PChar->getMod(Mod::WEATHER_REDUCTION) : static_cast<int16>(0));

                // Avatar's Favor multiplier after all regular reductions.
                const bool applyFavor =
                    PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AvatarsFavor) &&
                    ((PPet->petID() >= PETID_CARBUNCLE && PPet->petID() <= PETID_CAIT_SITH) || PPet->petID() == PETID_SIREN);
                perpetuationCost = statuseffecthelpers::ApplyAvatarFavorPerpetuation(perpetuationCost, applyFavor);
            }

            // Astral Flow / min-1 clamp.
            perpetuationCost = statuseffecthelpers::FinalizePerpetuationCost(
                perpetuationCost,
                m_POwner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AstralFlow));

            m_POwner->addMP(refresh - perpetuationCost);

            if (m_POwner->PPet != nullptr && m_POwner->PPet->objtype == TYPE_PET)
            {
                CPetEntity* PPet = (CPetEntity*)m_POwner->PPet;
                if (statuseffecthelpers::ShouldDespawnAvatarOnZeroMP(
                        m_POwner->health.mp == 0,
                        true,
                        PPet->getPetType() == PET_TYPE::AVATAR))
                {
                    petutils::DespawnPet(m_POwner);
                }
            }
        }
        else
        {
            m_POwner->addMP(refresh);
        }

        if (statuseffecthelpers::ShouldApplyRegainTP(m_POwner->objtype == TYPE_MOB, m_POwner->PAI->IsEngaged()))
        {
            m_POwner->addTP(regain);
        }

        if (m_POwner->PPet)
        {
            if (auto* PAutomaton = dynamic_cast<CAutomatonEntity*>(m_POwner->PPet))
            {
                PAutomaton->burdenTick();
            }
        }
    }
}

bool CStatusEffectContainer::HasPreventActionEffect(bool ignoreCharm)
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (!PStatusEffect->isDeleted() &&
            statuseffecthelpers::IsPreventActionEffectID(static_cast<uint16>(PStatusEffect->GetStatusID()), ignoreCharm))
        {
            return true;
        }
    }
    return false;
}

uint16 CStatusEffectContainer::GetConfrontationEffect()
{
    for (const auto& PEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsConfrontationFlag(
                PEffect->HasEffectFlag(xi::StatusEffectFlag::Confrontation)))
        {
            return statuseffecthelpers::ConfrontationPowerOrZero(true, PEffect->GetPower());
        }
    }
    return 0;
}

void CStatusEffectContainer::CopyConfrontationEffect(CBattleEntity* PEntity)
{
    for (const auto& PEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::ShouldCopyConfrontation(
                PEffect->HasEffectFlag(xi::StatusEffectFlag::Confrontation)))
        {
            PEntity->StatusEffectContainer->AddStatusEffect(*PEffect);
        }
    }
}

bool CStatusEffectContainer::CheckForElevenRoll()
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsElevenRollEffect(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                PStatusEffect->GetSubPower(),
                statuseffecthelpers::ElevenRollIDFirst,
                statuseffecthelpers::ElevenRollIDLast,
                statuseffecthelpers::RuneistsRollID))
        {
            return true;
        }
    }
    return false;
}

bool CStatusEffectContainer::IsAsleep()
{
    // Pure membership: IsAsleepStatusID (slice 2825; alias of IsAsleepEffectID).
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (!PStatusEffect->isDeleted() &&
            statuseffecthelpers::IsAsleepStatusID(static_cast<uint16>(PStatusEffect->GetStatusID())))
        {
            return true;
        }
    }
    return false;
}

void CStatusEffectContainer::WakeUp()
{
    // Production dual-wires PlanWakeUp / WakeUpStatusIDs (slice 2820) for the
    // ordered SleepI → SleepIi → Lullaby DelStatusEffect sequence.
    for (const auto statusID : statuseffecthelpers::WakeUpStatusIDs())
    {
        DelStatusEffect(static_cast<xi::StatusEffect>(statusID));
    }
}

bool CStatusEffectContainer::HasBustEffect(uint16 id)
{
    for (const auto& PStatusEffect : m_StatusEffectSet)
    {
        if (statuseffecthelpers::IsBustEffectForAbility(
                static_cast<uint16>(PStatusEffect->GetStatusID()),
                PStatusEffect->GetSubPower(),
                static_cast<uint16>(xi::StatusEffect::Bust),
                id))
        {
            return true;
        }
    }
    return false;
}

auto CStatusEffectContainer::statusIcons() const -> const uint8*
{
    return m_StatusIcons;
}

auto CStatusEffectContainer::statusBits() const -> const uint64&
{
    return m_Flags;
}
