/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

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

#include "map_session_container.h"

#include "map_networking.h"
#include "map_session.h"
#include "map_session_container_capacity.h"
#include "status_effect_container.h"

#include "common/database.h"
#include "common/scheduler.h"
#include "common/xi.h"

#include "entities/char_entity.h"

#include "utils/charutils.h"
#include "utils/petutils.h"

void MapSessionIndex::addSession(MapSession* session)
{
    if (session != nullptr)
    {
        sessions_[session->client_ipp] = session;
    }
}

void MapSessionIndex::addPendingSession(MapSession* session)
{
    if (session != nullptr)
    {
        pendingSessions_[session->charID] = session;
    }
}

auto MapSessionIndex::getSessionByIPP(const IPP& ipp) const -> MapSession*
{
    if (const auto it = sessions_.find(ipp); it != sessions_.end())
    {
        return it->second;
    }
    return nullptr;
}

auto MapSessionIndex::getSessionByIPP(const uint64 ipp) const -> MapSession*
{
    return getSessionByIPP(IPP(ipp));
}

auto MapSessionIndex::getSessionByCharId(const uint32 charId) const -> MapSession*
{
    for (const auto& [_, session] : sessions_)
    {
        if (session != nullptr && session->charID == charId)
        {
            return session;
        }
    }
    return nullptr;
}

auto MapSessionIndex::getSessionByAccountId(const uint32 accountId) const -> MapSession*
{
    for (const auto& [_, session] : sessions_)
    {
        if (session != nullptr && session->accountID == accountId)
        {
            return session;
        }
    }
    return nullptr;
}

auto MapSessionIndex::getPendingSessionByCharId(const uint32 charId) const -> MapSession*
{
    if (const auto it = pendingSessions_.find(charId); it != pendingSessions_.end())
    {
        return it->second;
    }
    return nullptr;
}

auto MapSessionIndex::removeSession(MapSession* session) -> bool
{
    if (session == nullptr)
    {
        return false;
    }
    const auto it = sessions_.find(session->client_ipp);
    if (it == sessions_.end() || it->second != session)
    {
        return false;
    }
    sessions_.erase(it);
    return true;
}

auto MapSessionIndex::removePendingSession(MapSession* session) -> bool
{
    if (session == nullptr)
    {
        return false;
    }
    const auto it = pendingSessions_.find(session->charID);
    if (it == pendingSessions_.end() || it->second != session)
    {
        return false;
    }
    pendingSessions_.erase(it);
    return true;
}

auto MapSessionIndex::removePendingSession(const uint32 charId) -> MapSession*
{
    const auto it = pendingSessions_.find(charId);
    if (it == pendingSessions_.end())
    {
        return nullptr;
    }
    auto* session = it->second;
    pendingSessions_.erase(it);
    return session;
}

auto MapSessionIndex::confirmedSize() const -> std::size_t
{
    return sessions_.size();
}

auto MapSessionIndex::pendingSize() const -> std::size_t
{
    return pendingSessions_.size();
}

MapSessionContainer::MapSessionContainer(Scheduler& scheduler)
: scheduler_(scheduler)
{
}

// Go host pure half: mapsession.ApplyCreateSession (6421).
auto MapSessionContainer::createSession(IPP ipp) -> MapSession*
{
    TracyZoneScoped;

    ShowDebugFmt("Creating session for {}", ipp.getIPString());

    const auto rset                  = db::preparedStmt("SELECT charid FROM accounts_sessions WHERE client_addr = ? LIMIT 1", ipp.getIP());
    const bool queryOK               = static_cast<bool>(rset);
    const bool hasAccountsSessionRow = queryOK && rset->rowsCount() != 0;
    if (!mapsessionhelpers::ShouldCreateSession(queryOK, hasAccountsSessionRow))
    {
        if (!queryOK)
        {
            ShowError("SQL query failed in MapSessionContainer::createSession!");
        }
        else
        {
            // This is noisy and not really necessary
            DebugSocketsFmt("recv_parse: Invalid login attempt from {}", ipp.getIPString());
        }
        return nullptr;
    }

    auto map_session_data = std::make_unique<MapSession>();

    map_session_data->scheduler  = &scheduler_;
    map_session_data->client_ipp = ipp;
    map_session_data->tapLastUpdate();

    // Same-key index replace: pure gate; host owns removeSession.
    auto* previous = index_.getSessionByIPP(ipp);
    if (mapsessionhelpers::ShouldReplaceExistingSession(previous != nullptr))
    {
        index_.removeSession(previous);
    }
    sessions_[ipp] = std::move(map_session_data);
    index_.addSession(sessions_[ipp].get());

    return sessions_[ipp].get();
}

auto MapSessionContainer::createPendingSession(uint32 charId) -> MapSession*
{
    TracyZoneScoped;

    ShowDebugFmt("Creating pending session for character id {}", charId);

    const auto rset = db::preparedStmt("SELECT charid FROM accounts_sessions WHERE charid = ? LIMIT 1", charId);
    if (!mapsessionhelpers::ShouldCreatePendingSession(static_cast<bool>(rset)))
    {
        ShowError("SQL query failed in MapSessionContainer::createPendingSession");
        return nullptr;
    }

    auto map_session_data = std::make_unique<MapSession>();

    map_session_data->scheduler = &scheduler_;
    map_session_data->charID    = charId;
    map_session_data->tapLastUpdate();

    // Same-key index replace: pure gate; host owns removePendingSession.
    auto* previous = index_.getPendingSessionByCharId(charId);
    if (mapsessionhelpers::ShouldReplaceExistingSession(previous != nullptr))
    {
        index_.removePendingSession(previous);
    }
    pending_sessions_[charId] = std::move(map_session_data);
    index_.addPendingSession(pending_sessions_[charId].get());

    return pending_sessions_[charId].get();
}

auto MapSessionContainer::getSessionByIPP(IPP ipp) -> MapSession*
{
    TracyZoneScoped;

    return index_.getSessionByIPP(ipp);
}

auto MapSessionContainer::getSessionByIPP(uint64 ipp) -> MapSession*
{
    TracyZoneScoped;

    return index_.getSessionByIPP(ipp);
}

auto MapSessionContainer::getSessionByChar(CCharEntity* PChar) -> MapSession*
{
    TracyZoneScoped;

    if (mapsessionhelpers::ShouldRejectNullCharLookup(PChar == nullptr))
    {
        return nullptr;
    }

    for (const auto& [_, session] : sessions_)
    {
        const bool   sessionHasChar = session->PChar != nullptr;
        const uint32 sessionCharID  = sessionHasChar ? session->PChar->id : 0;
        if (mapsessionhelpers::SessionMatchesCharID(sessionHasChar, sessionCharID, PChar->id))
        {
            return session.get();
        }
    }

    return nullptr;
}

auto MapSessionContainer::getSessionByCharId(uint32 charId) -> MapSession*
{
    TracyZoneScoped;

    return index_.getSessionByCharId(charId);
}

auto MapSessionContainer::getPendingSessionByCharId(uint32 charId) -> MapSession*
{
    TracyZoneScoped;

    return index_.getPendingSessionByCharId(charId);
}

auto MapSessionContainer::getSessionByAccountId(uint32 accountId) -> MapSession*
{
    TracyZoneScoped;

    return index_.getSessionByAccountId(accountId);
}

auto MapSessionContainer::getSessionByCharName(const std::string& name) -> MapSession*
{
    TracyZoneScoped;

    for (const auto& [_, session] : sessions_)
    {
        const bool        sessionHasChar = session->PChar != nullptr;
        const std::string sessionName    = sessionHasChar ? session->PChar->name : std::string{};
        if (mapsessionhelpers::SessionMatchesCharName(sessionHasChar, sessionName, name))
        {
            return session.get();
        }
    }

    return nullptr;
}

// Go host pure half: mapsession.ApplyCleanupConfirmedSession / ApplyCleanupPendingSession (6420).
// Prior: ApplyTimeoutCleanupSQL / OtherMapFromServer (6417).
void MapSessionContainer::cleanupSessions(IPP mapIPP)
{
    TracyZoneScoped;

    auto timeoutSetting = settings::get<uint16>("map.MAX_TIME_LASTUPDATE");

    auto it = sessions_.begin();
    while (it != sessions_.end())
    {
        auto& map_session_data = it->second;

        auto* PChar = map_session_data->PChar.get();
        auto  now   = earth_time::now();

        if (now > map_session_data->last_update + 5s)
        {
            // Mark link-dead: pure gate + plan; host owns SQL / char / SpawnPCs.
            const bool hasChar         = PChar != nullptr;
            const bool alreadyLinkDead = hasChar && PChar->isLinkDead;
            if (mapsessionhelpers::ShouldMarkLinkDead(hasChar, alreadyLinkDead))
            {
                const auto plan = mapsessionhelpers::PlanLinkDeadMark(PChar->status == STATUS_TYPE::NORMAL);
                for (uint8 i = 0; i < plan.count; ++i)
                {
                    switch (plan.actions[i])
                    {
                        case mapsessionhelpers::LinkDeadTransitionAction::SetDisconnectingFlag:
                            db::preparedStmt("UPDATE char_flags SET disconnecting = 1 WHERE charid = ?", map_session_data->charID);
                            break;
                        case mapsessionhelpers::LinkDeadTransitionAction::SetLinkDead:
                            PChar->isLinkDead = true;
                            break;
                        case mapsessionhelpers::LinkDeadTransitionAction::SetUpdateHPMask:
                            PChar->updatemask |= UPDATE_HP;
                            break;
                        case mapsessionhelpers::LinkDeadTransitionAction::SpawnPCsIfNormal:
                            // Is this unintentionally sending extra packets when a player is disconnecting?
                            PChar->loc.zone->SpawnPCs(PChar);
                            break;
                        default:
                            break;
                    }
                }
            }

            if (now > map_session_data->last_update + std::chrono::seconds(timeoutSetting))
            {
                bool otherMap = false;

                // check if session is attached to a different map server...
                // Host owns this SQL lookup; pure plan only consumes the bool.
                auto rset = db::preparedStmt("SELECT server_addr, server_port FROM accounts_sessions WHERE charid = ?", map_session_data->charID);
                if (rset && rset->rowsCount() && rset->next())
                {
                    uint32 server_addr = rset->get<uint32>("server_addr");
                    uint32 server_port = rset->get<uint32>("server_port");

                    // s_addr of 0 is single process map server without IP address set explicitly in commandline
                    // map_port is 0 without the port being explicitly set in commandline
                    if ((mapIPP.getIP() != 0 && server_addr != mapIPP.getIP()) || (mapIPP.getPort() != 0 && server_port != mapIPP.getPort()))
                    {
                        otherMap = true;
                    }
                }

                // Confirmed-session timeout: pure plan; host owns SQL / char /
                // pet / zone / index / erase. Logging remains host-side.
                if (PChar != nullptr)
                {
                    ShowDebugFmt("Clearing map server session for player: '{}' in zone: '{}' (On other map server = {})", PChar->name, PChar->loc.zone ? PChar->loc.zone->getName() : "None", otherMap ? "Yes" : "No");
                }
                else
                {
                    ShowWarning("map_cleanup: WITHOUT CHAR timed out, session closed on this process");
                }

                const mapsessionhelpers::TimeoutDecision timeoutInput{
                    .hasCharacter = PChar != nullptr,
                    .otherMap     = otherMap,
                    .hasMobPet    = PChar != nullptr && PChar->PPet != nullptr && PChar->PPet->objtype == TYPE_MOB,
                    .shuttingDown = map_session_data->shuttingDown,
                };
                const auto plan = mapsessionhelpers::PlanTimeoutCleanup(timeoutInput);
                for (uint8 i = 0; i < plan.count; ++i)
                {
                    switch (plan.actions[i])
                    {
                        // Go host pure half: mapsession.ApplyTimeoutCleanupEffects (6418).
                        case mapsessionhelpers::CleanupAction::SaveStatusEffects:
                            map_session_data->PChar->StatusEffectContainer->SaveStatusEffects(true);
                            break;
                        case mapsessionhelpers::CleanupAction::DeleteDatabaseSession:
                            db::preparedStmt("DELETE FROM accounts_sessions WHERE charid = ?", map_session_data->charID);
                            break;
                        case mapsessionhelpers::CleanupAction::SaveCharacterPosition:
                            charutils::SaveCharPosition(PChar);
                            break;
                        case mapsessionhelpers::CleanupAction::DespawnMobPet:
                            petutils::DespawnPet(PChar);
                            break;
                        case mapsessionhelpers::CleanupAction::SetCharacterShutdown:
                            PChar->status = STATUS_TYPE::SHUTDOWN;
                            break;
                        case mapsessionhelpers::CleanupAction::RemoveCharacterFromZone:
                            charutils::removeCharFromZone(PChar);
                            break;
                        case mapsessionhelpers::CleanupAction::ReleaseCharacter:
                            map_session_data->PChar.reset();
                            break;
                        case mapsessionhelpers::CleanupAction::RemoveSessionIndex:
                            index_.removeSession(map_session_data.get());
                            break;
                        case mapsessionhelpers::CleanupAction::EraseSession:
                            sessions_.erase(it++);
                            break;
                        default:
                            break;
                    }
                }

                continue;
            }
        }
        else if (mapsessionhelpers::ShouldRecoverLinkDead(PChar != nullptr, PChar != nullptr && PChar->isLinkDead))
        {
            // Recover link-dead: pure gate + plan; host owns SQL / char / SpawnPCs / SaveCharStats.
            const auto plan = mapsessionhelpers::PlanLinkDeadRecover(PChar->status == STATUS_TYPE::NORMAL);
            for (uint8 i = 0; i < plan.count; ++i)
            {
                switch (plan.actions[i])
                {
                    case mapsessionhelpers::LinkDeadTransitionAction::ClearDisconnectingFlag:
                        db::preparedStmt("UPDATE char_flags SET disconnecting = 0 WHERE charid = ?", map_session_data->charID);
                        break;
                    case mapsessionhelpers::LinkDeadTransitionAction::ClearLinkDead:
                        PChar->isLinkDead = false;
                        break;
                    case mapsessionhelpers::LinkDeadTransitionAction::SetUpdateHPMask:
                        PChar->updatemask |= UPDATE_HP;
                        break;
                    case mapsessionhelpers::LinkDeadTransitionAction::SpawnPCsIfNormal:
                        PChar->loc.zone->SpawnPCs(PChar);
                        break;
                    case mapsessionhelpers::LinkDeadTransitionAction::SaveCharStats:
                        charutils::SaveCharStats(PChar);
                        break;
                    default:
                        break;
                }
            }
        }
        ++it;
    }

    std::erase_if(
        pending_sessions_,
        [&](auto& pair)
        {
            auto& map_session_data = pair.second;

            auto now = earth_time::now();

            if (now > map_session_data->last_update + std::chrono::seconds(timeoutSetting))
            {
                ShowDebugFmt("Clearing map server pending session for pending char ID: '{}'", map_session_data->charID);

                // Pending timeout erase: pure plan; host owns SQL / index / erase.
                const auto plan = mapsessionhelpers::PlanPendingTimeoutCleanup();
                bool       erase = false;
                for (uint8 i = 0; i < plan.count; ++i)
                {
                    switch (plan.actions[i])
                    {
                        case mapsessionhelpers::PendingTimeoutCleanupAction::DeleteDatabaseSession:
                            db::preparedStmt("DELETE FROM accounts_sessions WHERE charid = ?", map_session_data->charID);
                            break;
                        case mapsessionhelpers::PendingTimeoutCleanupAction::RemovePendingIndex:
                            index_.removePendingSession(map_session_data.get());
                            break;
                        case mapsessionhelpers::PendingTimeoutCleanupAction::ErasePending:
                            erase = true;
                            break;
                        default:
                            break;
                    }
                }
                return erase;
            }

            return false; // Keep
        });
}

void MapSessionContainer::destroySession(IPP ipp)
{
    TracyZoneScoped;

    if (auto map_session_data = getSessionByIPP(ipp))
    {
        destroySession(map_session_data);
    }
}

// Go host pure half: mapsession.ApplyDestroySession (6419).
void MapSessionContainer::destroySession(MapSession* map_session_data)
{
    TracyZoneScoped;

    if (map_session_data == nullptr)
    {
        return;
    }

    // Refuse stale or foreign pointers so a replacement session cannot be
    // erased through the owning map while the index still points elsewhere.
    // Index removal happens before PlanDestroy so planning is identity-safe
    // remaining effects only (RemoveSessionIndex is not part of the plan).
    if (!index_.removeSession(map_session_data))
    {
        return;
    }

    ShowDebugFmt("Closing session for {}", map_session_data->client_ipp.toString());

    // Remaining destroy effects: pure plan; host owns SQL / zone / char / erase.
    const mapsessionhelpers::DestroyDecision destroyInput{
        .hasCharacter = map_session_data->PChar != nullptr,
        .hasZone      = map_session_data->PChar != nullptr && map_session_data->PChar->loc.zone != nullptr,
        .shuttingDown = map_session_data->shuttingDown,
    };
    const auto plan = mapsessionhelpers::PlanDestroy(destroyInput);
    for (uint8 i = 0; i < plan.count; ++i)
    {
        switch (plan.actions[i])
        {
            case mapsessionhelpers::CleanupAction::DeleteDatabaseSession:
                // clear accounts_sessions if character is logging out (not when zoning)
                db::preparedStmt("DELETE FROM accounts_sessions WHERE charid = ?", map_session_data->charID);
                break;
            case mapsessionhelpers::CleanupAction::DecreaseZoneCounter:
                // This should already be done in removeCharFromZone, but just to be safe...
                map_session_data->PChar->loc.zone->DecreaseZoneCounter(map_session_data->PChar.get());
                break;
            case mapsessionhelpers::CleanupAction::ReleaseCharacter:
                map_session_data->PChar.reset();
                break;
            case mapsessionhelpers::CleanupAction::EraseSession:
                sessions_.erase(map_session_data->client_ipp);
                break;
            default:
                break;
        }
    }
}

void MapSessionContainer::destroyPendingSession(MapSession* map_session_data)
{
    TracyZoneScoped;

    if (map_session_data == nullptr)
    {
        return;
    }

    // Lookup + pure gate; host owns index erase and ownership-map delete.
    auto*      current        = index_.getPendingSessionByCharId(map_session_data->charID);
    const bool found          = current != nullptr;
    const bool pointerMatches = current == map_session_data;
    if (!mapsessionhelpers::ShouldDestroyPendingByPointer(found, pointerMatches))
    {
        return;
    }

    index_.removePendingSession(map_session_data);

    ShowDebugFmt("Closing pending session for character id {}", map_session_data->charID);

    pending_sessions_.erase(map_session_data->charID);
}

void MapSessionContainer::destroyPendingSession(uint32 charId)
{
    TracyZoneScoped;

    // Lookup + pure gate; host owns index erase and ownership-map delete.
    const bool found = index_.getPendingSessionByCharId(charId) != nullptr;
    if (!mapsessionhelpers::ShouldDestroyPendingByCharID(found))
    {
        return;
    }

    index_.removePendingSession(charId);

    ShowDebugFmt("Closing pending session for character id {}", charId);

    pending_sessions_.erase(charId);
}
