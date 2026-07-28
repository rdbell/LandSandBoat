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

#include <algorithm>
#include <cstring>

#include "ai/states/death_state.h"

#include "alliance.h"

#include "battlefield.h"
#include "battlefield_handler.h"
#include "battlefield_handler_load.h"
#include "battlefield_handler_registration.h"

#include "entities/battle_entity.h"
#include "entities/char_entity.h"

#include "lua/luautils.h"

#include "packets/s2c/0x119_abil_recast.h"

#include "status_effect.h"
#include "status_effect_container.h"

#include "utils/charutils.h"
#include "utils/zoneutils.h"

#include "zone.h"

CBattlefieldHandler::CBattlefieldHandler(CZone* PZone)
: m_PZone(PZone)
, m_MaxBattlefields(luautils::OnBattlefieldHandlerInitialize(PZone))
{
}

CBattlefieldHandler::~CBattlefieldHandler() = default;

void CBattlefieldHandler::HandleBattlefields(timer::time_point tick)
{
    TracyZoneScoped;

    for (auto& [area, PBattlefield] : m_Battlefields)
    {
        if (!PBattlefield->CanCleanup())
        {
            PBattlefield->onTick(tick);
        }
    }

    for (auto it = m_Battlefields.begin(); it != m_Battlefields.end();)
    {
        auto* PBattlefield = it->second.get();
        if (PBattlefield->CanCleanup())
        {
            if (PBattlefield->Cleanup(tick, false))
            {
                ShowDebug("[CBattlefieldHandler]HandleBattlefields cleaned up Battlefield %s", PBattlefield->GetName().c_str());
                it = m_Battlefields.erase(it);
                continue;
            }
        }

        ++it;
    }

    for (auto iter = m_orphanedPlayers.begin(); iter != m_orphanedPlayers.end();)
    {
        if (tick < (*iter).second)
        {
            ++iter;
            continue;
        }

        auto* PChar = m_PZone->GetCharByID((*iter).first);
        if (PChar)
        {
            luautils::OnBattlefieldKick(PChar);
            PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Confrontation, EffectNotice::Silent);
            m_PZone->updateCharLevelRestriction(PChar);
        }
        iter = m_orphanedPlayers.erase(iter);
    }
}

uint8 CBattlefieldHandler::LoadBattlefield(CCharEntity* PChar, const BattlefieldRegistration& registration)
{
    TracyZoneScoped;

    const auto areaOccupied = m_Battlefields.contains(registration.area);
    const auto plan         = battlefieldhandlerhelpers::PlanLoadAdmission(
        PChar->PBattlefield != nullptr,
        m_Battlefields.size() >= m_MaxBattlefields,
        areaOccupied,
        registration.id == 0xFFFF);

    if (plan.action == battlefieldhandlerhelpers::LoadAction::Return)
    {
        return plan.returnCode;
    }

    auto battlefield = std::make_unique<CBattlefield>(registration.id, m_PZone, registration.area, PChar);

    const auto rset = db::preparedStmt("SELECT name, fastestName, fastestTime, fastestPartySize "
                                       "FROM bcnm_records "
                                       "WHERE bcnmId = ?",
                                       registration.id);

    if (!rset || rset->rowsCount() == 0 || !rset->next())
    {
        ShowError("Cannot load battlefield : %u ", registration.id);
        return battlefieldhandlerhelpers::PlanLoadRecordResult(false);
    }

    const auto name            = rset->get<std::string>("name");
    const auto recordholder    = rset->get<std::string>("fastestName");
    const auto recordtime      = std::chrono::seconds(rset->get<uint32>("fastestTime"));
    const auto recordPartySize = rset->get<size_t>("fastestPartySize");

    battlefield->SetName(name);
    battlefield->SetRecord(recordholder, recordtime, recordPartySize);
    battlefield->SetTimeLimit(registration.timeLimit);
    battlefield->SetLevelCap(registration.levelCap);
    battlefield->SetMaxParticipants(registration.maxPlayers);
    battlefield->SetRuleMask(registration.rules);
    battlefield->m_isMission = registration.isMission;
    battlefield->m_showTimer = registration.showTimer;

    const auto area = battlefield->GetArea();
    m_Battlefields.insert(std::make_pair(area, std::move(battlefield)));
    auto* PBattlefield = m_Battlefields[area].get();

    if (!PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Battlefield))
    {
        PChar->StatusEffectContainer->AddStatusEffectSilent(
            xi::StatusEffect::Battlefield, static_cast<uint16>(xi::StatusEffect::Battlefield), PBattlefield->GetID(), 0s, 0s, PChar->id, PBattlefield->GetArea());
    }

    luautils::OnBattlefieldRegister(PChar, PBattlefield);
    luautils::OnBattlefieldInitialize(PBattlefield);
    PBattlefield->InsertEntity(PChar, true);

    return battlefieldhandlerhelpers::PlanLoadRecordResult(true);
}

CBattlefield* CBattlefieldHandler::GetBattlefield(CBaseEntity* PEntity, bool checkRegistered)
{
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);

    if (checkRegistered && entity && entity->objtype == TYPE_PC)
    {
        for (auto& [area, battlefield] : m_Battlefields)
        {
            if (battlefield->IsRegistered(static_cast<CCharEntity*>(entity)))
            {
                return battlefield.get();
            }
        }
        return nullptr;
    }

    for (auto& [area, battlefield] : m_Battlefields)
    {
        if (battlefield->GetEntity(entity))
        {
            return battlefield.get();
        }
    }
    return nullptr;
}

CBattlefield* CBattlefieldHandler::GetBattlefieldByArea(uint8 area) const
{
    const auto it = m_Battlefields.find(area);
    return it != m_Battlefields.end() ? it->second.get() : nullptr;
}

CBattlefield* CBattlefieldHandler::GetBattlefieldByInitiator(uint32 charID)
{
    for (auto& [area, battlefield] : m_Battlefields)
    {
        if (battlefield->GetInitiator().id == charID)
        {
            return battlefield.get();
        }
    }
    return nullptr;
}

uint8 CBattlefieldHandler::RegisterBattlefield(CCharEntity* PChar, const BattlefieldRegistration& registration)
{
    // attempt to add to an existing battlefield
    auto* PBattlefield = GetBattlefield(PChar, true);

    // Could not find this character registered, try find by id and initiator
    if (!PBattlefield)
    {
        for (const auto& [area, battlefield] : m_Battlefields)
        {
            if (battlefield->GetInitiator().id == registration.initiator && battlefield->GetID() == registration.id)
            {
                PBattlefield = battlefield.get();
                break;
            }
        }
    }

    const auto plan = battlefieldhandlerhelpers::PlanRegistration(
        PChar->PBattlefield != nullptr,
        PBattlefield != nullptr,
        PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Battlefield),
        PChar->GetLocalVar("[BCNM]EnterExisting") == 1,
        PBattlefield && PBattlefield->CheckInProgress(),
        PBattlefield && registration.area == PBattlefield->GetArea());

    if (plan.resetEnterExisting)
    {
        PChar->SetLocalVar("[BCNM]EnterExisting", 0);
    }

    switch (plan.action)
    {
        case battlefieldhandlerhelpers::RegistrationAction::Return:
            if (PChar->PBattlefield)
            {
                ShowDebug("%s tried to enter another battlefield", PChar->getName());
            }
            return plan.returnCode;
        case battlefieldhandlerhelpers::RegistrationAction::InsertExisting:
            return PBattlefield->InsertEntity(PChar, false) ? BATTLEFIELD_RETURN_CODE_CUTSCENE : BATTLEFIELD_RETURN_CODE_BATTLEFIELD_FULL;
        case battlefieldhandlerhelpers::RegistrationAction::LoadNew:
            return LoadBattlefield(PChar, registration);
    }

    return BATTLEFIELD_RETURN_CODE_WAIT;
}

bool CBattlefieldHandler::RemoveFromBattlefield(CBaseEntity* PEntity, CBattlefield* PBattlefield, uint8 leavecode)
{
    PBattlefield = PBattlefield ? PBattlefield : GetBattlefield(PEntity);
    return PBattlefield ? PBattlefield->RemoveEntity(PEntity, leavecode) : false;
}

bool CBattlefieldHandler::IsRegistered(CCharEntity* PChar)
{
    for (const auto& [area, battlefield] : m_Battlefields)
    {
        if (battlefield->IsRegistered(PChar))
        {
            return true;
        }
    }
    return false;
}

bool CBattlefieldHandler::ReachedMaxCapacity(int battlefieldId) const
{
    // area all areas full
    if (m_Battlefields.size() >= (size_t)m_MaxBattlefields)
    {
        return true;
    }

    // NOTE: If allowedAreas is used for a BCNM, this check will return true, but instead
    // the player will be rejected from the instance in the 32000 event update.  This is intentional
    // at this time.

    // we have a free battlefield
    return false;
}

uint8 CBattlefieldHandler::MaxBattlefieldAreas() const
{
    return m_MaxBattlefields;
}

void CBattlefieldHandler::addOrphanedPlayer(CCharEntity* PChar)
{
    auto orphan = std::make_pair(PChar->id, timer::now() + 5s);
    m_orphanedPlayers.emplace_back(orphan);
}
