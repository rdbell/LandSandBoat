/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#pragma once

#include "common/cbasetypes.h"

#include "packets/basic.h"

#include "entities/battle_entity.h"
#include "modifier.h"
#include "packets/c2s/0x01a_action.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

struct mon_data_t;
class CCharEntity;

// ===
// See scripts/globals/monstrosity.lua for a general overview of how Monstrosity works and is designed.
// ===
namespace monstrosity
{

struct MonstrosityData_t
{
public:
    MonstrosityData_t();

    uint8  MonstrosityId;
    uint16 Species;
    uint16 Flags;
    uint16 Look;
    uint8  Size;

    uint8 NamePrefix1;
    uint8 NamePrefix2;

    JOBTYPE MainJob;
    JOBTYPE SubJob;
    uint32  CurrentExp;

    std::array<uint16, 12> EquippedInstincts{ 0 };
    std::array<uint8, 128> levels{ 0 };
    std::array<uint8, 64>  instincts{ 0 };
    std::array<uint8, 32>  variants{ 0 };

    bool Belligerency;

    position_t EntryPos{};
    uint16     EntryZoneId;
    uint8      EntryMainJob;
    uint8      EntrySubJob;
};

struct SpeciesCatalogRow
{
    uint8       monstrosityId{};
    uint16      monstrositySpeciesCode{};
    std::string name{};
    JOBTYPE     mjob{};
    JOBTYPE     sjob{};
    uint8       size{};
    uint16      look{};
};

using SpeciesCatalog = std::unordered_map<uint16, SpeciesCatalogRow>;

struct InstinctCatalogRow
{
    uint16                 monstrosityInstinctId{};
    uint8                  cost{};
    std::string            name{};
    std::vector<CModifier> mods{};
};

using InstinctCatalog = std::unordered_map<uint16, InstinctCatalogRow>;

struct DeathMenuPlan
{
    bool restoreHpMp{};
    bool clearAnimation{};
    bool markHpUpdate{};
    bool returnToEntrance{};
    bool retryAtOrigin{};
    bool clearDeathTime{};
    bool setDisappear{};
    bool clearPacketList{};
    bool restartCurrentZone{};
};

struct ZoneInPlan
{
    bool   applyInstinctModifiers{};
    bool   addGestation{};
    uint32 gestationDurationSeconds{};
    bool   sendFullUpdate{};
    bool   markLookUpdate{};
};

struct MonsterSkillActionPlan
{
    bool   invokeMobSkill{};
    uint16 actionIndex{};
    uint16 skillId{};
};

struct SpeciesChangeCandidate
{
    uint8   monstrosityId{};
    JOBTYPE mainJob{};
    JOBTYPE subJob{};
    uint8   size{};
    uint16  look{};
};

struct SpeciesChangePlan
{
    bool    changeSpecies{};
    uint16  species{};
    uint8   monstrosityId{};
    JOBTYPE mainJob{};
    JOBTYPE subJob{};
    uint8   size{};
    uint16  look{};
    bool    clearInstincts{};
    bool    eraseStatusEffects{};
};

struct DescriptorUpdatePlan
{
    bool  setNamePrefix1{};
    uint8 namePrefix1{};
    bool  setNamePrefix2{};
    uint8 namePrefix2{};
    bool  writeData{ true };
    bool  sendFullUpdate{ true };
};

struct InstinctSlotUpdatePlan
{
    bool   setRequestedInstinct{};
    uint16 requestedInstinct{};
    bool   removePreviousModifiers{};
    bool   addRequestedModifiers{};
    bool   restorePreviousLoadout{};
    bool   abortHandler{};
};

struct PopulateMonstrosityDataPlan
{
    bool readData{};
    bool writeData{};
};

struct MonstrosityDataRow
{
    uint8                 monstrosityId{};
    uint16                species{};
    uint8                 namePrefix1{};
    uint8                 namePrefix2{};
    uint32                currentExp{};
    std::array<uint16, 12> equippedInstincts{};
    std::array<uint8, 128> levels{};
    std::array<uint8, 64>  instincts{};
    std::array<uint8, 32>  variants{};
    bool                  belligerency{};
    position_t            entryPos{};
    uint16                entryZoneId{};
    uint8                 entryMainJob{};
    uint8                 entrySubJob{};
};

struct SpeciesRuntimeData
{
    uint16  look{};
    JOBTYPE mainJob{};
    JOBTYPE subJob{};
    uint8   size{};
};

struct MonstrosityDataWrite
{
    uint32                charId{};
    uint8                 monstrosityId{};
    uint16                species{};
    uint8                 namePrefix1{};
    uint8                 namePrefix2{};
    uint32                currentExp{};
    std::array<uint16, 12> equippedInstincts{};
    std::array<uint8, 128> levels{};
    std::array<uint8, 64>  instincts{};
    std::array<uint8, 32>  variants{};
    uint8                 belligerency{};
    position_t            entryPos{};
    uint16                entryZoneId{};
    uint8                 entryMainJob{};
    uint8                 entrySubJob{};
};

struct MonstrosityDataWritePlan
{
    bool                  write{};
    MonstrosityDataWrite values{};
};

void LoadStaticData();
void ApplySpeciesCatalogRows(SpeciesCatalog& catalog, const std::vector<SpeciesCatalogRow>& rows);
void ApplyInstinctCatalogRows(InstinctCatalog& catalog, const std::vector<InstinctCatalogRow>& rows);

void ReadMonstrosityData(CCharEntity* PChar);
void WriteMonstrosityData(CCharEntity* PChar);

void   TryPopulateMonstrosityData(CCharEntity* PChar);
void   HandleZoneIn(CCharEntity* PChar);
uint32 GetPackedMonstrosityName(CCharEntity* PChar);
void   SendFullMonstrosityUpdate(CCharEntity* PChar);

void HandleMonsterSkillActionPacket(const CCharEntity* PChar, const GP_CLI_COMMAND_ACTION& data);
void HandleEquipChangePacket(CCharEntity* PChar, const mon_data_t& data);

void SetLevel(CCharEntity* PChar, uint8 id, uint8 level);

void HandleDeathMenu(CCharEntity* PChar, GP_CLI_COMMAND_ACTION_HOMEPOINTMENU type);
DeathMenuPlan PlanDeathMenu(bool hasMonstrosity, GP_CLI_COMMAND_ACTION_HOMEPOINTMENU type);
ZoneInPlan PlanZoneIn(bool monstrosityEnabled, bool hasMonstrosity, bool isFeretory, bool belligerency);
MonsterSkillActionPlan PlanMonsterSkillAction(uint8 mainJob, bool hasMonstrosity, uint16 actionIndex, uint16 skillId);
SpeciesChangePlan PlanSpeciesChange(bool hasCandidate, uint16 speciesIndex, const SpeciesChangeCandidate& candidate, uint8 previousMonstrosityId, uint8 speciesLevel, bool variantUnlocked, bool dontWipeBuffs);
DescriptorUpdatePlan PlanDescriptorUpdate(bool speciesFlag, bool instinctFlag, bool descriptor1Flag, bool descriptor2Flag, uint8 descriptor1Index, uint8 descriptor2Index);
InstinctSlotUpdatePlan PlanInstinctSlotUpdate(uint16 requestedInstinct, bool hasCatalogEntry, bool isUnlocked, bool rejectLoadout);
PopulateMonstrosityDataPlan PlanPopulateMonstrosityData(bool monstrosityEnabled, uint8 mainJob);
MonstrosityData_t BuildMonstrosityData(bool hasRow, const MonstrosityDataRow& row, bool hasSpecies, const SpeciesRuntimeData& species);
MonstrosityDataWritePlan PlanMonstrosityDataWrite(bool hasData, uint32 charId, const MonstrosityData_t& data);

bool IsInstinctUnlocked(CCharEntity* PChar, uint16 instinct);
bool IsVariantUnlocked(CCharEntity* PChar, uint8 variant);

void SetBelligerencyFlag(CCharEntity* PChar, bool flag);

// Debug
void MaxAllLevels(CCharEntity* PChar);
void UnlockAllInstincts(CCharEntity* PChar);
void UnlockAllVariants(CCharEntity* PChar);

} // namespace monstrosity
