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

// --- UpdateEnmity admission / TH / range / bonus (slice 1357) ---

// ShouldRejectNonMobHolder mirrors holder objtype != TYPE_MOB early return.
inline auto ShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

// EnmityRangeNormal is max distance for non-NM holders (25.0f).
constexpr float EnmityRangeNormal = 25.0f;

// EnmityRangeNotorious is max distance for NM holders (28.0f).
constexpr float EnmityRangeNotorious = 28.0f;

// EnmityRangeMax mirrors m_Type == MOBTYPE_NOTORIOUS ? 28.0f : 25.0f.
inline auto EnmityRangeMax(const bool isNotorious) -> float
{
    return isNotorious ? EnmityRangeNotorious : EnmityRangeNormal;
}

// IsWithinEnmityRangePure mirrors same-zone && within maxRange.
// distanceIsWithin is host-evaluated isWithinDistance(holder, entity, maxRange).
inline auto IsWithinEnmityRangePure(const bool sameZone, const bool distanceIsWithin) -> bool
{
    return sameZone && distanceIsWithin;
}

// ShouldZeroEnmityOutOfRange mirrors !IsWithinEnmityRange → CE=VE=0.
// Formula (slice 1357 residual / 2927 dual-wire):
//   ShouldZeroEnmityOutOfRange(withinRange) = !withinRange
// Dual-wire capacity: enmityrangehelpers::ShouldZeroEnmityOutOfRange in
// map/enmity_zero_out_of_range_capacity.h (same formula).
inline auto ShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

// TreasureHunterMainCap is max TH for THF main (8).
constexpr int16 TreasureHunterMainCap = 8;

// TreasureHunterNonMainCap is max TH for non-THF main (4).
constexpr int16 TreasureHunterNonMainCap = 4;

// CapTreasureHunterLevel enforces TH8 for THF main and TH4 otherwise.
// rawTH is getMod(TREASURE_HUNTER); isTHFMain is GetMJob() == JOB_THF.
inline auto CapTreasureHunterLevel(const int16 rawTH, const bool isTHFMain) -> int16
{
    if (isTHFMain)
    {
        return std::min(TreasureHunterMainCap, rawTH);
    }
    return std::min(TreasureHunterNonMainCap, rawTH);
}

// ShouldApplyDirectActionTH mirrors if (directAction) TH/GF side effects.
inline auto ShouldApplyDirectActionTH(const bool directAction) -> bool
{
    return directAction;
}

// ShouldRaiseHolderTH mirrors m_THLvl < THlevel.
inline auto ShouldRaiseHolderTH(const int16 holderTH, const int16 candidateTH) -> bool
{
    return holderTH < candidateTH;
}

// ShouldRaiseHolderGilfinder mirrors m_GilfinderLevel < GFlevel.
inline auto ShouldRaiseHolderGilfinder(const int16 holderGF, const int16 candidateGF) -> bool
{
    return holderGF < candidateGF;
}

// ShouldRebindEnmityOwner mirrors PEnmityOwner == nullptr on existing entry.
inline auto ShouldRebindEnmityOwner(const bool ownerPresent) -> bool
{
    return !ownerPresent;
}

// ShouldActivateEnmityEntry mirrors CE >= 0 && VE >= 0 after update.
inline auto ShouldActivateEnmityEntry(const int32 ce, const int32 ve) -> bool
{
    return ce >= 0 && ve >= 0;
}

// ShouldCreateNewEnmityEntry mirrors else if (CE >= 0 && VE >= 0) for missing key.
inline auto ShouldCreateNewEnmityEntry(const bool entryExists, const int32 ce, const int32 ve) -> bool
{
    return !entryExists && ce >= 0 && ve >= 0;
}

// ShouldApplyInitialEnmityBoost mirrors initial==true (no active entries yet).
inline auto ShouldApplyInitialEnmityBoost(const bool anyActiveEntry) -> bool
{
    return !anyActiveEntry;
}

// InitialCEBoost / InitialVEBoost for first active entry on a quiet list.
constexpr int32 InitialCEBoost = 200;
constexpr int32 InitialVEBoost = 900;

// ShouldAddMasterBaseEnmity mirrors withMaster && (pet || charmed-mob-by-PC).
// isPet is objtype==TYPE_PET; isCharmedMobByPC is TYPE_MOB with PC master.
inline auto ShouldAddMasterBaseEnmity(const bool withMaster, const bool hasMaster, const bool isPet, const bool isCharmedMobByPC) -> bool
{
    return withMaster && hasMaster && (isPet || isCharmedMobByPC);
}

// ShouldMarkNotTameable mirrors if (!tameable) m_tameable = false.
inline auto ShouldMarkNotTameable(const bool tameable) -> bool
{
    return !tameable;
}

// EnmityBonusMinClamp / EnmityBonusMaxClamp for raw enmityBonus before factor.
constexpr int EnmityBonusMinClamp = -50;
constexpr int EnmityBonusMaxClamp = 100;

// CalculateEnmityBonusFactor mirrors (100 + clamp(enmityBonus,-50,100)) / 100.0f.
// enmityBonus is sum of ENMITY mod and PC merit adjustments (host-computed).
inline auto CalculateEnmityBonusFactor(const int enmityBonus) -> float
{
    const int clamped = std::clamp(enmityBonus, EnmityBonusMinClamp, EnmityBonusMaxClamp);
    return (100.0f + static_cast<float>(clamped)) / 100.0f;
}

// ShouldSkipDamageEnmitySelf mirrors holder id == entity id.
inline auto ShouldSkipDamageEnmitySelf(const bool sameEntity) -> bool
{
    return sameEntity;
}

// FloorDamageForEnmity mirrors Damage < 1 ? 1 : Damage.
inline auto FloorDamageForEnmity(const int32 damage) -> int32
{
    return damage < 1 ? 1 : damage;
}

// AttackEnmityCEDelta mirrors -1800 * Damage / maxHP * reduction.
// reduction is (100 - min(ENMITY_LOSS_REDUCTION, 100)) / 100.
inline auto AttackEnmityLossReduction(const int16 enmityLossReductionMod) -> float
{
    return (100.0f - static_cast<float>(std::min<int16>(enmityLossReductionMod, 100))) / 100.0f;
}

inline auto AttackEnmityCEDelta(const int32 damage, const int32 maxHP, const float reduction) -> int32
{
    if (maxHP <= 0)
    {
        return 0;
    }
    return static_cast<int32>(-1800.0f * static_cast<float>(damage) / static_cast<float>(maxHP) * reduction);
}

// CoverEnmityCEBonus is +200 CE applied to cover user.
constexpr int32 CoverEnmityCEBonus = 200;

// CoverEnmityLowerPercent is 10% lower on cover target.
constexpr uint8 CoverEnmityLowerPercent = 10;

// ShouldApplyCoverEnmity mirrors both cover target and user non-null.
inline auto ShouldApplyCoverEnmity(const bool targetNonNull, const bool userNonNull) -> bool
{
    return targetNonNull && userNonNull;
}

// CoverUserNewCE mirrors GetCE(user) + 200.
inline auto CoverUserNewCE(const int32 currentCE) -> int32
{
    return currentCE + CoverEnmityCEBonus;
}

// ShouldRaiseHiPCLvl mirrors m_HiPCLvl < entity MLevel after damage enmity.
inline auto ShouldRaiseHiPCLvl(const uint8 holderHiPCLvl, const uint8 entityMLevel) -> bool
{
    return holderHiPCLvl < entityMLevel;
}

// ShouldSkipHighestEnmitySameAllegiance mirrors owner allegiance == holder.
// ownerMissing means POwner is null — still eligible (treat as candidate).
inline auto ShouldSkipHighestEnmitySameAllegiance(const bool ownerPresent, const bool sameAllegiance) -> bool
{
    return ownerPresent && sameAllegiance;
}

// ShouldPreferCurrentBattleTargetOnTie mirrors tie + highest is battle target.
inline auto ShouldPreferCurrentBattleTargetOnTie(
    const bool enmityTied,
    const bool hasCurrentHighest,
    const bool holderHasBattleTarget,
    const bool currentHighestIsBattleTarget) -> bool
{
    return enmityTied && hasCurrentHighest && holderHasBattleTarget && currentHighestIsBattleTarget;
}

// ShouldPruneHighestEnmity mirrors missing/wrong-zone/instance/dead owner.
inline auto ShouldPruneHighestEnmity(
    const bool ownerResolved,
    const bool sameZone,
    const bool sameInstance,
    const bool isDead) -> bool
{
    return !ownerResolved || !sameZone || !sameInstance || isDead;
}

// ShouldAddBaseEnmitySameZone mirrors AddBaseEnmity zone check.
inline auto ShouldAddBaseEnmitySameZone(const bool sameZone) -> bool
{
    return sameZone;
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
