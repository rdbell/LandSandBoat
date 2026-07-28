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

#include "packets/s2c/0x057_weather.h"

namespace
{

constexpr std::uint16_t WeatherCycle = 2160;

}

// TODO:
// It is necessary to divide the CZone class into basic and heirs. Already painted: Standard, Resident, Instance and Dynamis
// Each of these zones has special behavior

#include "zone.h"
#include "zone_in_battlefield.h"

#include "trigger_area_dispatch.h"
#include "zone_capacity.h"
#include "zone_query_by_name_capacity.h"
#include "zone_mesh_name.h"
#include "zone_weather_decoder.h"

#include "common/logging.h"
#include "common/settings.h"
#include "common/timer.h"
#include "common/utils.h"
#include "common/vana_time.h"

#include <cstring>
#include <filesystem>

#include "battlefield.h"
#include "enums/loot_recast.h"
#include "ipc_client.h"
#include "latent_effect_container.h"
#include "map/navmesh/navmesh.h"
#include "map/navmesh/navmesh_builder.h"
#include "map_engine.h"
#include "monstrosity.h"
#include "nominate_manager.h"
#include "party.h"
#include "recast_container.h"
#include "spawn_handler.h"
#include "status_effect_container.h"
#include "treasure_pool.h"
#include "zone_entities.h"

#include "entities/npc_entity.h"
#include "entities/pet_entity.h"

#include "lua/luautils.h"

#include "utils/battleutils.h"
#include "utils/charutils.h"
#include "utils/moduleutils.h"

#include <map/ximesh/ximesh.h>

CZone::CZone(Scheduler& scheduler, MapConfig config, ZONEID ZoneID, REGION_TYPE RegionID, CONTINENT_TYPE ContinentID, uint8 levelRestriction)
: scheduler_(scheduler)
, config_(config)
, navMesh_{ std::make_unique<NullNavMesh>() }
, xiMesh_{ std::make_unique<NullXiMesh>() }
, m_zoneID(ZoneID)
, m_zoneType(ZONE_TYPE::UNKNOWN)
, m_regionID(RegionID)
, m_continentID(ContinentID)
, m_levelRestriction(levelRestriction)
{
    TracyZoneScoped;

    m_TreasurePool       = nullptr;
    m_BattlefieldHandler = nullptr;
    m_zoneEntities       = new CZoneEntities(scheduler_, config_, this);
    m_CampaignHandler    = new CCampaignHandler(this);
    m_spawnHandler       = std::make_unique<SpawnHandler>(this);
    nominateManager_     = std::make_unique<NominateManager>(*this);

    // settings should load first
    LoadZoneSettings();

    LoadZoneLines();
    LoadZoneWeather();

    if (config_.isTestServer)
    {
        return;
    }

    // This must run continually, regardless of if the zone is awake
    spawnHandlerTimerToken_ = scheduler.intervalOnMainThread(
        kSpawnHandlerInterval,
        [this]() -> Task<void>
        {
            this->spawnHandler().Tick(timer::now());
            co_return;
        });
}

CZone::~CZone()
{
    TracyZoneScoped;

    destroy(m_TreasurePool);
    destroy(m_zoneEntities);
    destroy(m_BattlefieldHandler);

    if (m_CampaignHandler)
    {
        destroy(m_CampaignHandler);
    }

    m_triggerAreaList.clear();

    for (auto zoneLine : m_zoneLineList)
    {
        destroy(zoneLine);
    }
    m_zoneLineList.clear();
}

auto CZone::GetID() const -> ZONEID
{
    return m_zoneID;
}

ZONE_TYPE CZone::GetTypeMask()
{
    return m_zoneType;
}

REGION_TYPE CZone::GetRegionID()
{
    return m_regionID;
}

CONTINENT_TYPE CZone::GetContinentID()
{
    return m_continentID;
}

uint8 CZone::getLevelRestriction()
{
    return m_levelRestriction;
}

uint32 CZone::GetIP() const
{
    return m_zoneIP;
}

uint16 CZone::GetPort() const
{
    return m_zonePort;
}

uint16 CZone::GetTax() const
{
    return m_tax;
}

auto CZone::weather() -> WeatherContainer&
{
    return weather_;
}

auto CZone::weather() const -> const WeatherContainer&
{
    return weather_;
}

auto CZone::spawnHandler() const -> SpawnHandler&
{
    return *m_spawnHandler;
}

auto CZone::nominateManager() const -> NominateManager&
{
    return *nominateManager_;
}

auto CZone::campaignHandler() const -> CCampaignHandler*
{
    return m_CampaignHandler;
}

auto CZone::battlefieldHandler() const -> CBattlefieldHandler*
{
    return m_BattlefieldHandler;
}

const std::string& CZone::getName()
{
    return m_zoneName;
}

uint16 CZone::GetSoloBattleMusic() const
{
    return m_zoneMusic.m_bSongS;
}

uint16 CZone::GetPartyBattleMusic() const
{
    return m_zoneMusic.m_bSongM;
}

uint16 CZone::GetBackgroundMusicDay() const
{
    return m_zoneMusic.m_songDay;
}

uint16 CZone::GetBackgroundMusicNight() const
{
    return m_zoneMusic.m_songNight;
}

void CZone::SetSoloBattleMusic(uint16 music)
{
    m_zoneMusic.m_bSongS = music;
}

void CZone::SetPartyBattleMusic(uint16 music)
{
    m_zoneMusic.m_bSongM = music;
}

void CZone::SetBackgroundMusicDay(uint16 music)
{
    m_zoneMusic.m_songDay = music;
}

void CZone::SetBackgroundMusicNight(uint16 music)
{
    m_zoneMusic.m_songNight = music;
}

/**
 * Queries for entities (mobs or npcs) which name match the given pattern.
 *
 * @param pattern The pattern used to match the entity name. We use % as wildcard for consistency
 * with other methods that perform pattern matching.
 * E.g: %anto% matches Shantotto and Canto-anto
 */
const QueryByNameResult_t& CZone::queryEntitiesByName(const std::string& pattern)
{
    TracyZoneScoped;

    const auto plan = zonequeryhelpers::PlanQueryByName(pattern, m_queryByNameResults.contains(pattern));
    if (plan.returnCached)
    {
        return m_queryByNameResults.at(pattern);
    }

    std::vector<CBaseEntity*> entities;

    // TODO: Make work for instances
    zonequeryhelpers::ResolveMatches(
        [this](const auto& callback)
        {
            ForEachNpc(callback);
        },
        [this](const auto& callback)
        {
            ForEachMob(callback);
        },
        [&](const auto* entity)
        {
            return matches(entity->getName(), pattern);
        },
        [&](auto* entity)
        {
            entities.emplace_back(entity);
        });

    if (plan.cacheResult)
    {
        m_queryByNameResults[pattern] = std::move(entities);
    }
    return m_queryByNameResults[pattern];
}

uint32 CZone::GetLocalVar(const char* var)
{
    return localVars_[var];
}

std::unordered_map<std::string, uint32>& CZone::GetLocalVars()
{
    return localVars_;
}

void CZone::SetLocalVar(const char* var, uint32 val)
{
    localVars_[var] = val;
}

void CZone::ResetLocalVars()
{
    localVars_.clear();
}

bool CZone::CanUseMisc(uint16 misc) const
{
    return zonehelpers::CanUseMisc(m_miscMask, misc);
}

zoneLine_t* CZone::GetZoneLine(uint32 zoneLineID)
{
    for (const auto& zoneLine : m_zoneLineList)
    {
        if (zoneLine->zoneLineId == zoneLineID)
        {
            return zoneLine;
        }
    }
    return nullptr;
}

// Spawns players across 8 fixed slots along the target zoneline area.
// Spacing is calculated from the documented invisible box representing the zoneline
auto zoneLine_t::nextSpawnPosition() -> position_t
{
    const float scale    = std::max(destinationScaleX, destinationScaleZ); // Spawn area length
    const float spacing  = (scale - 1.0f) / 8.0f;                          // Distance between slots
    const float offset   = (m_spawnSlot - 4) * spacing;                    // Offset from center (slot 4)
    const float rotation = rotationToRadian(destinationPos.rotation);      // Direction to apply offset

    m_spawnSlot = (m_spawnSlot + 1) % 8;

    return {
        destinationPos.x + offset * std::sin(rotation),
        destinationPos.y,
        destinationPos.z + offset * std::cos(rotation),
        0,
        destinationPos.rotation,
    };
}

void CZone::LoadZoneLines()
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT zonelineid, from_zone, from_pos_x, from_pos_y, from_pos_z, "
                                       "to_zone, to_pos_x, to_pos_y, to_pos_z, to_scale_x, to_scale_z, to_rotation "
                                       "FROM zonelines "
                                       "WHERE from_zone = ?",
                                       m_zoneID);
    FOR_DB_MULTIPLE_RESULTS(rset)
    {
        auto* zl = new zoneLine_t;

        zl->zoneLineId              = rset->get<uint32>("zonelineid");
        zl->originZoneId            = rset->get<ZONEID>("from_zone");
        zl->originPos.x             = rset->get<float>("from_pos_x");
        zl->originPos.y             = rset->get<float>("from_pos_y");
        zl->originPos.z             = rset->get<float>("from_pos_z");
        zl->destinationZoneId       = rset->get<ZONEID>("to_zone");
        zl->destinationPos.x        = rset->get<float>("to_pos_x");
        zl->destinationPos.y        = rset->get<float>("to_pos_y");
        zl->destinationPos.z        = rset->get<float>("to_pos_z");
        zl->destinationPos.rotation = radianToRotation(rset->get<float>("to_rotation"));
        zl->destinationScaleX       = rset->get<float>("to_scale_x");
        zl->destinationScaleZ       = rset->get<float>("to_scale_z");

        m_zoneLineList.emplace_back(zl);
    }
}

/*************************************************************************
 *                                                                        *
 *  Loads weather for the zone from zone_bweather SQL Table               *
 *                                                                        *
 *  Weather is a rotating pattern of 2160 vanadiel days for each zone.    *
 *  It's stored as a blob of 2160 16-bit values, each representing 1 day  *
 *  starting from day 0 and storing 3 5-bit weather values each.          *
 *                                                                        *
 *              0        00000       00000        00000                   *
 *              ^        ^^^^^       ^^^^^        ^^^^^                   *
 *          padding      normal      common       rare                    *
 *                                                                        *
 *************************************************************************/

void CZone::LoadZoneWeather()
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT weather "
                                       "FROM zone_weather "
                                       "WHERE zone = ? LIMIT 1",
                                       m_zoneID);
    FOR_DB_SINGLE_RESULT(rset)
    {
        uint16_t weatherBlob[WeatherCycle]{};

        db::extractFromBlob(rset, "weather", weatherBlob);
        zoneweatherhelpers::DecodeBlob(weatherBlob, [this](const uint16 day, const ZoneWeather entry) {
            weather_.addEntry(day, entry);
        });
    }
}

void CZone::LoadZoneSettings()
{
    TracyZoneScoped;

    const auto rset = db::preparedStmt("SELECT "
                                       "zone.name,"
                                       "zone.zoneip,"
                                       "zone.zoneport,"
                                       "zone.music_day,"
                                       "zone.music_night,"
                                       "zone.battlesolo,"
                                       "zone.battlemulti,"
                                       "zone.tax,"
                                       "zone.misc,"
                                       "zone.zonetype,"
                                       "bcnm.name AS bcnmname "
                                       "FROM zone_settings AS zone "
                                       "LEFT JOIN bcnm_records AS bcnm "
                                       "USING (zoneid) "
                                       "WHERE zoneid = ? "
                                       "LIMIT 1",
                                       m_zoneID);
    FOR_DB_SINGLE_RESULT(rset)
    {
        m_zoneName.insert(0, rset->get<std::string>("name"));
        m_zoneIP   = str2ip(rset->get<std::string>("zoneip"));
        m_zonePort = rset->get<uint16>("zoneport");

        m_zoneMusic.m_songDay   = rset->get<uint8>("music_day");
        m_zoneMusic.m_songNight = rset->get<uint8>("music_night");
        m_zoneMusic.m_bSongS    = rset->get<uint8>("battlesolo");
        m_zoneMusic.m_bSongM    = rset->get<uint8>("battlemulti");
        m_tax                   = static_cast<uint16>(rset->get<float>("tax") * 100); // tax for bazaar
        m_miscMask              = rset->get<uint16>("misc");
        m_zoneType              = rset->get<ZONE_TYPE>("zonetype");

        if (rset->getOrDefault<std::string>("bcnmname", "") != "") // bcnmid cannot be used now, because they start from scratch
        {
            m_BattlefieldHandler = new CBattlefieldHandler(this);
        }

        if (m_miscMask & MISC_TREASURE)
        {
            m_TreasurePool = new CTreasurePool(TreasurePoolType::Zone);
        }

        if (m_CampaignHandler && m_CampaignHandler->m_PZone == nullptr)
        {
            destroy(m_CampaignHandler);
        }
    }
}

// Go host pure half: zone.ApplyLoadNavMesh / PlanLoadNavMesh (slice 6410);
// full file-detour + rebuild-install composition: ApplyLoadNavMeshFull (6437).
auto CZone::LoadNavMesh() -> Task<void>
{
    auto       navMesh = std::make_unique<CNavMesh>(static_cast<uint16>(GetID()));
    const auto file    = fmt::format("navmeshes/{}.nav", getName());

    if (!config_.rebuildNavmeshes && navMesh->load(file))
    {
        navMesh_ = std::move(navMesh);
        co_return;
    }

    NavMeshBuilder builder(*xiMesh_);

    auto* dtNavMesh = co_await builder.buildAsync(scheduler_, getName(), static_cast<uint16>(GetID()), NavMeshConfig{});
    if (dtNavMesh && navMesh->installNavMesh(dtNavMesh))
    {
        navMesh->save(file);
        navMesh_ = std::move(navMesh);
        co_return;
    }

    DebugNavmesh("CZone::LoadNavMesh: Build failed for zone (%s)", getName().c_str());
}

// Go host pure half: zone.ApplyRebuildNavMesh / ShouldInstallBuiltMesh (6436).
void CZone::RebuildNavMesh(const NavMeshConfig& config)
{
    const auto  zoneName  = getName();
    const auto  zoneID    = static_cast<uint16>(GetID());
    const auto* xiMeshPtr = xiMesh_.get();

    scheduler_.postToMainThread(
        [this, zoneName, zoneID, config, xiMeshPtr]() -> Task<void>
        {
            NavMeshBuilder builder(*xiMeshPtr);

            auto* dtNavMesh = co_await builder.buildAsync(scheduler_, zoneName, zoneID, config);
            auto  navMesh   = std::make_unique<CNavMesh>(zoneID);
            if (dtNavMesh && navMesh->installNavMesh(dtNavMesh))
            {
                navMesh->save(fmt::format("navmeshes/{}.nav", zoneName));
                navMesh_ = std::move(navMesh);
            }
        });
}

auto CZone::navMesh() const -> INavMesh*
{
    return navMesh_.get();
}

auto CZone::xiMesh() const -> IXiMesh*
{
    return xiMesh_.get();
}

// Go host pure half: zone.ApplyLoadXiMesh / PlanLoadXiMesh (slice 6410).
void CZone::LoadXiMesh()
{
    TracyZoneScoped;

    // TODO: Align ximesh filenames with zone_settings names so this isn't needed.
    const auto meshName = zonemeshhelpers::ResolveXiMeshName(getName());

    const auto file = fmt::format("ximeshes/{}.ximesh", meshName);
    if (std::filesystem::exists(file))
    {
        try
        {
            xiMesh_ = std::make_unique<XiMesh>(file);
        }
        catch (const std::exception& e)
        {
            ShowErrorFmt("CZone::LoadXiMesh: Failed to load '{}': {}", file, e.what());
        }
    }
}

void CZone::InsertMOB(CBaseEntity* PMob)
{
    m_zoneEntities->InsertMOB(PMob);
}

void CZone::InsertNPC(CBaseEntity* PNpc)
{
    m_zoneEntities->InsertNPC(PNpc);
}

void CZone::InsertPET(CBaseEntity* PPet)
{
    m_zoneEntities->InsertPET(PPet);
}

void CZone::InsertTRUST(CBaseEntity* PTrust)
{
    m_zoneEntities->InsertTRUST(PTrust);
}

void CZone::InsertTriggerArea(std::unique_ptr<ITriggerArea>&& triggerArea)
{
    if (triggerArea != nullptr)
    {
        m_triggerAreaList.emplace_back(std::move(triggerArea));
    }
}

/************************************************************************
 *                                                                       *
 *  We are looking for a monster for a party. For monsters grouped       *
 *  together, mutual aid (link) system is used                           *
 *                                                                       *
 ************************************************************************/

void CZone::FindPartyForMob(CBaseEntity* PEntity)
{
    TracyZoneScoped;

    m_zoneEntities->FindPartyForMob(PEntity);
}

void CZone::onEntityMoved(CBaseEntity* PEntity)
{
    m_zoneEntities->onEntityMoved(PEntity);
}

void CZone::TransportDepart(uint16 boundary, uint16 prevZoneId, uint16 transportId)
{
    m_zoneEntities->TransportDepart(boundary, prevZoneId, transportId);
}

void CZone::updateCharLevelRestriction(CCharEntity* PChar)
{
    TracyZoneScoped;

    const bool hasRestriction = PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::LevelRestriction);
    if (hasRestriction)
    {
        // If the level restriction is already the same then no need to change it
        CStatusEffect* statusEffect = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::LevelRestriction);
        if (zonehelpers::ShouldSkipLevelRestrictionUpdate(
                true,
                statusEffect == nullptr,
                statusEffect != nullptr && statusEffect->GetPower() == m_levelRestriction))
        {
            return;
        }

        if (zonehelpers::ShouldDeleteExistingLevelRestriction(true, false))
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::LevelRestriction);
        }
    }

    if (zonehelpers::ShouldApplyZoneLevelRestriction(m_levelRestriction))
    {
        // remove buffs in level cap zones as well (such as riverne sites)
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Dispelable, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Erasable, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Attack, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::OnZone, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Song, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Roll, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::SynthSupport, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Bloodpact, EffectNotice::Silent);
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Reraise);
        PChar->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::LevelRestriction, static_cast<uint16>(xi::StatusEffect::LevelRestriction), m_levelRestriction, 0s, 0s);
    }
}

void CZone::SetWeather(const Weather weather)
{
    TracyZoneScoped;

    if (zonehelpers::ShouldRejectInvalidWeather(magic_enum::enum_contains<Weather>(weather)))
    {
        ShowWarningFmt("{}", zonehelpers::FormatInvalidWeatherWarning(static_cast<uint16_t>(weather)));
        return;
    }

    if (zonehelpers::ShouldSkipSameWeather(weather_.current() == weather))
    {
        return;
    }

    m_zoneEntities->WeatherChange(weather);

    const uint32 changeTime = earth_time::vanadiel_timestamp();
    weather_.set(weather, changeTime);

    m_zoneEntities->PushPacket(
        nullptr,
        CHAR_INZONE,
        std::make_unique<GP_SERV_COMMAND_WEATHER>(
            changeTime,
            weather,
            xirand::GetRandomNumber(zonehelpers::WeatherPacketOffsetMin, zonehelpers::WeatherPacketOffsetMaxExclusive)));
}

void CZone::UpdateWeather()
{
    TracyZoneScoped;

    vanadiel_time::time_point CurrentVanaDate   = vanadiel_time::now(); // Current Vanadiel time
    vanadiel_time::time_point nextVanaMidnight  = vanadiel_time::get_next_midnight(CurrentVanaDate);
    vanadiel_time::time_point StartFogVanaDate  = nextVanaMidnight - xi::vanadiel_clock::days(1) + xi::vanadiel_clock::hours(2); // Vanadiel timestamp of 2 AM in minutes
    vanadiel_time::time_point EndFogVanaDate    = StartFogVanaDate + xi::vanadiel_clock::hours(5);                               // Vanadiel timestamp of 7 AM in minutes
    vanadiel_time::duration   WeatherNextUpdate = 0s;
    uint32                    WeatherDay        = 0;
    uint8                     WeatherChance     = 0;

    // Random time between 3 minutes and 30 minutes for the next weather change
    WeatherNextUpdate = std::chrono::seconds(xirand::GetRandomNumber(
        zonehelpers::WeatherUpdateDelayMinSeconds,
        zonehelpers::WeatherUpdateDelayMaxExclusiveSeconds));

    // Calculate what day we are on since the start of vanadiel time
    WeatherDay = vanadiel_time::count_days(CurrentVanaDate.time_since_epoch());

    // The weather starts over again every 2160 days
    WeatherDay = WeatherDay % zonehelpers::WeatherCycleDays;

    // Get a random number to determine which weather effect we will use
    WeatherChance = xirand::GetRandomNumber(100);

    const ZoneWeather weatherType = weather_.entryForDay(static_cast<uint16>(WeatherDay));

    auto selectedWeather = Weather::None;

    // 15% chance for rare weather, 35% chance for common weather, 50% chance for normal weather
    // * Percentages were generated from a 6 hour sample and rounded down to closest multiple of 5*
    switch (zonehelpers::SelectWeatherBand(WeatherChance))
    {
        case 0:
            selectedWeather = weatherType.rare;
            break;
        case 1:
            selectedWeather = weatherType.common;
            break;
        default:
            selectedWeather = weatherType.normal;
            break;
    }

    // This check is incorrect, fog is not simply a time of day, though it may consistently happen in SOME zones
    // (Al'Taieu likely has it every morning, while Atohwa Chasm can have it at random any time of day)
    if (zonehelpers::ShouldForceMorningFog(
            (CurrentVanaDate >= StartFogVanaDate) && (CurrentVanaDate < EndFogVanaDate),
            selectedWeather < Weather::HotSpell,
            (GetTypeMask() & ZONE_TYPE::CITY) != 0))
    {
        selectedWeather = Weather::Fog;
        // Force the weather to change by 7 am
        WeatherNextUpdate = EndFogVanaDate - CurrentVanaDate;
    }

    SetWeather(selectedWeather);
    luautils::OnZoneWeatherChange(GetID(), selectedWeather);

    scheduler_.postToMainThread(
        [this, duration = std::chrono::duration_cast<earth_time::duration>(WeatherNextUpdate)]() -> Task<void>
        {
            co_await scheduler_.yieldFor(duration);
            if (zonehelpers::ShouldRescheduleDynamicWeather(this->weather().isStatic()))
            {
                this->UpdateWeather();
            }
        });
}

bool CZone::CheckMobsPathedBack()
{
    bool anyAway = false;
    if (m_zoneEntities && m_zoneEntities->GetMobList().size() > 0)
    {
        const auto& mobListMap = m_zoneEntities->GetMobList();
        for (const auto& pair : mobListMap)
        {
            CMobEntity* mob = dynamic_cast<CMobEntity*>(pair.second);
            // if the mob is (not dead/despawned AND it is not fully healed) OR it is pathing home
            if (mob && zonehelpers::IsMobAwayFromHome(mob->isDead(), mob->isFullyHealed(), mob->m_IsPathingHome))
            {
                // at least one mob is away from home or not fully healed
                anyAway = true;
                break;
            }
        }
        return zonehelpers::ShouldReportAllMobsHomeAndHealed(true, anyAway);
    }

    return zonehelpers::ShouldReportAllMobsHomeAndHealed(false, false);
}

/************************************************************************
 *                                                                       *
 *  Remove a character from the zone. If ZoneServer and character are    *
 *  online, and there is no more left in the zone, then stop zone        *
 *                                                                       *
 ************************************************************************/

void CZone::DecreaseZoneCounter(CCharEntity* PChar)
{
    TracyZoneScoped;

    m_zoneEntities->DecreaseZoneCounter(PChar);

    const bool charListEmpty = m_zoneEntities->CharListEmpty();
    if (zonehelpers::ShouldStampZoneEmptyTime(charListEmpty))
    {
        m_timeZoneEmpty = timer::now();
    }
    else if (zonehelpers::ShouldDespawnPCOnLeave(charListEmpty))
    {
        m_zoneEntities->DespawnPC(PChar);
    }

    CharZoneOut(PChar);
}

/************************************************************************
 *                                                                       *
 *  Add a character to the zone. If zone isn't running, then load zone.  *
 *  Be sure to check the number of characters in the zone.               *
 *  The maximum number of characters in one zone is 768                  *
 *                                                                       *
 ************************************************************************/

void CZone::IncreaseZoneCounter(CCharEntity* PChar)
{
    TracyZoneScoped;

    if (zonehelpers::ShouldRejectIncreaseZoneCounter(
            PChar == nullptr,
            PChar != nullptr && PChar->loc.zone != nullptr,
            PChar != nullptr && PChar->PTreasurePool != nullptr))
    {
        ShowWarning("%s", zonehelpers::FormatIncreaseZoneCounterWarning());
        return;
    }

    PChar->targid = m_zoneEntities->GetNewCharTargID();

    if (zonehelpers::ShouldRejectHighCharTargid(PChar->targid))
    {
        ShowError("%s", zonehelpers::FormatInsertCharTargidHighErrorPrefix());
        return;
    }

    m_zoneEntities->InsertPC(PChar);

    if (zonehelpers::ShouldCreateZoneTimers(zoneTimerToken_.has_value(), m_zoneEntities->CharListEmpty()))
    {
        createZoneTimers();
    }

    PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::OnZonePathos, EffectNotice::Silent);

    CharZoneIn(PChar);
}

void CZone::SpawnMOBs(CCharEntity* PChar)
{
    m_zoneEntities->SpawnMOBs(PChar);
}

void CZone::SpawnPETs(CCharEntity* PChar)
{
    m_zoneEntities->SpawnPETs(PChar);
}

void CZone::SpawnTRUSTs(CCharEntity* PChar)
{
    m_zoneEntities->SpawnTRUSTs(PChar);
}

void CZone::SpawnNPCs(CCharEntity* PChar)
{
    m_zoneEntities->SpawnNPCs(PChar);
}

void CZone::SpawnPCs(CCharEntity* PChar)
{
    m_zoneEntities->SpawnPCs(PChar);
}

void CZone::SpawnConditionalNPCs(CCharEntity* PChar)
{
    m_zoneEntities->SpawnConditionalNPCs(PChar);
}

void CZone::SpawnTransport(CCharEntity* PChar)
{
    m_zoneEntities->SpawnTransport(PChar);
}

CBaseEntity* CZone::GetEntity(uint16 targid, uint8 filter)
{
    return m_zoneEntities->GetEntity(targid, filter);
}

/************************************************************************
 *                                                                       *
 *  Process the world's adjustments to time of day changing              *
 *                                                                       *
 ************************************************************************/

void CZone::TOTDChange(vanadiel_time::TOTD TOTD)
{
    TracyZoneScoped;

    m_zoneEntities->TOTDChange(TOTD);

    luautils::OnTOTDChange(m_zoneID, TOTD);
}

void CZone::SavePlayTime()
{
    TracyZoneScoped;

    m_zoneEntities->SavePlayTime();
}

CCharEntity* CZone::GetCharByName(const std::string& name)
{
    return m_zoneEntities->GetCharByName(name);
}

CCharEntity* CZone::GetCharByID(uint32 id)
{
    return m_zoneEntities->GetCharByID(id);
}

void CZone::PushPacket(CBaseEntity* PEntity, GLOBAL_MESSAGE_TYPE message_type, const std::unique_ptr<CBasicPacket>& packet)
{
    TracyZoneScoped;

    m_zoneEntities->PushPacket(PEntity, message_type, packet);
}

void CZone::UpdateEntityPacket(CBaseEntity* PEntity, ENTITYUPDATE type, uint8 updatemask, bool alwaysInclude)
{
    TracyZoneScoped;

    m_zoneEntities->UpdateEntityPacket(PEntity, type, updatemask, alwaysInclude);
}

void CZone::WideScan(CCharEntity* PChar, uint16 radius)
{
    TracyZoneScoped;

    m_zoneEntities->WideScan(PChar, radius);
}

/************************************************************************
 *                                                                       *
 *  Characters should be processed last when processing activity and     *
 *  status effects of entities in the zone.                              *
 *                                                                       *
 ************************************************************************/

auto CZone::ZoneServer(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;

    co_await m_zoneEntities->ZoneServer(tick);

    if (m_BattlefieldHandler != nullptr)
    {
        m_BattlefieldHandler->HandleBattlefields(tick);
    }

    if (zonehelpers::ShouldStopZoneTimers(
            zoneTimerToken_.has_value(),
            m_zoneEntities->CharListEmpty(),
            m_timeZoneEmpty,
            timer::now(),
            CheckMobsPathedBack()))
    {
        zoneTimerToken_.reset();
        zoneTimerTriggerAreasToken_.reset();
    }

    co_return;
}

void CZone::ForEachChar(const std::function<void(CCharEntity*)>& func)
{
    TracyZoneScoped;

    m_zoneEntities->ForEachChar(func);
}

void CZone::ForEachCharInstance(CBaseEntity* PEntity, const std::function<void(CCharEntity*)>& func)
{
    TracyZoneScoped;

    ForEachChar(func);
}

void CZone::ForEachMob(const std::function<void(CMobEntity*)>& func)
{
    TracyZoneScoped;

    m_zoneEntities->ForEachMob(func);
}

void CZone::ForEachMobInstance(CBaseEntity* PEntity, const std::function<void(CMobEntity*)>& func)
{
    TracyZoneScoped;

    ForEachMob(func);
}

void CZone::ForEachNpc(const std::function<void(CNpcEntity*)>& func)
{
    TracyZoneScoped;

    m_zoneEntities->ForEachNpc(func);
}

void CZone::ForEachNpcInstance(CBaseEntity* PEntity, const std::function<void(CNpcEntity*)>& func)
{
    TracyZoneScoped;

    ForEachNpc(func);
}

void CZone::ForEachTrust(const std::function<void(CTrustEntity*)>& func)
{
    TracyZoneScoped;

    m_zoneEntities->ForEachTrust(func);
}

void CZone::ForEachTrustInstance(CBaseEntity* PEntity, const std::function<void(CTrustEntity*)>& func)
{
    TracyZoneScoped;

    ForEachTrust(func);
}

void CZone::ForEachPet(const std::function<void(CPetEntity*)>& func)
{
    TracyZoneScoped;

    m_zoneEntities->ForEachPet(func);
}

void CZone::ForEachPetInstance(CBaseEntity* PEntity, const std::function<void(CPetEntity*)>& func)
{
    TracyZoneScoped;

    ForEachPet(func);
}

void CZone::ForEachAlly(const std::function<void(CMobEntity*)>& func)
{
    TracyZoneScoped;

    m_zoneEntities->ForEachAlly(func);
}

void CZone::ForEachAllyInstance(CBaseEntity* PEntity, const std::function<void(CMobEntity*)>& func)
{
    TracyZoneScoped;

    ForEachAlly(func);
}

void CZone::createZoneTimers()
{
    TracyZoneScoped;

    const auto plan = zonehelpers::PlanZoneTimers(config_.isTestServer);
    if (!plan.install)
    {
        return;
    }

    zoneTimerToken_ = scheduler_.intervalOnMainThread(
        plan.logicInterval,
        [this]() -> Task<void>
        {
            co_await this->ZoneServer(timer::now());
        });

    zoneTimerTriggerAreasToken_ = scheduler_.intervalOnMainThread(
        plan.triggerAreaInterval,
        [this]() -> Task<void>
        {
            co_await this->CheckTriggerAreas();
        });
}

void CZone::CharZoneIn(CCharEntity* PChar)
{
    TracyZoneScoped;

    PChar->loc.zone        = this;
    PChar->loc.destination = 0;
    PChar->clearTriggerAreas();

    if (zonehelpers::ShouldDismountOnZoneIn(PChar->isMounted(), CanUseMisc(MISC_MOUNT)))
    {
        PChar->animation = ANIMATION_NONE;
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Mounted);
    }

    if (zonehelpers::ShouldClearCostumeOnZoneIn(PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Costume)))
    {
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Costume);
    }

    if (PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Illusion))
    {
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Illusion);
    }

    PChar->ReloadPartyInc();

    // Zone-wide treasure pool takes precendence over all others
    if (m_TreasurePool && m_TreasurePool->getPoolType() == TreasurePoolType::Zone)
    {
        PChar->PTreasurePool = m_TreasurePool;
        PChar->PTreasurePool->addMember(PChar);
    }
    else
    {
        if (PChar->PParty)
        {
            PChar->PParty->ReloadTreasurePool(PChar);
        }
        else
        {
            PChar->PTreasurePool = new CTreasurePool(TreasurePoolType::Solo);
            PChar->PTreasurePool->addMember(PChar);
        }
    }

    if (!(m_zoneType & ZONE_TYPE::INSTANCED))
    {
        charutils::ClearTempItems(PChar);
        PChar->PInstance = nullptr;
    }

    auto* PBattlefield = m_BattlefieldHandler ? m_BattlefieldHandler->GetBattlefield(PChar, true) : nullptr;
    const auto plan    = zonehelpers::PlanZoneInBattlefield(
        m_BattlefieldHandler != nullptr,
        PBattlefield != nullptr,
        PChar->StatusEffectContainer->HasStatusEffectByFlag(xi::StatusEffectFlag::Confrontation),
        CBattlefield::hasPlayerEntered(PChar),
        PChar->PPet != nullptr,
        PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::LevelSync),
        PChar->PParty != nullptr);

    switch (plan.action)
    {
        case zonehelpers::ZoneInBattlefieldAction::InsertRegistered:
            PBattlefield->InsertEntity(PChar, plan.entered);
            break;
        case zonehelpers::ZoneInBattlefieldAction::AddOrphaned:
            m_BattlefieldHandler->addOrphanedPlayer(PChar);
            break;
        case zonehelpers::ZoneInBattlefieldAction::ClearConfrontation:
            PChar->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Confrontation, EffectNotice::Silent);
            if (plan.updateLevelRestriction)
            {
                updateCharLevelRestriction(PChar);
            }
            if (plan.clearPetConfrontation)
            {
                PChar->PPet->StatusEffectContainer->DelStatusEffectsByFlag(xi::StatusEffectFlag::Confrontation, EffectNotice::Silent);
            }
            break;
        case zonehelpers::ZoneInBattlefieldAction::ClearLevelSync:
            PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelSync);
            PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelRestriction);
            break;
        case zonehelpers::ZoneInBattlefieldAction::None:
            break;
    }

    // Mark current zone as visited
    PChar->m_ZonesVisitedList[PChar->getZone() >> 3] |= (1 << (PChar->getZone() % 8));

    monstrosity::HandleZoneIn(PChar);

    PChar->PLatentEffectContainer->CheckLatentsZone();

    charutils::ReadHistory(PChar);

    // Restore seal recast timer if enabled
    if (settings::get<bool>("main.PERSIST_SEAL_TIMERS"))
    {
        const auto plan = zonehelpers::PlanSealTimerRestore(true, static_cast<uint32>(PChar->getCharVar("SealTimerExpiry")), earth_time::timestamp());
        if (plan.restore)
        {
            PChar->PRecastContainer->AddLootRecast(LootRecastID::Seal, std::chrono::seconds(plan.remainingSeconds));
        }
        if (plan.clearStoredExpiry)
        {
            PChar->setCharVar("SealTimerExpiry", 0);
        }
    }

    moduleutils::OnCharZoneIn(PChar);
}

void CZone::CharZoneOut(CCharEntity* PChar)
{
    TracyZoneScoped;

    if (const auto triggerArea = zonehelpers::FirstZoneOutTriggerArea(m_triggerAreaList, [PChar](const auto id) { return PChar->isInTriggerArea(id); }); triggerArea != m_triggerAreaList.end())
    {
        luautils::OnTriggerAreaLeave(PChar, *triggerArea);
    }

    // Save seal recast timer if enabled
    if (settings::get<bool>("main.PERSIST_SEAL_TIMERS"))
    {
        auto* recast = PChar->PRecastContainer->GetLootRecast(LootRecastID::Seal);
        if (recast && recast->RecastTime > 0s)
        {
            auto remaining = (recast->TimeStamp + recast->RecastTime) - timer::now();
            if (const auto expirationTimestamp = zonehelpers::PlanSealTimerExpiry(true, true, remaining, earth_time::timestamp()))
            {
                PChar->setCharVar("SealTimerExpiry", static_cast<int32>(*expirationTimestamp));
            }
        }
    }

    moduleutils::OnCharZoneOut(PChar);
    luautils::OnZoneOut(PChar);

    if (PChar->m_LevelRestriction != 0)
    {
        if (PChar->PParty)
        {
            if (PChar->PParty->GetSyncTarget() == PChar || PChar->PParty->GetLeader() == PChar)
            {
                PChar->PParty->SetSyncTarget("", MsgStd::LevelSyncDeactivateLeftArea);
            }
            if (PChar->PParty->GetSyncTarget() != nullptr)
            {
                uint8 count = 0;
                for (uint32 i = 0; i < PChar->PParty->members.size(); ++i)
                {
                    if (PChar->PParty->members.at(i) != PChar && PChar->PParty->members.at(i)->getZone() == PChar->PParty->GetSyncTarget()->getZone())
                    {
                        count++;
                    }
                }
                if (count < 2) // 3, because one is zoning out - thus at least 2 will be left
                {
                    PChar->PParty->SetSyncTarget("", MsgStd::LevelSyncRemoveTooFewMembers);
                }
            }
        }
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelSync);
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::LevelRestriction);
    }

    if (PChar->PTreasurePool != nullptr) // TODO: Condition for eliminating problems with MobHouse, we need to solve it once and for all!
    {
        PChar->PTreasurePool->delMember(PChar);
    }

    // If zone-wide treasure pool but no players in zone then destroy current pool and create new pool
    // this prevents loot from staying in zone pool after the last player leaves the zone
    if (m_TreasurePool && m_TreasurePool->getPoolType() == TreasurePoolType::Zone && m_zoneEntities->CharListEmpty())
    {
        destroy(m_TreasurePool);
        m_TreasurePool = new CTreasurePool(TreasurePoolType::Zone);
    }

    PChar->loc.zone = nullptr;

    if (PChar->status == STATUS_TYPE::SHUTDOWN)
    {
        PChar->loc.destination = m_zoneID;
    }
    else
    {
        PChar->loc.prevzone = m_zoneID;
    }

    charutils::WriteHistory(PChar);
}

bool CZone::IsZoneActive() const
{
    return zoneTimerToken_.has_value();
}

CZoneEntities* CZone::GetZoneEntities()
{
    return m_zoneEntities;
}

auto CZone::CheckTriggerAreas() -> Task<void>
{
    TracyZoneScoped;

    ForEachChar(
        [&](CCharEntity* PChar)
        {
            // TODO: When we start to use octrees or spatial hashing to split up zones,
            //     : use them here to make the search domain smaller.

            // Do not enter trigger areas while loading in. Set in xi.player.onGameIn.
            if (PChar->GetLocalVar("ZoningIn") > 0)
            {
                return;
            }

            for (const auto& triggerArea : m_triggerAreaList)
            {
                const auto triggerAreaID = triggerArea->getTriggerAreaID();
                switch (triggerarea::MembershipActionFor(false, triggerArea->isPointInside(PChar->loc.p), PChar->isInTriggerArea(triggerAreaID)))
                {
                    case triggerarea::MembershipAction::Enter:
                        // Add the TriggerArea to the players cache of current TriggerAreas.
                        PChar->onTriggerAreaEnter(triggerAreaID);
                        luautils::OnTriggerAreaEnter(PChar, triggerArea);
                        break;
                    case triggerarea::MembershipAction::Leave:
                        // Remove the TriggerArea from the players cache of current TriggerAreas.
                        PChar->onTriggerAreaLeave(triggerAreaID);
                        luautils::OnTriggerAreaLeave(PChar, triggerArea);
                        break;
                    case triggerarea::MembershipAction::None:
                        break;
                }
            }
        });

    co_return;
}
