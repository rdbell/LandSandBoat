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

#include "attack.h"
#include "attack_capacity.h"
#include "physical_hit_rate_capacity.h"
#include "seigan_counter_capacity.h"
#include "ai/ai_container.h"
#include "attackround.h"
#include "common/timer.h"
#include "common/utils.h"
#include "common/xirand.h"
#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "items/item_weapon.h"
#include "job_points.h"
#include "mob_modifier.h"
#include "status_effect_container.h"
#include "utils/puppetutils.h"

CAttack::CAttack(CBattleEntity* attacker, CBattleEntity* defender, PHYSICAL_ATTACK_TYPE type, PHYSICAL_ATTACK_DIRECTION direction, CAttackRound* attackRound)
: m_attacker(attacker)
, m_victim(defender)
, m_attackRound(attackRound)
, m_attackType(type)
, m_attackDirection(direction)
{
}

/************************************************************************
 *                                                                      *
 *  Returns the attack direction.                                       *
 *                                                                      *
 ************************************************************************/
PHYSICAL_ATTACK_DIRECTION CAttack::GetAttackDirection() const
{
    return m_attackDirection;
}

/************************************************************************
 *                                                                      *
 *  Returns the attack type.                                            *
 *                                                                      *
 ************************************************************************/
PHYSICAL_ATTACK_TYPE CAttack::GetAttackType() const
{
    return m_attackType;
}

/************************************************************************
 *                                                                      *
 *  Sets the attack type.                                               *
 *                                                                      *
 ************************************************************************/
void CAttack::SetAttackType(PHYSICAL_ATTACK_TYPE type)
{
    m_attackType = type;
}

/************************************************************************
 *                                                                      *
 *  Returns the isCritical flag.                                        *
 *                                                                      *
 ************************************************************************/
bool CAttack::IsCritical() const
{
    return m_isCritical;
}

/************************************************************************
 *                                                                      *
 *  Sets the critical flag.                                             *
 *                                                                      *
 ************************************************************************/
void CAttack::SetCritical(bool value)
{
    m_isCritical = value;

    const auto isDaken = m_attackType == PHYSICAL_ATTACK_TYPE::DAKEN;
    bool       hasSange = false;
    bool       hasSangeMerits = false;
    int16      enhancesSange = 0;
    int32      sangeMeritValue = 0;

    if (isDaken)
    {
        if (m_attacker->StatusEffectContainer)
        {
            const CStatusEffect* sangeEffect = m_attacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Sange);
            CCharEntity*         PChar       = dynamic_cast<CCharEntity*>(m_attacker);

            if (sangeEffect && PChar && PChar->PMeritPoints)
            {
                hasSange       = true;
                hasSangeMerits = true;
                enhancesSange  = PChar->getMod(Mod::ENHANCES_SANGE);
                sangeMeritValue = PChar->PMeritPoints->GetMeritValue(MERIT_SANGE, PChar);
            }
        }
    }

    CStatusEffect* footworkEffect = m_attackType == PHYSICAL_ATTACK_TYPE::KICK
                                        ? m_attacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Footwork)
                                        : nullptr;
    const auto modifiers = attackhelpers::ResolveCriticalRatioModifiers(
        isDaken,
        m_attackType == PHYSICAL_ATTACK_TYPE::KICK,
        footworkEffect != nullptr,
        footworkEffect != nullptr ? footworkEffect->GetSubPower() : 0,
        hasSange && hasSangeMerits,
        enhancesSange,
        sangeMeritValue);

    float rangedRatio = 0.0f;
    float meleeRatio  = 0.0f;

    if (isDaken)
    {
        rangedRatio = battleutils::GetRangedDamageRatio(m_attacker, m_victim, m_isCritical, modifiers.rangedAttackBonus);
    }
    else
    {

        SKILLTYPE skilltype  = SKILLTYPE::SKILL_NONE;
        SLOTTYPE  weaponSlot = static_cast<SLOTTYPE>(GetWeaponSlot());

        if (m_attacker->objtype == TYPE_PC)
        {
            if (auto* weapon = dynamic_cast<CItemWeapon*>(m_attacker->m_Weapons[weaponSlot]))
            {
                skilltype = static_cast<SKILLTYPE>(weapon->getSkillType());
            }
            else
            {
                skilltype = SKILLTYPE::SKILL_HAND_TO_HAND;
            }
        }

        // need to pass the weapon slot because damage ratio depends on ATT which varies by slot
        meleeRatio = battleutils::GetDamageRatio(m_attacker, m_victim, m_isCritical, modifiers.meleeAttackBonus, skilltype, weaponSlot, false);
    }

    const auto state = attackhelpers::ResolveCriticalState(value, isDaken, rangedRatio, meleeRatio);
    m_isCritical     = state.isCritical;
    m_damageRatio    = state.damageRatio;
}

/************************************************************************
 *                                                                      *
 *  Gets the evaded flag.                                               *
 *                                                                      *
 ************************************************************************/
bool CAttack::IsEvaded() const
{
    return m_isEvaded;
}

/************************************************************************
 *                                                                      *
 *  Sets the evaded flag.                                               *
 *                                                                      *
 ************************************************************************/
void CAttack::SetEvaded(bool value)
{
    m_isEvaded = value;
}

/************************************************************************
 *                                                                      *
 *  Gets the blocked flag.                                              *
 *                                                                      *
 ************************************************************************/
bool CAttack::IsBlocked() const
{
    return m_isBlocked;
}

bool CAttack::IsParried() const
{
    return m_isParried;
}

bool CAttack::IsGuarded() const
{
    return m_isGuarded;
}

bool CAttack::CheckGuarded()
{
    const auto resolution = attackhelpers::ResolveGuardCheck(
        attackutils::IsGuarded(m_attacker, m_victim),
        m_damageRatio);
    m_isGuarded     = resolution.guarded;
    m_damageRatio   = resolution.damageRatio;
    return m_isGuarded;
}

bool CAttack::CheckParried()
{
    const auto attackType = static_cast<uint8>(m_attackType);
    const bool parryProcs = !attackhelpers::ShouldSkipParryForDaken(attackType) &&
                            attackutils::IsParried(m_attacker, m_victim);
    m_isParried           = attackhelpers::ResolveParryCheck(m_isParried, attackType, parryProcs).parried;
    return m_isParried;
}

bool CAttack::IsAnticipated() const
{
    return m_anticipated;
}

bool CAttack::IsDeflected() const
{
    const bool hasDefenseBoost = m_victim->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::DefenseBoost);
    uint16     subpower        = 0;
    bool       inFront         = false;
    if (hasDefenseBoost)
    {
        subpower = m_victim->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::DefenseBoost)->GetSubPower();
        if (subpower != 0)
        {
            inFront = infront(m_attacker->loc.p, m_victim->loc.p, subpower);
        }
    }
    return attackhelpers::IsDeflected(hasDefenseBoost, subpower, inFront);
}

/************************************************************************
 *                                                                      *
 *  Returns the isFirstSwing flag.                                      *
 *                                                                      *
 ************************************************************************/
bool CAttack::IsFirstSwing() const
{
    return m_isFirstSwing;
}

/************************************************************************
 *                                                                      *
 *  Sets this swing as the first.                                       *
 *                                                                      *
 ************************************************************************/
void CAttack::SetAsFirstSwing(bool isFirst)
{
    m_isFirstSwing = isFirst;
}

/************************************************************************
 *                                                                      *
 *  Gets the damage ratio.                                              *
 *                                                                      *
 ************************************************************************/
float CAttack::GetDamageRatio() const
{
    return m_damageRatio;
}

/************************************************************************
 *                                                                      *
 *  Sets the attack type.                                               *
 *                                                                      *
 ************************************************************************/
uint8 CAttack::GetWeaponSlot()
{
    if (m_attackRound->IsH2H())
    {
        return SLOT_MAIN;
    }
    if (m_attackType == PHYSICAL_ATTACK_TYPE::DAKEN)
    {
        return SLOT_AMMO;
    }
    return m_attackDirection == RIGHTATTACK ? SLOT_MAIN : SLOT_SUB;
}

/************************************************************************
 *                                                                      *
 *  Returns the animation ID.                                           *
 *                                                                      *
 ************************************************************************/
uint16 CAttack::GetAnimationID()
{
    AttackAnimation animation{};

    // Try normal kick attacks (without footwork)
    if (this->m_attackType == PHYSICAL_ATTACK_TYPE::KICK)
    {
        animation = this->m_attackDirection == RIGHTATTACK ? AttackAnimation::RIGHTKICK : AttackAnimation::LEFTKICK;
    }

    else if (this->m_attackType == PHYSICAL_ATTACK_TYPE::DAKEN)
    {
        animation = AttackAnimation::THROW;
    }

    // Normal attack
    else
    {
        animation = this->m_attackDirection == RIGHTATTACK ? AttackAnimation::RIGHTATTACK : AttackAnimation::LEFTATTACK;
    }

    return static_cast<uint16>(animation);
}

/************************************************************************
 *                                                                      *
 *  Returns the hitrate for this swing.                                 *
 *                                                                      *
 ************************************************************************/
uint8 CAttack::GetHitRate()
{
    const auto path = attackhelpers::ResolveHitRatePath(
        static_cast<uint8>(m_attackType),
        static_cast<uint8>(m_attackDirection));

    switch (path)
    {
        case attackhelpers::HitRatePath::KickMelee:
        {
            m_hitRate = battleutils::GetHitRate(
                m_attacker,
                m_victim,
                attackhelpers::HitRateHandForPath(path));
            break;
        }
        case attackhelpers::HitRatePath::DakenRanged:
        {
            int16 accBonus = attackhelpers::SangeBaseAccBonus;
            if (m_attacker->StatusEffectContainer)
            {
                const CStatusEffect* sangeEffect = m_attacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Sange);
                CCharEntity*         PChar       = dynamic_cast<CCharEntity*>(m_attacker);
                const bool           hasSange    = sangeEffect != nullptr;
                const bool           hasMerits   = PChar != nullptr && PChar->PMeritPoints != nullptr;
                const int32          meritValue  = hasMerits ? PChar->PMeritPoints->GetMeritValue(MERIT_SANGE, PChar) : 0;
                accBonus                         = attackhelpers::ComputeSangeAccBonus(hasSange, hasMerits, meritValue);
            }
            m_hitRate = battleutils::GetRangedHitRate(m_attacker, m_victim, false, accBonus);
            break;
        }
        case attackhelpers::HitRatePath::RightMelee:
        case attackhelpers::HitRatePath::LeftMelee:
        {
            const auto hand   = attackhelpers::HitRateHandForPath(path);
            const auto accBon = attackhelpers::ZanshinAccBonusOrZero(static_cast<uint8>(m_attackType));
            if (accBon != 0)
            {
                m_hitRate = battleutils::GetHitRate(m_attacker, m_victim, hand, accBon);
            }
            else
            {
                m_hitRate = battleutils::GetHitRate(m_attacker, m_victim, hand);
            }

            // Deciding this here because SA/TA wears on attack, before the 2nd+ hits go off.
            if (attackhelpers::ShouldStampSATAOnPerfectHit(path, m_hitRate))
            {
                m_attackRound->SetSATA(true);
            }
            break;
        }
    }
    return m_hitRate;
}

/************************************************************************
 *                                                                      *
 *  Returns the damage for this swing.                                  *
 *                                                                      *
 ************************************************************************/
int32 CAttack::GetDamage() const
{
    return m_damage;
}

/************************************************************************
 *                                                                      *
 *  Sets the damage for this swing.                                     *
 *                                                                      *
 ************************************************************************/
void CAttack::SetDamage(int32 value)
{
    m_damage = value;
}

bool CAttack::CheckAnticipated()
{
    if (attackhelpers::ShouldSkipAnticipateForDaken(static_cast<uint8>(m_attackType)))
    {
        return false;
    }

    // bail out before product if we dont have TE
    CStatusEffect* thirdEyeEffect = m_victim->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::ThirdEye, 0);
    if (!attackhelpers::HasThirdEyeForAnticipate(thirdEyeEffect != nullptr))
    {
        return false;
    }

    const bool isPC = m_victim->objtype == TYPE_PC;
    bool       isTwoHanded = false;
    if (auto* weapon = dynamic_cast<CItemWeapon*>(m_victim->m_Weapons[SLOT_MAIN]))
    {
        isTwoHanded = weapon->isTwoHanded();
    }

    // Seigan+Third Eye counter (checkSeiganCounter pure).
    {
        const bool hasSeigan   = m_victim->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Seigan, 0);
        const bool isFacing64  = facing(m_victim->loc.p, m_attacker->loc.p, seigancounterhelpers::FacingCone);
        const bool isEngaged   = m_victim->PAI->IsEngaged();
        // Hit-rate factor: defender attacking original attacker (main hand).
        // Use GetHitRate percent / 100 to match getPhysicalHitRate scale after floor.
        const double hitRateFactor = static_cast<double>(battleutils::GetHitRate(m_victim, m_attacker, 0, 0)) / 100.0;
        const int    teCounterMod  = m_victim->getMod(Mod::THIRD_EYE_COUNTER_RATE);
        const int    roll          = xirand::GetRandomNumber(1, 101); // [1,100]

        m_isCountered = seigancounterhelpers::CheckSeiganCounter(
            true, // hasThirdEye — already gated
            hasSeigan,
            isFacing64,
            isEngaged,
            isPC,
            isTwoHanded,
            teCounterMod,
            hitRateFactor,
            roll);
        if (m_isCountered)
        {
            m_isCritical = (xirand::GetRandomNumber(100) < battleutils::GetCritHitRate(m_victim, m_attacker, false));
        }
    }

    // checkAnticipated retention: always anticipates when TE present; may delete TE.
    {
        const bool hasSeigan = m_victim->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Seigan, 0);
        const bool canRetain = physicalhitratehelpers::CanRetainThirdEye(isPC, isTwoHanded);

        std::int64_t timeInEffectMs = 0;
        if (hasSeigan && canRetain && thirdEyeEffect != nullptr)
        {
            const auto durationMs = static_cast<std::int64_t>(timer::count_milliseconds(thirdEyeEffect->GetDuration()));
            std::int64_t remainingMs = 0;
            if (thirdEyeEffect->GetDuration() > 0s)
            {
                const auto remaining = thirdEyeEffect->GetStartTime() - timer::now() + thirdEyeEffect->GetDuration();
                remainingMs          = std::max<std::int64_t>(timer::count_milliseconds(remaining), 0);
            }
            timeInEffectMs = durationMs - remainingMs;
            if (timeInEffectMs < 0)
            {
                timeInEffectMs = 0;
            }
        }

        const int retentionMod = m_victim->getMod(Mod::THIRD_EYE_RETENTION_RATE);
        const int roll         = xirand::GetRandomNumber(1, 10001); // [1,10000]
        const auto result      = physicalhitratehelpers::CheckAnticipatedRetention(
            hasSeigan, canRetain, timeInEffectMs, retentionMod, roll);

        m_anticipated = result.anticipated;
        if (result.shouldDeleteThirdEye)
        {
            m_victim->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::ThirdEye);
        }
        return m_anticipated;
    }
}

bool CAttack::IsSneakAttack() const
{
    return m_isSA;
}

bool CAttack::IsTrickAttack() const
{
    return m_isTA;
}

bool CAttack::IsCountered() const
{
    return m_isCountered;
}

bool CAttack::CheckCounter()
{
    // TODO return false if boost is active (when boost gets refactored to be current retail accurate)
    if (attackhelpers::ShouldSkipCounterForDaken(static_cast<uint8>(m_attackType)))
    {
        return false;
    }

    // Don't counter if not engaged or stunned, slept, etc.
    if (attackhelpers::ShouldBlockCounterForState(
            m_victim->PAI->IsEngaged(),
            m_victim->StatusEffectContainer->HasPreventActionEffect(true)))
    {
        m_isCountered = false;
        return m_isCountered;
    }

    uint8 meritCounter = 0;

    // Skip checking for counter merits if you're not on MNK
    if (attackhelpers::ShouldAddMNKCounterMerit(m_victim->objtype == TYPE_PC, m_victim->GetMJob() == JOB_MNK))
    {
        auto* PChar = static_cast<CCharEntity*>(m_victim);

        meritCounter = PChar->PMeritPoints->GetMeritValue(MERIT_COUNTER_RATE, PChar);
    }

    uint16 seiganChance = 0;

    if (m_victim->objtype == TYPE_PC && m_victim->getMod(Mod::SEIGAN_COUNTER_BONUS) > 0)
    {
        // counter check (rate AND your hit rate makes it land, else its just a regular hit)
        // having seigan active gives chance to counter at 25% of the zanshin proc rate
        auto* PChar              = static_cast<CCharEntity*>(m_victim);
        auto* weapon             = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[SLOT_MAIN]);
        bool  isValid2HandWeapon = weapon && weapon->isTwoHanded();
        bool  hasValidSeigan     = attackhelpers::IsValidSeiganForCounter(
            isValid2HandWeapon,
            PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Seigan, 0));

        seiganChance = attackhelpers::ComputeSeiganCounterChance(
            hasValidSeigan,
            PChar->getMod(Mod::ZANSHIN),
            PChar->PMeritPoints->GetMeritValue(MERIT_ZASHIN_ATTACK_RATE, PChar));
    }

    // Do not counter if PD is up
    if (!attackhelpers::ShouldSkipCounterForPerfectDodge(
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::PerfectDodge)))
    {
        const auto counterRate = attackhelpers::ClampCounterRate(m_victim->getMod(Mod::COUNTER), meritCounter);
        // Preserve short-circuit roll order: counter rate first, seigan only if counter fails.
        const bool counterRateProcs = xirand::GetRandomNumber(100) < counterRate;
        const bool seiganRateProcs  = !counterRateProcs && xirand::GetRandomNumber(100) < seiganChance;
        const bool rateAttempt      = attackhelpers::ShouldAttemptCounterRate(counterRateProcs, seiganRateProcs);
        const bool isFacing         = facing(m_victim->loc.p, m_attacker->loc.p, attackhelpers::CounterFacingArc);

        if (rateAttempt && isFacing)
        {
            if (xirand::GetRandomNumber(100) < battleutils::GetHitRate(m_victim, m_attacker))
            {
                m_isCountered = true;
                m_isCritical  = (xirand::GetRandomNumber(100) < battleutils::GetCritHitRate(m_victim, m_attacker, false));
            }
            else
            {
                m_attacker->PAI->EventHandler.triggerListener("MELEE_SWING_MISS", m_attacker, m_victim, this);
            }
        }
        else if (attackhelpers::ShouldPerfectCounter(
                     m_victim->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::PerfectCounter)))
        {
            // Perfect Counter only counters hits that normal counter misses, always critical, can counter 1-3 times before wearing
            // TODO: Perfect Counter can negate an attack even if it misses (No accuracy check yet)
            m_isCountered = true;
            m_isCritical  = true;

            // TODO: Implement VIT-based formula for Perfect Counter wearing off, and add JP bonus
            m_victim->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::PerfectCounter);
        }
    }
    return m_isCountered;
}

bool CAttack::IsCovered() const
{
    return m_isCovered;
}

bool CAttack::CheckCover()
{
    CBattleEntity* PCoverAbilityUser = m_attackRound->GetCoverAbilityUserEntity();
    const auto resolution = attackhelpers::ResolveCoverCheck(
        PCoverAbilityUser != nullptr,
        PCoverAbilityUser != nullptr && PCoverAbilityUser->isAlive());
    m_isCovered = resolution.covered;
    if (resolution.replaceVictim)
    {
        m_victim    = PCoverAbilityUser;
    }
    return m_isCovered;
}

/************************************************************************
 *                                                                      *
 *  Processes the damage for this swing.                                *
 *                                                                      *
 ************************************************************************/
void CAttack::ProcessDamage()
{
    // Pure attackhelpers assembly is production (slice 1577).
    // map.ENABLE_AUTO_ATTACK_LUA no longer routes to Lua calculateAttackDamage;
    // the pure path is the single retail-accurate implementation.

    // Sneak attack.
    if (attackhelpers::ShouldApplySneakAttack(
            m_attacker->GetMJob() == JOB_THF,
            m_isFirstSwing,
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::SneakAttack),
            behind(m_attacker->loc.p, m_victim->loc.p, 64),
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Hide),
            m_victim->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Doubt)))
    {
        m_bonusBasePhysicalDamage += attackhelpers::SneakAttackDexBonus(
            m_attacker->DEX(),
            m_attacker->getMod(Mod::SNEAK_ATK_DEX));
        m_isSA = true;
    }

    // Trick attack.
    if (attackhelpers::ShouldApplyTrickAttack(
            m_attacker->GetMJob() == JOB_THF,
            m_isFirstSwing,
            m_attackRound->GetTAEntity() != nullptr))
    {
        m_bonusBasePhysicalDamage += attackhelpers::TrickAttackAgiBonus(
            m_attacker->AGI(),
            m_attacker->getMod(Mod::TRICK_ATK_AGI));
        m_isTA = true;
    }

    // Consume mana
    if (attackhelpers::ShouldApplyConsumeMana(m_attacker->objtype == TYPE_PC))
    {
        m_bonusBasePhysicalDamage += battleutils::doConsumeManaEffect(static_cast<CCharEntity*>(m_attacker));
    }

    SLOTTYPE slot = static_cast<SLOTTYPE>(GetWeaponSlot());
    if (m_attackRound->IsH2H())
    {
        m_naturalH2hDamage = attackhelpers::NaturalH2HDamage(m_attacker->GetSkill(SKILL_HAND_TO_HAND));
        m_baseDamage       = m_attacker->GetMainWeaponDmg();
        int32 kickDamage   = 0;

        if (m_attacker->objtype == TYPE_MOB)
        {
            // Mobs use a different base damage formula than players.
            // H2H attacks from mobs have a base damage penalty applied based on what zone they are in.
            int32       fSTR     = battleutils::GetFSTR(m_attacker, m_victim, slot);
            REGION_TYPE regionID = m_attacker->loc.zone->GetRegionID();
            float       mobH2HPenalty = attackhelpers::SelectMobH2HPenalty(
                static_cast<CMobEntity*>(m_attacker)->getMobMod(MOBMOD_NO_H2H_PENALTY) != 0,
                regionID <= REGION_TYPE::LIMBUS);

            m_damage = m_baseDamage + m_bonusBasePhysicalDamage;
            kickDamage = m_attacker->getMod(Mod::KICK_DMG);

            float preRatio = attackhelpers::AssembleMobH2HDamagePreRatio(
                m_damage,
                attackhelpers::IsKickAttackType(static_cast<uint8>(m_attackType)),
                kickDamage,
                fSTR,
                mobH2HPenalty);
            m_damage = attackhelpers::FloorAtZero(static_cast<int32>(preRatio));
            m_damage = attackhelpers::ApplyDamageRatio(m_damage, m_damageRatio);
        }
        else if (attackhelpers::IsKickAttackType(static_cast<uint8>(m_attackType))) // Players use this calculation.
        {
            // KICK_DMG includes weapon dmg if footwork is active
            m_damage = attackhelpers::AssemblePlayerH2HKickPreRatio(
                m_naturalH2hDamage,
                m_attacker->getMod(Mod::KICK_DMG),
                static_cast<int32>(m_bonusBasePhysicalDamage),
                battleutils::GetFSTR(m_attacker, m_victim, slot));
            m_damage = attackhelpers::ApplyDamageRatio(m_damage, m_damageRatio);
        }
        else // Players use this calculation.
        {
            m_damage = attackhelpers::AssemblePlayerH2HPunchPreRatio(
                m_baseDamage,
                m_naturalH2hDamage,
                static_cast<int32>(m_bonusBasePhysicalDamage),
                battleutils::GetFSTR(m_attacker, m_victim, slot));
            m_damage = attackhelpers::ApplyDamageRatio(m_damage, m_damageRatio);
        }
    }
    else if (slot == SLOT_MAIN)
    {
        m_damage = attackhelpers::AssembleMainHandPreRatio(
            m_attacker->GetMainWeaponDmg(),
            static_cast<int32>(m_bonusBasePhysicalDamage),
            battleutils::GetFSTR(m_attacker, m_victim, slot));
        m_damage = attackhelpers::ApplyDamageRatio(m_damage, m_damageRatio);
    }
    else if (slot == SLOT_SUB)
    {
        m_damage = attackhelpers::AssembleSubHandPreRatio(
            m_attacker->GetSubWeaponDmg(),
            static_cast<int32>(m_bonusBasePhysicalDamage),
            battleutils::GetFSTR(m_attacker, m_victim, slot));
        m_damage = attackhelpers::ApplyDamageRatio(m_damage, m_damageRatio);
    }
    else if (slot == SLOT_AMMO)
    {
        // GetFSTR uses slot to determine fSTR vs fSTR2
        m_damage = attackhelpers::AssembleRangedAmmoPreRatio(
            m_attacker->GetRangedWeaponDmg(),
            battleutils::GetFSTR(m_attacker, m_victim, slot));
        m_damage = attackhelpers::ApplyDamageRatio(m_damage, m_damageRatio);
    }

    // Apply Scarlet Delirium damage bonus
    // xi::StatusEffect::ScarletDelirium1 is only active after damage has been dealt to the DRK and xi::StatusEffect::ScarletDelirium has been removed.
    if (attackhelpers::ShouldApplyScarletDelirium(
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::ScarletDelirium1)))
    {
        float effectPower = attackhelpers::ScarletDeliriumMultiplier(
            m_attacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::ScarletDelirium1)->GetPower());
        m_damage = attackhelpers::FloorProduct(m_damage, effectPower);
    }

    // Apply "Double Attack" damage and "Triple Attack" damage mods
    if (attackhelpers::ShouldApplyDoubleAttackDamage(static_cast<uint8>(m_attackType), m_attacker->objtype == TYPE_PC))
    {
        m_damage = attackhelpers::ApplyDoubleTripleAttackDamage(
            m_damage,
            m_attacker->getMod(Mod::DOUBLE_ATTACK_DMG));
    }
    else if (attackhelpers::ShouldApplyTripleAttackDamage(static_cast<uint8>(m_attackType), m_attacker->objtype == TYPE_PC))
    {
        m_damage = attackhelpers::ApplyDoubleTripleAttackDamage(
            m_damage,
            m_attacker->getMod(Mod::TRIPLE_ATTACK_DMG));
    }

    // Soul eater.
    if (attackhelpers::ShouldApplySoulEater(m_attacker->objtype == TYPE_PC))
    {
        m_damage = battleutils::doSoulEaterEffect(static_cast<CCharEntity*>(m_attacker), m_damage);
    }

    // Set attack type to Samba if the attack type is normal.  Don't overwrite other types.  Used for Samba double damage.
    if (attackhelpers::ShouldPromoteNormalToSamba(
            static_cast<uint8>(m_attackType),
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::DrainSamba),
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AspirSamba),
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::HasteSamba)))
    {
        SetAttackType(PHYSICAL_ATTACK_TYPE::SAMBA);
    }

    // Get player-only damage multipliers.
    if (auto* PChar = dynamic_cast<CCharEntity*>(m_attacker))
    {
        m_damage = attackutils::CheckForDamageMultiplier(PChar, dynamic_cast<CItemWeapon*>(m_attacker->m_Weapons[slot]), m_damage, m_attackType, slot, m_isFirstSwing);
    }

    // Apply Sneak Attack Augment Mod
    if (attackhelpers::ShouldApplySAAugment(
            m_attacker->getMod(Mod::AUGMENTS_SA),
            IsSneakAttack(),
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::SneakAttack)))
    {
        m_damage = attackhelpers::FloorProduct(
            m_damage,
            attackhelpers::AugmentDamageMultiplier(m_attacker->getMod(Mod::AUGMENTS_SA)));
    }

    // Apply Trick Attack Augment Mod
    if (attackhelpers::ShouldApplyTAAugment(
            m_attacker->getMod(Mod::AUGMENTS_TA),
            IsTrickAttack(),
            m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::TrickAttack)))
    {
        m_damage = attackhelpers::FloorProduct(
            m_damage,
            attackhelpers::AugmentDamageMultiplier(m_attacker->getMod(Mod::AUGMENTS_TA)));
    }

    // low level mobs can get negative fSTR so low they crater their (base weapon damage + fstr) to below 0.
    // TODO: find out proper fSTR calc for low level mobs when your VIT is ridiculously high. It's likely that this is slightly wrong (possibly you'd get more hits for 0 than you should)
    // However, there are legitimate strategies on retail with 1 dmg weapons and negative fSTR ranks that result in all auto attacks hitting for 0 but using enspells for damage so no TP is fed.
    // Absorption isn't possible at this point in the calculation, so zero it.
    m_damage = attackhelpers::ClampNonNegativeDamage(m_damage);

    // Try skill up. Pure routing: attackhelpers::PlanAttackSkillUp (slice 2773).
    {
        const bool isPC            = m_attacker->objtype == TYPE_PC;
        const bool isDaken         = m_attackType == PHYSICAL_ATTACK_TYPE::DAKEN;
        const bool hasWeaponAtSlot = dynamic_cast<CItemWeapon*>(m_attacker->m_Weapons[slot]) != nullptr;
        const bool isPet           = m_attacker->objtype == TYPE_PET;
        const bool masterIsPC      = m_attacker->PMaster != nullptr && m_attacker->PMaster->objtype == TYPE_PC;
        const bool isAutomaton     = isPet && static_cast<CPetEntity*>(m_attacker)->getPetType() == PET_TYPE::AUTOMATON;

        const auto skillUp = attackhelpers::PlanAttackSkillUp(
            m_damage,
            isPC,
            isDaken,
            hasWeaponAtSlot,
            isPet,
            masterIsPC,
            isAutomaton);

        switch (skillUp.target)
        {
            case attackhelpers::AttackSkillUpTarget::Throwing:
            {
                charutils::TrySkillUP(
                    static_cast<CCharEntity*>(m_attacker),
                    SKILLTYPE::SKILL_THROWING,
                    m_victim->GetMLevel());
                break;
            }
            case attackhelpers::AttackSkillUpTarget::WeaponSkillType:
            {
                if (auto* weapon = dynamic_cast<CItemWeapon*>(m_attacker->m_Weapons[slot]))
                {
                    charutils::TrySkillUP(
                        static_cast<CCharEntity*>(m_attacker),
                        static_cast<SKILLTYPE>(weapon->getSkillType()),
                        m_victim->GetMLevel());
                }
                break;
            }
            case attackhelpers::AttackSkillUpTarget::AutomatonMelee:
            {
                puppetutils::TrySkillUP(
                    static_cast<CAutomatonEntity*>(m_attacker),
                    SKILL_AUTOMATON_MELEE,
                    m_victim->GetMLevel());
                break;
            }
            default:
                break;
        }
    }
    m_isBlocked = attackutils::IsBlocked(m_attacker, m_victim);

    // Apply Restraint Weaponskill Damage Modifier
    // Effect power tracks the total bonus
    // Effect sub power tracks remainder left over from whole percentage flooring
    // Pure math: attackhelpers::ResolveRestraintWSDBoost (slice 2764).
    if (m_isFirstSwing && m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Restraint))
    {
        CStatusEffect* effect = m_attacker->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Restraint);

        if (effect == nullptr)
        {
            ShowError("Restraint effect was null.");
            return;
        }

        uint8 jpBonus = 0;
        if (m_attacker->objtype == TYPE_PC)
        {
            jpBonus = static_cast<CCharEntity*>(m_attacker)->PJobPoints->GetJobPointValue(JP_RESTRAINT_EFFECT) * 2;
        }

        const auto plan = attackhelpers::ResolveRestraintWSDBoost(
            m_isFirstSwing,
            true,
            effect->GetPower(),
            effect->GetSubPower(),
            m_attacker->GetWeaponDelay(false),
            m_attacker->getMod(Mod::ENHANCES_RESTRAINT),
            jpBonus);

        if (plan.applies)
        {
            effect->SetPower(effect->GetPower() + plan.boostAmount);
            effect->SetSubPower(plan.newSubPower);
            m_attacker->addModifier(Mod::ALL_WSDMG_FIRST_HIT, plan.boostAmount);
        }
    }
}
