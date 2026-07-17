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

#include "attackround.h"
#include "attackround_capacity.h"
#include "ai/ai_container.h"
#include "items/item_weapon.h"
#include "mob_modifier.h"
#include "packets/s2c/0x01d_item_same.h"
#include "status_effect_container.h"

/************************************************************************
 *                                                                        *
 *  Constructor.                                                            *
 *                                                                        *
 ************************************************************************/
CAttackRound::CAttackRound(CBattleEntity* attacker, CBattleEntity* defender)
{
    m_attacker          = attacker;
    m_defender          = defender;
    m_kickAttackOccured = false;
    m_sataOccured       = false;

    // Grab a trick attack assistant.
    m_taEntity = battleutils::getAvailableTrickAttackChar(attacker, attacker->GetBattleTarget());

    // Get cover partner
    if (attacker->GetBattleTarget()->objtype == TYPE_PC)
    {
        m_coverAbilityUserEntity = battleutils::GetCoverAbilityUser(attacker->GetBattleTarget(), attacker);
    }
    else
    {
        m_coverAbilityUserEntity = nullptr;
    }

    auto* PMain = dynamic_cast<CItemWeapon*>(attacker->m_Weapons[SLOT_MAIN]);
    auto* PSub  = dynamic_cast<CItemWeapon*>(attacker->m_Weapons[SLOT_SUB]);

    bool       h2hSingleSwing = false;
    const bool isH2H          = PMain != nullptr && IsH2H();
    if (isH2H)
    {
        if (dynamic_cast<CMobEntity*>(m_attacker))
        {
            h2hSingleSwing = static_cast<CMobEntity*>(m_attacker)->getMobMod(MOBMOD_H2H_SINGLE_SWING) > 0;
        }
    }

    const auto plan = attackroundhelpers::PlanInitialWeaponAttacks(
        PMain != nullptr,
        isH2H,
        h2hSingleSwing,
        PSub != nullptr,
        PSub != nullptr && attacker->IsDualWielding());
    for (uint8 i = 0; i < plan.mainAttackCalls; ++i)
    {
        CreateAttacks(PMain, plan.mainUsesLeftDirection ? LEFTATTACK : RIGHTATTACK);
    }
    if (plan.createSubAttack)
    {
        CreateAttacks(PSub, LEFTATTACK);
    }

    // Build kick attacks.
    CreateKickAttacks();

    // Build Daken throw
    CreateDakenAttack();

    // Append follow-up attacks
    ProcFollowUpAttacks();

    // Set the first attack flag
    m_attackSwings[0].SetAsFirstSwing();

    // Delete the haste samba effect.
    attacker->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::HasteSambaHaste);
}

/************************************************************************
 *                                                                        *
 *  Destructor.                                                            *
 *                                                                        *
 ************************************************************************/
CAttackRound::~CAttackRound() = default;

/************************************************************************
 *                                                                        *
 *  Returns the attack swing count.                                        *
 *                                                                        *
 ************************************************************************/
uint8 CAttackRound::GetAttackSwingCount()
{
    return (uint8)m_attackSwings.size();
}

/************************************************************************
 *                                                                        *
 *  Returns an attack via index.                                            *
 *                                                                        *
 ************************************************************************/
CAttack& CAttackRound::GetAttack(uint8 index)
{
    return m_attackSwings[index];
}

/************************************************************************
 *                                                                        *
 *  Returns the current attack.                                            *
 *                                                                        *
 ************************************************************************/
CAttack& CAttackRound::GetCurrentAttack()
{
    return m_attackSwings[0];
}

/************************************************************************
 *                                                                        *
 *  Sets the SATA flag.                                                    *
 *                                                                        *
 ************************************************************************/
void CAttackRound::SetSATA(bool value)
{
    m_sataOccured = value;
}

/************************************************************************
 *                                                                        *
 *  Returns the SATA flag.                                                *
 *                                                                        *
 ************************************************************************/
bool CAttackRound::GetSATAOccured() const
{
    return m_sataOccured;
}

/************************************************************************
 *                                                                        *
 *  Returns the TA entity.                                                *
 *                                                                        *
 ************************************************************************/
CBattleEntity* CAttackRound::GetTAEntity()
{
    return m_taEntity;
}

/************************************************************************
 *                                                                       *
 *  Returns the Cover entity.                                            *
 *                                                                       *
 ************************************************************************/
CBattleEntity* CAttackRound::GetCoverAbilityUserEntity()
{
    return m_coverAbilityUserEntity;
}

/************************************************************************
 *                                                                       *
 *  Returns the H2H flag.                                                *
 *                                                                       *
 ************************************************************************/
bool CAttackRound::IsH2H()
{
    if (auto* weapon = dynamic_cast<CItemWeapon*>(m_attacker->m_Weapons[SLOT_MAIN]))
    {
        return weapon->getSkillType() == SKILL_HAND_TO_HAND;
    }
    return false;
}

/************************************************************************
 *                                                                        *
 *  Adds an attack swing.                                                *
 *                                                                        *
 ************************************************************************/
void CAttackRound::AddAttackSwing(PHYSICAL_ATTACK_TYPE type, PHYSICAL_ATTACK_DIRECTION direction, uint8 count)
{
    if (m_attackSwings.size() < MAX_ATTACKS)
    {
        for (size_t i = 0; i < count; ++i)
        {
            // Flip direction of second H2H swing
            if (IsH2H() && m_attackSwings.size() == 1)
            {
                m_attackSwings.emplace_back(m_attacker, m_defender, type, RIGHTATTACK, this);
            }
            else
            {
                m_attackSwings.emplace_back(m_attacker, m_defender, type, direction, this);
            }

            if (m_attackSwings.size() == MAX_ATTACKS)
            {
                return;
            }
        }
    }
}

/************************************************************************
 *                                                                        *
 *  Deletes the first attack in the list.                                *
 *                                                                        *
 ************************************************************************/
void CAttackRound::DeleteAttackSwing()
{
    m_attackSwings.erase(m_attackSwings.begin());
}

/************************************************************************
 *                                                                       *
 *  Creates up to many attacks for a particular hand.                    *
 *                                                                       *
 ************************************************************************/
void CAttackRound::CreateAttacks(CItemWeapon* PWeapon, PHYSICAL_ATTACK_DIRECTION direction)
{
    if (!PWeapon)
    {
        return;
    }

    uint8 num = 1;

    bool isPC = m_attacker->objtype == TYPE_PC;

    // Checking the players weapon hit count
    if (attackroundhelpers::ShouldUseWeaponHitCount(PWeapon->getReqLvl(), m_attacker->GetMLevel()))
    {
        num = PWeapon->getHitCount();
    }

    // Existance of "Occasionally attacks X times" overwrites PWeapon hit count
    if (attackroundhelpers::ShouldApplyMaxSwingsMod(isPC, static_cast<uint8>(m_attacker->getMod(Mod::MAX_SWINGS))))
    {
        auto modSwings = attackroundhelpers::ClampMaxSwings(static_cast<uint8>(m_attacker->getMod(Mod::MAX_SWINGS)));
        num            = battleutils::getHitCount(modSwings);
    }

    // If the attacker is a mobentity or derived from mobentity, check to see if it has any special mutli-hit capabilties
    if (dynamic_cast<CMobEntity*>(m_attacker))
    {
        auto multiHitMax = (uint8) static_cast<CMobEntity*>(m_attacker)->getMobMod(MOBMOD_MULTI_HIT);

        if (attackroundhelpers::ShouldApplyMobMultiHit(multiHitMax))
        {
            num = attackroundhelpers::MobMultiHitSwingCount(battleutils::getHitCount(multiHitMax));
        }
    }

    // Checking the players triple, double and quadruple attack
    int16 tripleAttack = m_attacker->getMod(Mod::TRIPLE_ATTACK);
    int16 doubleAttack = m_attacker->getMod(Mod::DOUBLE_ATTACK);
    int16 quadAttack   = m_attacker->getMod(Mod::QUAD_ATTACK);
    const bool isMainHand = attackroundhelpers::IsMainHandForMultiHit(
        IsH2H(), m_attackSwings.empty(), direction == RIGHTATTACK);

    // Checking for Mythic Weapon Aftermath
    int16 occAttThriceRate = attackroundhelpers::ClampAttackRate(m_attacker->getMod(Mod::MYTHIC_OCC_ATT_THRICE));
    int16 occAttTwiceRate  = attackroundhelpers::ClampAttackRate(m_attacker->getMod(Mod::MYTHIC_OCC_ATT_TWICE));

    // Checking for merit upgrades
    if (isPC)
    {
        CCharEntity* PChar = (CCharEntity*)m_attacker;

        // Merit chance only applies if player has the job trait
        if (charutils::hasTrait(PChar, TRAIT_TRIPLE_ATTACK))
        {
            tripleAttack += PChar->PMeritPoints->GetMeritValue(MERIT_TRIPLE_ATTACK_RATE, PChar);
        }

        // Ambush Augment adds +1% Triple Attack per merit (need to satisfy conditions for Ambush)
        {
            const int16 rotDiff = static_cast<int16>(std::abs(
                static_cast<int>(m_defender->loc.p.rotation) - static_cast<int>(m_attacker->loc.p.rotation)));
            if (attackroundhelpers::ShouldApplyAmbushTripleBonus(
                    charutils::hasTrait(PChar, TRAIT_AMBUSH),
                    PChar->getMod(Mod::AUGMENTS_AMBUSH) > 0,
                    attackroundhelpers::AmbushRotationInWindow(rotDiff)))
            {
                tripleAttack += PChar->PMeritPoints->GetMerit(MERIT_AMBUSH)->count;
            }
        }

        if (charutils::hasTrait(PChar, TRAIT_DOUBLE_ATTACK))
        {
            doubleAttack += PChar->PMeritPoints->GetMeritValue(MERIT_DOUBLE_ATTACK_RATE, PChar);
        }
        // TODO: Quadruple attack merits when SE release them.

        // Iga Garb +2 Set augment: possibility to add another swing while using Dual Wield
        if (attackroundhelpers::ShouldAddOffhandExtraDualWield(isMainHand))
        {
            doubleAttack += m_attacker->getMod(Mod::EXTRA_DUAL_WIELD_ATTACK);
        }
    }

    quadAttack   = attackroundhelpers::ClampAttackRate(quadAttack);
    doubleAttack = attackroundhelpers::ClampAttackRate(doubleAttack);
    tripleAttack = attackroundhelpers::ClampAttackRate(tripleAttack);

    // Preference: Mikage > exclusive ladder (QA/TA/DA/mythic/occasional) > default.
    // Daken is CreateDakenAttack(); Zanshin is battle_entity.
    const bool weaponIsMain = m_attacker->m_Weapons[SLOT_MAIN] != nullptr &&
                              m_attacker->m_Weapons[SLOT_MAIN]->getID() == PWeapon->getID();
    const bool hasMikage = m_attacker->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Mikage);

    bool multiHitOccurred = false;

    if (attackroundhelpers::ShouldAddMikageSwings(hasMikage, weaponIsMain))
    {
        auto shadows = static_cast<uint8>(m_attacker->getMod(Mod::UTSUSEMI));
        AddAttackSwing(PHYSICAL_ATTACK_TYPE::NORMAL, direction, shadows);
    }
    else
    {
        // Host rolls for exclusive ladder (order preserved: QA then TA then DA then mythic thrice/twice).
        // Preserve original short-circuit roll order and isMainHand mythic gate
        // so RNG consumption matches retail path (else-if chain).
        const bool quadProcs   = xirand::GetRandomNumber(100) < quadAttack;
        const bool tripleProcs = !quadProcs && xirand::GetRandomNumber(100) < tripleAttack;
        const bool doubleProcs = !quadProcs && !tripleProcs && xirand::GetRandomNumber(100) < doubleAttack;
        const bool mythicThriceProcs = isMainHand && !quadProcs && !tripleProcs && !doubleProcs &&
                                       xirand::GetRandomNumber(100) < occAttThriceRate;
        const bool mythicTwiceProcs  = isMainHand && !quadProcs && !tripleProcs && !doubleProcs &&
                                       !mythicThriceProcs && xirand::GetRandomNumber(100) < occAttTwiceRate;

        const auto pref = attackroundhelpers::ResolveExclusiveMultiHitPreference(
            quadProcs, tripleProcs, doubleProcs, isMainHand, mythicThriceProcs, mythicTwiceProcs, num);

        if (attackroundhelpers::ShouldApplyExclusiveMultiHitSwings(pref))
        {
            const auto swings = attackroundhelpers::ExclusiveMultiHitSwingCount(pref, num);
            const auto atype  = static_cast<PHYSICAL_ATTACK_TYPE>(
                attackroundhelpers::ExclusiveMultiHitAttackType(pref));
            AddAttackSwing(atype, direction, swings);
            multiHitOccurred = attackroundhelpers::MultiHitOccurred(pref);
        }
    }

    // Additional swing modifier (stacks!), mostly for Amood weapons
    if (attackroundhelpers::ShouldAddAdditionalSwing(
            isPC, xirand::GetRandomNumber(100) < m_attacker->getMod(Mod::ADDITIONAL_SWING_CHANCE)))
    {
        AddAttackSwing(PHYSICAL_ATTACK_TYPE::NORMAL, direction, 1);
    }

    // Default hit when exclusive QA/TA/DA did not consume the preference path.
    if (attackroundhelpers::ShouldAddDefaultHit(multiHitOccurred))
    {
        AddAttackSwing(PHYSICAL_ATTACK_TYPE::NORMAL, direction, 1);
    }
}

/************************************************************************
 *  IsAttackTypeEligibleForFollowUp()
 *  Return true if the attackType attack swing eligible to proc followUpType
 *  Virtue Stone, TODO Raetic, TODO Dynamis [D]
 ************************************************************************/
bool CAttackRound::IsAttackTypeEligibleForFollowUp(Mod followUpType, PHYSICAL_ATTACK_TYPE attackType)
{
    return attackroundhelpers::IsAttackTypeEligibleForFollowUp(
        followUpType == Mod::AMMO_SWING,
        static_cast<uint8>(attackType));
}

/************************************************************************
 *  ProcFollowUpAttacks() - Players only
 *  Attempt to proc follow-up attacks and append them to the attack round
 *  Virtue Stone, TODO Raetic, TODO Dynamis [D]
 ************************************************************************/
void CAttackRound::ProcFollowUpAttacks()
{
    if (CCharEntity* PChar = dynamic_cast<CCharEntity*>(m_attacker))
    {
        if (attackroundhelpers::ShouldProcFollowUpForChar(true, PChar->getMod(Mod::AMMO_SWING) != 0))
        {
            // iterate through attackSwings and attempt to proc and store a follow-up swing
            for (auto& attack : m_attackSwings)
            {
                PHYSICAL_ATTACK_DIRECTION direction = attack.GetAttackDirection();
                PHYSICAL_ATTACK_TYPE      type      = attack.GetAttackType();
                CItemEquipment*           PWeapon   = nullptr;

                if (IsAttackTypeEligibleForFollowUp(Mod::AMMO_SWING, type))
                {
                    if (attackroundhelpers::ShouldUseMainWeaponForFollowUp(IsH2H(), direction == RIGHTATTACK))
                    {
                        PWeapon = PChar->getEquip(SLOT_MAIN);
                    }
                    else if (attackroundhelpers::ShouldUseSubWeaponForFollowUp(direction == LEFTATTACK))
                    {
                        PWeapon = PChar->getEquip(SLOT_SUB);
                    }

                    if (attackroundhelpers::ShouldProcAmmoSwing(
                            PWeapon != nullptr,
                            PWeapon != nullptr && xirand::GetRandomNumber(100) < battleutils::GetScaledItemModifier(PChar, PWeapon, Mod::AMMO_SWING)))
                    {
                        CItemEquipment* PAmmo = PChar->getEquip(SLOT_AMMO);

                        if (PAmmo != nullptr &&
                            attackroundhelpers::IsVirtueStoneAmmo(PAmmo->getID(), PAmmo->getQuantity()))
                        {
                            auto  eloc = PChar->equipLocation(SLOT_AMMO);
                            uint8 loc  = eloc ? static_cast<uint8>(eloc->Container) : 0;
                            uint8 slot = eloc ? eloc->Slot : 0;

                            if (AddFollowUpAttack(direction))
                            {
                                if (attackroundhelpers::ShouldUnequipAmmoAfterConsume(PAmmo->getQuantity()))
                                {
                                    charutils::UnequipItem(PChar, SLOT_AMMO);
                                    PChar->RequestPersist(CHAR_PERSIST::EQUIP);
                                }

                                charutils::UpdateItem(PChar, loc, slot, -1);
                                PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
                            }
                        }
                    }
                }
            }
        }
        // TODO: else if (Raetic) {};
        // TODO: else if (Dynamis [D]) {};

        // Append any swings stored in m_followUpSwings to the attack round
        if (attackroundhelpers::ShouldAppendStoredFollowUps(!m_followUpSwings.empty()))
        {
            for (size_t i = 0; i < m_followUpSwings.size(); i++)
            {
                AddAttackSwing(PHYSICAL_ATTACK_TYPE::FOLLOWUP, m_followUpSwings[i], 1);
            }
        }
    }
}

/************************************************************************
 *  AddFollowUpAttack() - (Virtue Stone, Raetic, Dynamis [D])
 *  Attempt to store a follow-up swing. Return true if swing is stored.
 *  Ensure that one follow-up per hand is stored, in order
 ************************************************************************/
bool CAttackRound::AddFollowUpAttack(PHYSICAL_ATTACK_DIRECTION direction)
{
    if (attackroundhelpers::CanStoreFollowUpSwing(
            m_followUpSwings.size(),
            m_followUpSwings.empty(),
            !m_followUpSwings.empty() && m_followUpSwings.back() != direction))
    {
        m_followUpSwings.push_back(direction);
        return true;
    }

    return false;
}

/************************************************************************
 *                                                                       *
 *  Creates kick attacks.                                                *
 *                                                                       *
 ************************************************************************/
void CAttackRound::CreateKickAttacks()
{
    if (attackroundhelpers::ShouldCreateKickAttacks(IsH2H()))
    {
        // kick attack mod (All jobs)
        uint16 kickAttack = m_attacker->getMod(Mod::KICK_ATTACK_RATE);

        if (attackroundhelpers::ShouldAddMNKKickMerit(
                m_attacker->GetMJob() == JOB_MNK,
                m_attacker->objtype == TYPE_PC)) // MNK (Main job)
        {
            kickAttack += ((CCharEntity*)m_attacker)->PMeritPoints->GetMeritValue(MERIT_KICK_ATTACK_RATE, (CCharEntity*)m_attacker);
        }

        kickAttack = attackroundhelpers::ClampKickAttackRate(kickAttack);

        if (attackroundhelpers::ShouldProcKickAttack(xirand::GetRandomNumber(100) < kickAttack))
        {
            AddAttackSwing(PHYSICAL_ATTACK_TYPE::KICK, RIGHTATTACK, 1);
            m_kickAttackOccured = true;
        }

        // Tantra set mod: Try an extra left kick attack.
        if (attackroundhelpers::ShouldProcExtraKick(
                m_kickAttackOccured,
                xirand::GetRandomNumber(100) < m_attacker->getMod(Mod::EXTRA_KICK_ATTACK)))
        {
            AddAttackSwing(PHYSICAL_ATTACK_TYPE::KICK, LEFTATTACK, 1);
        }
    }
}

/************************************************************************
 *                                                                        *
 *  Creates a Daken throw.                                                *
 *                                                                        *
 ************************************************************************/
void CAttackRound::CreateDakenAttack()
{
    if (attackroundhelpers::ShouldCreateDakenAttack(m_attacker->objtype == TYPE_PC))
    {
        auto* PAmmo = static_cast<CItemWeapon*>(m_attacker->m_Weapons[SLOT_AMMO]);
        const bool isShuriken = PAmmo != nullptr && PAmmo->isShuriken();
        uint16     daken      = m_attacker->getMod(Mod::DAKEN);
        if (attackroundhelpers::ShouldProcDakenThrow(isShuriken, xirand::GetRandomNumber(100) < daken))
        {
            AddAttackSwing(PHYSICAL_ATTACK_TYPE::DAKEN, RIGHTATTACK, 1);
        }
    }
}
