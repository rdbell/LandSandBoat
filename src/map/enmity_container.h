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

#ifndef _CENMITYCONTAINER_H
#define _CENMITYCONTAINER_H

#include "common/cbasetypes.h"
#include "map_constants.h"

#include <algorithm>
#include <unordered_map>

class CBattleEntity;
class CMobEntity;
class CCharEntity;

struct EnmityObject_t
{
    CBattleEntity* PEnmityOwner;
    int32          CE; // Cumulative Enmity
    int32          VE; // Volatile Enmity
    bool           active;
};

typedef std::unordered_map<uint32, EnmityObject_t> EnmityList_t;

// Pure CE/VE math helpers shared by CEnmityContainer and unit tests.
// These deliberately avoid battle-entity, zone, and notoriety dependencies so
// OmegaXI parity fixtures can pin clamp/bonus/decay behavior in isolation.
namespace enmitymath
{
// Per-tick VE reduction: (int)(60 / kLogicUpdateRate) == 24 with default rate.
inline constexpr int32 VEDecayAmount()
{
    return static_cast<int32>(60 / kLogicUpdateRate);
}

inline auto ClampEnmity(int32 value, int32 cap) -> int32
{
    return std::clamp(value, 0, cap);
}

// SetCE/SetVE use min(amount, cap) only — no zero floor.
inline auto CapAmount(int32 amount, int32 cap) -> int32
{
    return std::min(amount, cap);
}

// Existing-entry delta: current + (delta > 0 ? delta * bonus : delta).
inline auto ApplyDelta(int32 current, int32 delta, float bonus) -> int32
{
    return static_cast<int32>(current + (delta > 0 ? delta * bonus : delta));
}

// New-entry axis after optional initial boost, then bonus, then [0, cap] clamp.
inline auto ApplyNewEntryAxis(int32 value, float bonus, int32 cap) -> int32
{
    return ClampEnmity(static_cast<int32>(value * bonus), cap);
}

inline auto LowerByPercent(int32 value, uint8 percent) -> int32
{
    float  mod    = (static_cast<float>(percent) / 100.0f);
    auto   amount = static_cast<int32>(value * mod);
    return value - (amount < 0 ? 0 : amount);
}

inline auto DecayVE(int32 ve) -> int32
{
    const int32 decay = VEDecayAmount();
    return ve - (ve > decay ? decay : ve);
}
} // namespace enmitymath

class CEnmityContainer
{
    int32 EnmityCap;

public:
    CEnmityContainer(CMobEntity* holder);
    ~CEnmityContainer();

    CBattleEntity* GetHighestEnmity(); // Decays VE and gets target with highest enmity

    float         CalculateEnmityBonus(CBattleEntity* PEntity);
    void          Clear(uint32 EntityID = 0);   // Removes Entries from list
    void          LogoutReset(uint32 EntityID); // Sets entry to inactive
    void          SetActive(uint32 EntityID, bool active);
    void          AddBaseEnmity(CBattleEntity* PEntity);
    void          UpdateEnmity(CBattleEntity* PEntity, int32 CE, int32 VE, bool withMaster = false, bool tameable = false, bool directAction = true);
    void          UpdateEnmityFromDamage(CBattleEntity* PEntity, int32 Damage);
    void          UpdateEnmityFromCure(CBattleEntity* PEntity, uint8 level, int32 CureAmount, int32 fixedCE = 0, int32 fixedVE = 0);
    void          UpdateEnmityFromAttack(CBattleEntity* PEntity, int32 Damage);
    bool          HasID(uint32 ID);                                                                         // true if ID is in the container with non-zero enmity level
    void          LowerEnmityByPercent(CBattleEntity* PEntity, uint8 percent, CBattleEntity* HateReceiver); // lower % of hate or transfer it
    int32         GetCE(CBattleEntity* PEntity) const;
    int32         GetVE(CBattleEntity* PEntity) const;
    void          SetCE(CBattleEntity* PEntity, const int32 amount);
    void          SetVE(CBattleEntity* PEntity, const int32 amount);
    void          DecayEnmity();
    bool          IsWithinEnmityRange(CBattleEntity* PEntity) const;
    EnmityList_t* GetEnmityList();
    bool          IsTameable() const;
    void          UpdateEnmityFromCover(CBattleEntity* PCoverAbilityTarget, CBattleEntity* PCoverAbilityUser);

private:
    EnmityList_t m_EnmityList;
    bool         m_tameable{ true };
    CMobEntity*  m_EnmityHolder; // usually a monster
};

#endif
