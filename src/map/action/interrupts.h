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

#pragma once
#include "action.h"

class CSpell;
class CPetSkill;
class CBattleEntity;

// Helpers to build specific action packet interruption flows
// Baby chocobos will die if you make changes without a proper test with retail captures
namespace ActionInterrupts
{

namespace detail
{

// AbilityInterruptAction builds the BATTLE2 action emitted when an ability is
// interrupted. Packet delivery remains owned by AbilityInterrupt.
auto AbilityInterruptAction(uint32 actorId) -> action_t;

// RangedInterruptAction builds the BATTLE2 action emitted when a ranged attack
// is interrupted. Packet delivery remains owned by RangedInterrupt.
auto RangedInterruptAction(uint32 actorId) -> action_t;

// MobSkillNoTargetInRangeAction builds the BATTLE2 action emitted when a mob
// skill has no targets in range. Packet delivery remains owned by
// MobSkillNoTargetInRange.
auto MobSkillNoTargetInRangeAction(uint32 actorId) -> action_t;

// OutOfRangeAction builds the BATTLE2 action emitted when a mob or weapon
// skill's target is out of range. Packet delivery remains owned by the public
// interruption helpers.
auto OutOfRangeAction(uint32 actorId, uint32 targetId) -> action_t;

// AttackInterruptAction builds the BATTLE2 action emitted when an attack is
// interrupted by a status effect. Packet delivery remains owned by the public
// interruption helpers.
auto AttackInterruptAction(uint32 actorId, uint32 targetId, MsgBasic messageID) -> action_t;

// RangedParalyzedAction builds the second BATTLE2 action emitted when a
// ranged attack is interrupted by paralysis. RangedInterrupt remains the
// first packet and packet delivery remains owned by the public helper.
auto RangedParalyzedAction(uint32 actorId) -> action_t;

// AbilityParalyzedAction builds one of the ordered BATTLE2 actions emitted
// when an ability is interrupted by paralysis. Packet delivery remains owned
// by the public helper.
auto AbilityParalyzedAction(uint32 actorId, uint32 targetId) -> action_t;

// MagicInterruptAction builds the BATTLE2 action emitted when a spell is
// interrupted before its finish action. Packet delivery remains owned by
// MagicInterrupt.
auto MagicInterruptAction(uint32 actorId, uint16 spellId, FourCC interruptID) -> action_t;

// MagicStatusFinishAction builds the first BATTLE2 action emitted when a
// spell is interrupted by a status effect. Packet delivery remains owned by
// MagicParalyzed and MagicIntimidated.
auto MagicStatusFinishAction(uint32 actorId, uint32 targetId, uint16 spellId, MsgBasic messageID) -> action_t;

// MagicStopCastAction builds the stop-cast BATTLE2 action emitted after a
// status interruption. Packet delivery remains owned by the public helper.
auto MagicStopCastAction(uint32 actorId, uint16 spellId, FourCC interruptID, timer::duration recast) -> action_t;

// ItemInterruptAction builds the BATTLE2 action emitted when an item use is
// interrupted before completion. Packet delivery remains owned by
// ItemInterrupt.
auto ItemInterruptAction(uint32 actorId) -> action_t;

// ItemStatusFinishAction builds the first BATTLE2 action emitted when an item
// use is interrupted by a status effect. Packet delivery remains owned by
// ItemParalyzed and ItemIntimidated.
auto ItemStatusFinishAction(uint32 actorId, uint32 targetId, MsgBasic messageID) -> action_t;

// ItemStopAction builds the item-cancel BATTLE2 action emitted after a status
// interruption. Packet delivery remains owned by the public helper.
auto ItemStopAction(uint32 actorId) -> action_t;

} // namespace detail

void AbilityInterrupt(CBattleEntity* PEntity);

void AvatarOutOfRange(CBattleEntity* PAvatar, const CPetSkill* PSkill, const CBattleEntity* PTarget);
void WyvernOutOfRange(CBattleEntity* PWyvern, const CPetSkill* PSkill, const CBattleEntity* PTarget);
void WyvernSkillReady(CBattleEntity* PWyvern);

void MobSkillNoTargetInRange(CBattleEntity* PEntity);
void MobSkillOutOfRange(CBattleEntity* PEntity, const CBattleEntity* PTarget);

void WeaponSkillOutOfRange(CBattleEntity* PEntity, const CBattleEntity* PTarget);

void RangedInterrupt(CBattleEntity* PEntity);
void RangedParalyzed(CBattleEntity* PEntity);

void AttackParalyzed(CBattleEntity* PEntity, const CBattleEntity* PTarget);
void AttackIntimidated(CBattleEntity* PEntity, const CBattleEntity* PTarget);

void AbilityParalyzed(CBattleEntity* PEntity, const CBattleEntity* PTarget);

void ItemInterrupt(CBattleEntity* PEntity);
void ItemParalyzed(CBattleEntity* PEntity, const CBattleEntity* PTarget);
void ItemIntimidated(CBattleEntity* PEntity, const CBattleEntity* PTarget);

void MagicInterrupt(CBattleEntity* PEntity, CSpell* PSpell);
void MagicParalyzed(CBattleEntity* PEntity, CSpell* PSpell, const CBattleEntity* PTarget);
void MagicIntimidated(CBattleEntity* PEntity, CSpell* PSpell, const CBattleEntity* PTarget);

}; // namespace ActionInterrupts
