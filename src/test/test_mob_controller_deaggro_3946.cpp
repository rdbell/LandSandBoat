#include "test_mob_controller_deaggro_3946.h"

#include "map/ai/controllers/mob_controller_deaggro_capacity.h"
#include "map/ai/controllers/mob_controller_detection_capacity.h"
#include "map/ai/controllers/mob_controller_readiness_capacity.h"
#include "map/ai/controllers/mob_controller_movement_capacity.h"
#include "map/ai/controllers/mob_controller_aggro_capacity.h"
#include "map/ai/helpers/gambits_tp_trigger_capacity.h"
#include "map/ai/controllers/mob_controller_follow_capacity.h"
#include "map/ai/controllers/mob_controller_spell_admission_capacity.h"
#include "map/ai/controllers/mob_controller_move_range_capacity.h"
#include "map/ai/controllers/mob_controller_target_validity_capacity.h"
#include "map/ai/controllers/player_controller_engage_capacity.h"
#include "map/ai/controllers/player_controller_weaponskill_capacity.h"
#include "map/ai/controllers/player_controller_ability_recast_capacity.h"
#include "map/ai/controllers/player_controller_action_gate_capacity.h"
#include "map/ai/controllers/player_controller_ability_gate_capacity.h"
#include "map/ai/controllers/trust_controller_noncombat_follow_capacity.h"
#include "map/ai/controllers/trust_controller_tick_capacity.h"
#include "map/ai/controllers/trust_controller_target_sync_capacity.h"
#include "map/ai/controllers/trust_controller_engage_capacity.h"
#include "map/ai/controllers/trust_controller_roam_formation_capacity.h"
#include "map/ai/controllers/trust_controller_recovery_capacity.h"
#include "map/ai/controllers/trust_controller_ranged_attack_capacity.h"
#include "map/ai/controllers/trust_controller_cast_coordination_capacity.h"
#include "map/ai/controllers/trust_controller_reposition_capacity.h"
#include "map/ai/controllers/trust_controller_ability_capacity.h"
#include "map/ai/controllers/trust_controller_noncombat_movement_capacity.h"
#include "map/ai/controllers/trust_controller_combat_movement_capacity.h"
#include "map/ai/controllers/player_charm_controller_roam_capacity.h"
#include "map/ai/controllers/player_charm_controller_combat_capacity.h"
#include "map/ai/controllers/player_charm_controller_tick_capacity.h"
#include "map/ai/controllers/pet_controller_tick_capacity.h"
#include "map/ai/controllers/pet_controller_deaggro_capacity.h"
#include "map/ai/controllers/pet_controller_healing_capacity.h"
#include "map/ai/controllers/pet_controller_buff_tick_capacity.h"
#include "map/ai/controllers/pet_controller_master_loss_capacity.h"
#include "map/ai/controllers/pet_controller_immobile_capacity.h"
#include "map/ai/controllers/pet_controller_healing_roam_capacity.h"

#include <iostream>

auto runMobControllerDeaggro3946SelfTests() -> bool
{
    using mobcontrollerdeaggro::CanPursueByScent;
    using mobcontrollerdeaggro::ShouldDeaggroForHide;
    using mobcontrollerdeaggro::ShouldDeaggroForLock;
    using mobcontrollerdetection::Evaluate;
    using mobcontrollerreadiness::SpecialSkillReady;
    using mobcontrollerreadiness::SpellReady;
    using mobcontrollermovement::CanMoveForward;
    using mobcontrolleraggro::CanAggroTarget;
    using gambitstptrigger::Evaluate;
    using gambitstptrigger::Trigger;
    using mobcontrollerfollow::SetTarget;
    using mobcontrollerfollow::Type;
    using mobcontrollerspelladmission::CanCastSpells;
    using mobcontrollermoverange::Resolve;
    using mobcontrollertargetvalidity::ShouldDeaggroNoTarget;
    using mobcontrollertargetvalidity::TargetInvalid;
    using playercontrollerengage::Evaluate;
    using playercontrollerengage::Error;
    using WeaponSkillError = playercontrollerweaponskill::Error;
    using playercontrollerabilityrecast::RemainingSeconds;
    using playercontrolleractiongate::Cast;
    using playercontrolleractiongate::Ranged;
    using playercontrolleractiongate::Item;
    using ActionError = playercontrolleractiongate::Error;
    using AbilityError = playercontrollerabilitygate::Error;
    using trustcontrollernoncombatfollow::Resolve;
    using trustcontrollertick::Route;
    using trustcontrollertargetsync::ShouldSync;
    using trustcontrollerengage::ShouldEngage;
    using trustcontrollerroamformation::Action;

    const auto base = std::chrono::steady_clock::time_point{};
    const bool detectionOK = Evaluate(false, false, false, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).shouldDeaggro &&
                             !Evaluate(false, false, false, true, false, base + std::chrono::seconds(25) - std::chrono::nanoseconds(1), base, std::chrono::seconds(0)).shouldDeaggro &&
                             !Evaluate(false, false, false, false, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).shouldDeaggro &&
                             !Evaluate(false, false, false, true, false, base + std::chrono::seconds(27), base, std::chrono::seconds(3)).shouldDeaggro &&
                             Evaluate(false, false, false, true, false, base + std::chrono::seconds(28), base, std::chrono::seconds(3)).shouldDeaggro &&
                             Evaluate(false, false, false, true, true, base + std::chrono::seconds(25), base, std::chrono::hours(1)).shouldDeaggro &&
                             Evaluate(true, false, false, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).tapDeaggro &&
                             Evaluate(false, true, false, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).tapDeaggro &&
                             Evaluate(false, false, true, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).tapDeaggro;
    const bool readinessOK = SpellReady(true, false, false, 0, 0, base, base + std::chrono::hours(1), std::chrono::seconds(0)) &&
                             SpellReady(false, true, false, 0, 0, base, base + std::chrono::hours(1), std::chrono::seconds(0)) &&
                             !SpellReady(false, false, true, 3, 3, base + std::chrono::hours(1), base, std::chrono::seconds(0)) &&
                             SpellReady(false, false, false, 0, 0, base + std::chrono::seconds(10), base + std::chrono::seconds(10), std::chrono::seconds(0)) &&
                             SpellReady(false, false, false, 5.1f, 0, base + std::chrono::seconds(8), base + std::chrono::seconds(10), std::chrono::seconds(2)) &&
                             !SpellReady(false, false, true, 5.1f, 0, base + std::chrono::seconds(8), base + std::chrono::seconds(10), std::chrono::seconds(2)) &&
                             !SpecialSkillReady(false, false, 0, base + std::chrono::hours(1), base, std::chrono::seconds(0), std::chrono::seconds(0)) &&
                             !SpecialSkillReady(true, true, 0, base + std::chrono::hours(1), base, std::chrono::seconds(0), std::chrono::seconds(0)) &&
                             SpecialSkillReady(true, false, 0, base + std::chrono::seconds(10), base + std::chrono::seconds(5), std::chrono::seconds(5), std::chrono::seconds(0)) &&
                             SpecialSkillReady(true, false, 5.1f, base + std::chrono::seconds(8), base + std::chrono::seconds(5), std::chrono::seconds(5), std::chrono::seconds(2));
    const bool movementOK = CanMoveForward(0, false, 0, 0, false, false, false, 0, false, 0, 0, 0, false, false, false, 0, 0) &&
                            !CanMoveForward(0, false, 0, 19, true, true, true, 0, false, 0, 0, 0, false, false, false, 0, 0) &&
                            CanMoveForward(0, true, 10, 19, true, true, true, 0, false, 0, 0, 0, false, false, false, 0, 0) &&
                            CanMoveForward(0, false, 0, 19, false, false, false, 70, false, 70, 70, 1, false, true, false, 0, 0) &&
                            !CanMoveForward(0, false, 0, 19, false, false, false, 70, false, 70, 70, 1, true, true, false, 0, 0) &&
                            !CanMoveForward(0, false, 0, 19, false, false, false, 70, false, 70, 70, 1, false, true, true, 0, 0) &&
                            CanMoveForward(0, false, 0, 0, false, true, false, 0, false, 0, 0, 0, false, false, false, 0, 0) &&
                            !CanMoveForward(0, false, 0, 0, false, false, false, 0, false, 0, 0, 0, false, false, false, 10, 10.1f);
    const bool aggroOK = CanAggroTarget(true, true, false, true, false, false, false, false, false, false, false, 0, false, false, false, false, false, true, false, true) &&
                         !CanAggroTarget(false, true, false, true, false, false, false, false, false, false, false, 0, false, false, false, false, false, true, false, true) &&
                         !CanAggroTarget(true, false, false, true, false, false, false, false, false, false, false, 0, false, false, false, false, false, true, false, true) &&
                         !CanAggroTarget(true, true, false, false, false, false, false, false, false, false, false, 0, false, false, false, false, false, true, false, true) &&
                         CanAggroTarget(true, true, true, false, false, false, false, false, false, false, false, 0, false, false, false, false, false, true, false, true) &&
                         !CanAggroTarget(true, true, false, true, false, false, false, true, false, true, true, 7, false, false, false, false, false, true, false, true) &&
                         !CanAggroTarget(true, true, false, true, false, false, false, false, false, false, false, 0, true, true, false, false, false, true, false, true) &&
                         !CanAggroTarget(true, true, false, true, false, false, false, false, false, false, false, 0, false, false, true, false, false, true, false, true) &&
                         !CanAggroTarget(true, true, false, true, false, false, false, false, false, false, false, 0, false, false, false, false, true, true, false, true);
    const bool tpTriggerOK = !Evaluate(false, 3000, Trigger::ASAP, 0, 0, false, false, false).ready &&
                             Evaluate(true, 3000, Trigger::Closer, 0, 0, false, false, false).ready &&
                             Evaluate(true, 0, Trigger::ASAP, 0, 0, false, false, false).ready &&
                             Evaluate(true, 1000, Trigger::Random, 1, 999, false, false, false).ready &&
                             Evaluate(true, 1000, Trigger::Random, 1, 999, false, false, false).value == 1000 &&
                             !Evaluate(true, 1000, Trigger::Random, 1000, 1000, false, false, false).ready &&
                             Evaluate(true, 0, Trigger::Opener, 1, 0, true, false, false).ready &&
                             Evaluate(true, 0, Trigger::Closer, 0, 0, false, true, true).ready &&
                             Evaluate(true, 1500, Trigger::CloserUntilTP, 1, 0, false, false, false).ready &&
                             Evaluate(true, 1500, Trigger::CloserUntilTP, 1, 0, false, false, false).value == 1500;
    const auto followSame = SetTarget(true, Type::Roam, true, Type::Roam, true, false, std::chrono::seconds(1));
    const auto followSet = SetTarget(false, Type::None, true, Type::RunAway, false, false, std::chrono::seconds(0));
    const auto followClear = SetTarget(true, Type::Roam, false, Type::None, false, true, std::chrono::seconds(1));
    const bool followOK = followSame.hasTarget && followSame.type == Type::Roam && !followSame.notifyFollow &&
                          followSet.hasTarget && followSet.notifyFollow &&
                          !followClear.hasTarget && followClear.type == Type::None && followClear.neutral &&
                          followClear.neutralAfter == std::chrono::seconds(31) && followClear.notifyUnfollow && followClear.clearOwnerAndEnmity;
    const bool spellAdmissionOK = CanCastSpells(true, false, false, false, false, true, false, true) &&
                                  !CanCastSpells(false, false, false, false, false, true, false, true) &&
                                  !CanCastSpells(true, true, false, false, false, true, false, true) &&
                                  !CanCastSpells(true, false, true, false, false, true, false, true) &&
                                  !CanCastSpells(true, false, false, true, true, true, false, true) &&
                                  !CanCastSpells(true, false, false, false, false, false, false, true) &&
                                  !CanCastSpells(true, false, false, false, false, true, false, false) &&
                                  CanCastSpells(true, false, false, false, false, true, true, false);
    const auto baseRange = Resolve(5, 0, false, 0, 0);
    const auto skillRange = Resolve(5, 9, false, 0, 0);
    const auto rangedRange = Resolve(5, 9, true, 20, 0);
    const auto offsetRange = Resolve(5, 0, false, 0, 15);
    const auto clampedRange = Resolve(1, 0, false, 0, 20);
    const bool moveRangeOK = baseRange.attackRange == 5 && baseRange.closeDistance == 4.6f &&
                             skillRange.attackRange == 9 && skillRange.closeDistance == 8.6f &&
                             rangedRange.attackRange == 20 && rangedRange.closeDistance == 19.6f &&
                             offsetRange.closeDistance == 3.5f && clampedRange.closeDistance == 0;
    const bool targetValidityOK = ShouldDeaggroNoTarget(false, true) && !ShouldDeaggroNoTarget(false, false) &&
                                  !TargetInvalid(true, true, false, true, true, true, true, true, true, true) &&
                                  TargetInvalid(false, true, false, true, true, true, true, true, true, true) &&
                                  TargetInvalid(true, false, false, true, true, true, true, true, true, true) &&
                                  TargetInvalid(true, true, true, true, true, true, true, true, true, true) &&
                                  TargetInvalid(true, true, false, false, true, true, true, true, true, true) &&
                                  TargetInvalid(true, true, false, true, false, true, true, true, true, true) &&
                                  TargetInvalid(true, true, false, true, true, false, true, true, true, true) &&
                                  TargetInvalid(true, true, false, true, true, true, false, true, true, true) &&
                                  TargetInvalid(true, true, false, true, true, true, true, false, true, true) &&
                                  TargetInvalid(true, true, false, true, true, true, true, true, false, true) &&
                                  TargetInvalid(true, true, false, true, true, true, true, true, true, false);
    const auto engageBase = std::chrono::steady_clock::time_point{};
    const bool playerEngageOK = !Evaluate(false, 0, engageBase, std::chrono::seconds(0), engageBase).dispatch &&
                                Evaluate(true, 29, engageBase, std::chrono::seconds(1), engageBase + std::chrono::seconds(2)).dispatch &&
                                Evaluate(true, 30, engageBase, std::chrono::seconds(0), engageBase + std::chrono::seconds(1)).error == Error::TooFar &&
                                Evaluate(true, 29, engageBase, std::chrono::seconds(1), engageBase + std::chrono::seconds(1)).error == Error::WaitLonger;
    const bool playerWeaponSkillOK = playercontrollerweaponskill::Evaluate(true, true, true, true, true, false, 0, 1000, false, false, true, false, true, false).dispatch &&
                                     playercontrollerweaponskill::Evaluate(false, true, true, true, true, false, 0, 1000, false, false, true, false, true, false).error == WeaponSkillError::Unable &&
                                     playercontrollerweaponskill::Evaluate(true, true, true, false, true, false, 0, 1000, false, false, true, false, true, false).error == WeaponSkillError::CannotUse &&
                                     playercontrollerweaponskill::Evaluate(true, true, true, true, true, true, 0, 1000, false, false, true, false, true, false).error == WeaponSkillError::CannotUseAny &&
                                     playercontrollerweaponskill::Evaluate(true, true, true, true, true, false, 0, 999, false, false, true, false, true, false).error == WeaponSkillError::NotEnoughTP &&
                                     playercontrollerweaponskill::Evaluate(true, true, true, true, true, false, 0, 1000, true, false, true, false, true, false).error == WeaponSkillError::NoRangedWeapon &&
                                     !playercontrollerweaponskill::Evaluate(true, true, true, true, true, false, 0, 1000, false, false, true, true, true, false).dispatch &&
                                     playercontrollerweaponskill::Evaluate(true, true, true, true, true, false, 0, 1000, false, false, true, false, false, false).error == WeaponSkillError::CannotSee &&
                                     playercontrollerweaponskill::Evaluate(true, true, true, true, true, false, 0, 1000, false, false, true, false, false, true).dispatch;
    const auto recastBase = std::chrono::steady_clock::time_point{};
    const bool abilityRecastOK = RemainingSeconds(recastBase + std::chrono::seconds(10) + std::chrono::milliseconds(1), recastBase + std::chrono::seconds(5), std::chrono::seconds(1), std::chrono::seconds(0), 0) == 7 &&
                                 RemainingSeconds(recastBase + std::chrono::seconds(10), recastBase + std::chrono::seconds(5), std::chrono::seconds(1), std::chrono::seconds(2), 1) == 6 &&
                                 RemainingSeconds(recastBase + std::chrono::seconds(10), recastBase + std::chrono::seconds(5), std::chrono::seconds(1), std::chrono::seconds(2), 3) == 2 &&
                                 RemainingSeconds(recastBase + std::chrono::seconds(1), recastBase + std::chrono::seconds(5), std::chrono::seconds(0), std::chrono::seconds(0), 0) == 0;
    const bool playerActionGateOK = Cast(true, false, false).dispatch && Cast(true, true, false).error == ActionError::Unable &&
                                    !Cast(true, false, true).dispatch && Ranged(true, true, false).dispatch &&
                                    Ranged(false, true, false).error == ActionError::WaitLonger && !Ranged(true, true, true).dispatch &&
                                    Item(true, true, false).dispatch && !Item(false, true, false).dispatch && !Item(true, true, true).dispatch;
    const bool playerAbilityGateOK = playercontrollerabilitygate::Evaluate(true, true, true, false, false).dispatch &&
                                     playercontrollerabilitygate::Evaluate(false, true, true, false, false).error == AbilityError::Unable &&
                                     playercontrollerabilitygate::Evaluate(true, true, false, false, false).error == AbilityError::Unable &&
                                     playercontrollerabilitygate::Evaluate(true, true, true, true, false).error == AbilityError::Recast &&
                                     !playercontrollerabilitygate::Evaluate(true, true, true, false, true).dispatch;
    const auto firstTrustFollow = Resolve(0, false, false);
    const auto laterTrustFollow = Resolve(2, true, false);
    const auto missingTrustFollow = Resolve(2, false, false);
    const auto selfTrustFollow = Resolve(1, true, true);
    const bool trustFollowOK = firstTrustFollow.followMaster && firstTrustFollow.distance == 3.0f &&
                               laterTrustFollow.followPrevious && laterTrustFollow.distance == 4.0f &&
                               missingTrustFollow.followMaster && selfTrustFollow.followMaster;
    const bool trustTickOK = trustcontrollertick::Resolve(false, false, false, false, false, false) == Route::None &&
                             trustcontrollertick::Resolve(true, true, false, false, false, false) == Route::Despawn &&
                             trustcontrollertick::Resolve(true, false, true, true, false, false) == Route::NonCombat &&
                             trustcontrollertick::Resolve(true, false, false, false, true, false) == Route::Combat &&
                             trustcontrollertick::Resolve(true, false, false, false, false, false) == Route::Roam &&
                             trustcontrollertick::Resolve(true, false, false, false, false, true) == Route::None;
    const bool trustTargetSyncOK = ShouldSync(true, true, 1, 0) &&
                                   !ShouldSync(false, true, 1, 0) &&
                                   !ShouldSync(true, false, 1, 0) &&
                                   !ShouldSync(true, true, 1, -1);
    const bool trustEngageOK = ShouldEngage(true, true, false, 1, 0) &&
                               ShouldEngage(true, true, true, 0, 0) &&
                               !ShouldEngage(true, true, false, 0, 0) &&
                               ShouldEngage(true, true, true, 2, 0) &&
                               !ShouldEngage(false, true, false, 1, 0) &&
                               !ShouldEngage(true, false, true, 0, 0) &&
                               !ShouldEngage(true, true, true, 1, trustcontrollerengage::CorneliaModelID);
    const auto firstDeclump = trustcontrollerroamformation::Resolve(0, 0.9f, false);
    const auto laterDeclump = trustcontrollerroamformation::Resolve(1, 1.4f, false);
    const auto firstThreshold = trustcontrollerroamformation::Resolve(0, 1.0f, false);
    const auto firstPath = trustcontrollerroamformation::Resolve(0, 2.1f, false);
    const auto laterPath = trustcontrollerroamformation::Resolve(1, 3.6f, false);
    const auto firstStep = trustcontrollerroamformation::Resolve(0, 9.0f, false);
    const auto warpThreshold = trustcontrollerroamformation::Resolve(1, 30.0f, false);
    const auto laterWarp = trustcontrollerroamformation::Resolve(1, 30.1f, false);
    const auto firstClear = trustcontrollerroamformation::Resolve(0, 1.5f, true);
    const auto laterNone = trustcontrollerroamformation::Resolve(1, 3.0f, false);
    const bool trustRoamFormationOK = firstDeclump.action == Action::Declump && firstDeclump.targetDistance == 1.5f &&
                                      laterDeclump.action == Action::Declump && laterDeclump.targetDistance == 3.0f &&
                                      firstThreshold.action == Action::None &&
                                      firstPath.action == Action::Path && laterPath.action == Action::Path &&
                                      firstStep.action == Action::Step && warpThreshold.action == Action::Step && laterWarp.action == Action::Warp &&
                                      firstClear.action == Action::Clear && laterNone.action == Action::None;
    const bool trustRecoveryOK = trustcontrollerrecovery::Resolve(true, true, std::chrono::seconds(16), std::chrono::seconds(16), std::chrono::seconds(16), 0).recover &&
                                 trustcontrollerrecovery::Resolve(true, true, std::chrono::seconds(16), std::chrono::seconds(16), std::chrono::seconds(16), 0).nextHealingTick == 1 &&
                                 !trustcontrollerrecovery::Resolve(true, true, std::chrono::seconds(15), std::chrono::seconds(16), std::chrono::seconds(16), 0).recover &&
                                 !trustcontrollerrecovery::Resolve(true, true, std::chrono::seconds(16), std::chrono::seconds(15), std::chrono::seconds(16), 0).recover &&
                                 trustcontrollerrecovery::Resolve(true, true, std::chrono::seconds(16), std::chrono::seconds(16), std::chrono::seconds(11), 1).nextHealingTick == 2 &&
                                 !trustcontrollerrecovery::Resolve(true, false, std::chrono::seconds(16), std::chrono::seconds(16), std::chrono::seconds(16), 2).recover &&
                                 trustcontrollerrecovery::Resolve(true, true, std::chrono::seconds(16), std::chrono::seconds(16), std::chrono::seconds(4), 3).nextHealingTick == 3 &&
                                 trustcontrollerrecovery::Amount(101) == 5;
    const bool trustRangedAttackOK = trustcontrollerrangedattack::CanStart(std::chrono::seconds(10) + std::chrono::nanoseconds(1), false, std::chrono::seconds(0), false) &&
                                      !trustcontrollerrangedattack::CanStart(std::chrono::seconds(10), false, std::chrono::seconds(0), false) &&
                                      trustcontrollerrangedattack::CanStart(std::chrono::seconds(3) + std::chrono::nanoseconds(1), true, std::chrono::seconds(3), false) &&
                                      !trustcontrollerrangedattack::CanStart(std::chrono::seconds(3), true, std::chrono::seconds(3), false) &&
                                      !trustcontrollerrangedattack::CanStart(std::chrono::seconds(11), false, std::chrono::seconds(0), true);
    const bool trustCastCoordinationOK = !trustcontrollercastcoordination::CanCast(true, false, false, false, 1, 10, 1, 11, false, 0) &&
                                         trustcontrollercastcoordination::CanCast(true, false, false, false, 1, 12, 1, 11, false, 0) &&
                                         !trustcontrollercastcoordination::CanCast(false, true, false, false, 0, 0, 0, 0, true, 51) &&
                                         trustcontrollercastcoordination::CanCast(false, true, false, false, 0, 0, 0, 0, true, 50) &&
                                         trustcontrollercastcoordination::CanCast(false, true, false, false, 0, 0, 0, 0, false, 99) &&
                                         !trustcontrollercastcoordination::CanCast(false, false, true, false, 2, 20, 2, 21, false, 0) &&
                                         trustcontrollercastcoordination::CanCast(false, false, true, false, 2, 20, 3, 99, false, 0) &&
                                         !trustcontrollercastcoordination::CanCast(false, false, false, true, 4, 30, 4, 30, false, 0) &&
                                         trustcontrollercastcoordination::CanCast(false, false, false, true, 4, 31, 4, 30, false, 0);
    const bool trustRepositionOK = trustcontrollerreposition::ShouldSelect(7.4f, 10.0f, true, std::chrono::seconds(3) + std::chrono::nanoseconds(1), false) &&
                                   trustcontrollerreposition::ShouldSelect(12.6f, 10.0f, true, std::chrono::seconds(4), false) &&
                                   !trustcontrollerreposition::ShouldSelect(7.5f, 10.0f, true, std::chrono::seconds(4), false) &&
                                   trustcontrollerreposition::ShouldSelect(10.0f, 10.0f, false, std::chrono::seconds(4), false) &&
                                   !trustcontrollerreposition::ShouldSelect(7.0f, 10.0f, true, std::chrono::seconds(3), false) &&
                                   !trustcontrollerreposition::ShouldSelect(7.0f, 10.0f, true, std::chrono::seconds(4), true) &&
                                   trustcontrollerreposition::ShouldPath(2.1f, 0) &&
                                   !trustcontrollerreposition::ShouldPath(2.0f, 0) &&
                                   trustcontrollerreposition::ShouldPath(3.0f, 2) &&
                                   !trustcontrollerreposition::ShouldPath(3.0f, 3);
    const bool trustAbilityOK = trustcontrollerability::CanUse(false, true) &&
                                !trustcontrollerability::CanUse(true, true) &&
                                !trustcontrollerability::CanUse(false, false) &&
                                !trustcontrollerability::CanUse(true, false);
    const bool trustNonCombatMovementOK = trustcontrollernoncombatmovement::Resolve(3.0f, 3.0f) == trustcontrollernoncombatmovement::Action::Hold &&
                                         trustcontrollernoncombatmovement::Resolve(3.1f, 3.0f) == trustcontrollernoncombatmovement::Action::Path &&
                                         trustcontrollernoncombatmovement::Resolve(9.0f, 3.0f) == trustcontrollernoncombatmovement::Action::Step &&
                                         trustcontrollernoncombatmovement::Resolve(10.0f, 3.0f) == trustcontrollernoncombatmovement::Action::Step &&
                                         trustcontrollernoncombatmovement::Resolve(30.0f, 3.0f) == trustcontrollernoncombatmovement::Action::Step &&
                                         trustcontrollernoncombatmovement::Resolve(30.1f, 3.0f) == trustcontrollernoncombatmovement::Action::Warp &&
                                         trustcontrollernoncombatmovement::Resolve(8.9f, 3.0f) == trustcontrollernoncombatmovement::Action::Path;
    const auto noFollowMovement = trustcontrollercombatmovement::Resolve(false, true, 0, 0.0f, 0.0f, false);
    const auto noSpeedMovement = trustcontrollercombatmovement::Resolve(true, false, 0, 0.0f, 0.0f, false);
    const auto noMoveMaster = trustcontrollercombatmovement::Resolve(true, true, -1, 0.0f, 15.1f, false);
    const auto noMoveTarget = trustcontrollercombatmovement::Resolve(true, true, -1, 15.1f, 15.0f, false);
    const auto noMoveBoundary = trustcontrollercombatmovement::Resolve(true, true, -1, 15.0f, 15.0f, false);
    const auto nonCombatMovement = trustcontrollercombatmovement::Resolve(true, true, -2, 20.0f, 0.0f, false);
    const auto meleeCanAttack = trustcontrollercombatmovement::Resolve(true, true, 0, 5.0f, 0.0f, true);
    const auto meleePath = trustcontrollercombatmovement::Resolve(true, true, 0, 3.1f, 0.0f, false);
    const auto meleeStep = trustcontrollercombatmovement::Resolve(true, true, 0, 9.0f, 0.0f, false);
    const auto rangedMovement = trustcontrollercombatmovement::Resolve(true, true, 12, 0.0f, 0.0f, false);
    const bool trustCombatMovementOK = noFollowMovement.action == trustcontrollercombatmovement::Action::Hold &&
                                       noSpeedMovement.action == trustcontrollercombatmovement::Action::Hold &&
                                       noMoveMaster.action == trustcontrollercombatmovement::Action::PathOut && noMoveMaster.desiredDistance == 9.0f &&
                                       noMoveTarget.action == trustcontrollercombatmovement::Action::PathOut &&
                                       noMoveBoundary.action == trustcontrollercombatmovement::Action::Hold &&
                                       nonCombatMovement.action == trustcontrollercombatmovement::Action::Hold &&
                                       meleeCanAttack.action == trustcontrollercombatmovement::Action::Hold &&
                                       meleePath.action == trustcontrollercombatmovement::Action::MeleePath && meleePath.desiredDistance == 3.0f &&
                                       meleeStep.action == trustcontrollercombatmovement::Action::MeleeStep &&
                                       rangedMovement.action == trustcontrollercombatmovement::Action::PathOut && rangedMovement.desiredDistance == 12.0f;
    const auto charmEngaged = playercharmcontrollerroam::Resolve(true, true, true, 2.1f);
    const auto charmInRange = playercharmcontrollerroam::Resolve(false, true, true, 2.1f);
    const auto charmPath = playercharmcontrollerroam::Resolve(false, true, true, 2.2f);
    const auto charmBoundary = playercharmcontrollerroam::Resolve(false, true, true, 35.0f);
    const auto charmWarp = playercharmcontrollerroam::Resolve(false, true, true, 40.0f);
    const auto charmNoPathFinder = playercharmcontrollerroam::Resolve(false, false, true, 40.0f);
    const auto charmNoSpeed = playercharmcontrollerroam::Resolve(false, true, false, 40.0f);
    const bool playerCharmRoamOK = charmEngaged.engageMasterTarget && charmEngaged.action == playercharmcontrollerroam::Action::Hold &&
                                   charmInRange.action == playercharmcontrollerroam::Action::Hold &&
                                   charmPath.action == playercharmcontrollerroam::Action::Path &&
                                   charmBoundary.action == playercharmcontrollerroam::Action::Warp &&
                                   charmWarp.action == playercharmcontrollerroam::Action::Warp &&
                                   charmNoPathFinder.action == playercharmcontrollerroam::Action::Hold &&
                                   charmNoSpeed.action == playercharmcontrollerroam::Action::Hold;
    const auto charmCombatStopAndSync = playercharmcontrollercombat::Resolve(false, true, false, false, false, false);
    const auto charmCombatNoTarget = playercharmcontrollercombat::Resolve(true, false, false, false, false, false);
    const auto charmCombatAttack = playercharmcontrollercombat::Resolve(true, false, true, true, true, false);
    const auto charmCombatPursue = playercharmcontrollercombat::Resolve(true, false, true, true, false, true);
    const auto charmCombatNoFollow = playercharmcontrollercombat::Resolve(true, false, true, false, false, true);
    const auto charmCombatNoSpeed = playercharmcontrollercombat::Resolve(true, false, true, true, false, false);
    const bool playerCharmCombatOK = charmCombatStopAndSync.disengage && charmCombatStopAndSync.syncTarget &&
                                     !charmCombatNoTarget.disengage && !charmCombatNoTarget.syncTarget &&
                                     charmCombatAttack.lookAtTarget && !charmCombatAttack.pursueTarget &&
                                     charmCombatPursue.lookAtTarget && charmCombatPursue.pursueTarget &&
                                     !charmCombatNoFollow.lookAtTarget && !charmCombatNoFollow.pursueTarget &&
                                     charmCombatNoSpeed.lookAtTarget && !charmCombatNoSpeed.pursueTarget;
    const bool playerCharmTickOK = playercharmcontrollertick::Resolve(false, false, false) == playercharmcontrollertick::Route::RemoveCharm &&
                                   playercharmcontrollertick::Resolve(true, false, false) == playercharmcontrollertick::Route::RemoveCharm &&
                                   playercharmcontrollertick::Resolve(true, true, true) == playercharmcontrollertick::Route::Combat &&
                                   playercharmcontrollertick::Resolve(true, true, false) == playercharmcontrollertick::Route::Roam;
    const bool petTickOK = !petcontrollertick::ShouldDespawnForCharm(false, true, true) &&
                           petcontrollertick::ShouldDespawnForCharm(true, true, true) &&
                           !petcontrollertick::ShouldDespawnForCharm(true, true, false) &&
                           petcontrollertick::ShouldDespawnForJug(true, true, true, true, true) &&
                           !petcontrollertick::ShouldDespawnForJug(true, true, false, true, true) &&
                           !petcontrollertick::ShouldDespawnForJug(true, true, true, false, true) &&
                           !petcontrollertick::ShouldDespawnForJug(false, true, true, true, true);
    const bool petDeaggroOK = petcontrollerdeaggro::ShouldDeaggro(false, false, false, true, true, true) &&
                               !petcontrollerdeaggro::ShouldDeaggro(true, false, false, true, true, true) &&
                               petcontrollerdeaggro::ShouldDeaggro(true, true, false, true, true, true) &&
                               petcontrollerdeaggro::ShouldDeaggro(true, false, true, true, true, true) &&
                               petcontrollerdeaggro::ShouldDeaggro(true, false, false, false, true, true) &&
                               petcontrollerdeaggro::ShouldDeaggro(true, false, false, true, false, true) &&
                               petcontrollerdeaggro::ShouldDeaggro(true, false, false, true, true, false);
    const auto petStartHealing = petcontrollerhealing::Resolve(true, false, false);
    const auto petPreventedHealing = petcontrollerhealing::Resolve(true, false, true);
    const auto petAlreadyHealing = petcontrollerhealing::Resolve(true, true, false);
    const auto petStopHealing = petcontrollerhealing::Resolve(false, true, false);
    const auto petStanding = petcontrollerhealing::Resolve(false, false, false);
    const bool petHealingOK = petStartHealing.start && petStartHealing.isHealing &&
                              !petPreventedHealing.start && petPreventedHealing.isHealing &&
                              !petAlreadyHealing.start && petAlreadyHealing.isHealing &&
                              petStopHealing.stop && !petStopHealing.isHealing &&
                              !petStanding.start && !petStanding.stop && !petStanding.isHealing;
    const bool petBuffTickOK = petcontrollerbufftick::AllowsBuffTick(true, true) &&
                               !petcontrollerbufftick::AllowsBuffTick(true, false) &&
                               !petcontrollerbufftick::AllowsBuffTick(false, true) &&
                               !petcontrollerbufftick::AllowsBuffTick(false, false);
    const bool petMasterLossOK = petcontrollermasterloss::ShouldDie(false, false, true, false) &&
                                 petcontrollermasterloss::ShouldDie(true, true, true, false) &&
                                 !petcontrollermasterloss::ShouldDie(true, false, true, false) &&
                                 !petcontrollermasterloss::ShouldDie(false, false, false, false) &&
                                 !petcontrollermasterloss::ShouldDie(false, false, true, true) &&
                                 !petcontrollermasterloss::ShouldDie(true, true, true, true);
    const bool petImmobileOK = petcontrollerimmobile::IsImmobile(75) &&
                               petcontrollerimmobile::IsImmobile(17) &&
                               petcontrollerimmobile::IsImmobile(18) &&
                               petcontrollerimmobile::IsImmobile(19) &&
                               !petcontrollerimmobile::IsImmobile(1) &&
                               !petcontrollerimmobile::IsImmobile(65535);
    const bool petHealingRoamOK = petcontrollerhealingroam::ShouldHold(true, true) &&
                                  !petcontrollerhealingroam::ShouldHold(true, false) &&
                                  !petcontrollerhealingroam::ShouldHold(false, true) &&
                                  !petcontrollerhealingroam::ShouldHold(false, false);

    const bool scentOK = CanPursueByScent(true, false, true, false, false) &&
                         !CanPursueByScent(false, false, true, false, false) &&
                         !CanPursueByScent(true, true, true, false, false) &&
                         !CanPursueByScent(true, false, false, false, false) &&
                         !CanPursueByScent(true, false, true, true, false) &&
                         !CanPursueByScent(true, false, true, false, true);
    const bool hideOK = ShouldDeaggroForHide(true, true, false, false, false) &&
                        !ShouldDeaggroForHide(false, true, false, false, false) &&
                        !ShouldDeaggroForHide(true, false, false, false, false) &&
                        !ShouldDeaggroForHide(true, true, true, false, false) &&
                        !ShouldDeaggroForHide(true, true, false, true, false) &&
                        !ShouldDeaggroForHide(true, true, false, false, true);
    const bool lockOK = ShouldDeaggroForLock(true, false, true, false, false, false) &&
                        ShouldDeaggroForLock(false, true, false, true, true, false) &&
                        !ShouldDeaggroForLock(true, false, false, false, false, false) &&
                        !ShouldDeaggroForLock(false, true, false, false, true, false) &&
                        !ShouldDeaggroForLock(true, false, true, false, false, true);
    if (!scentOK || !detectionOK || !readinessOK || !movementOK || !aggroOK || !tpTriggerOK || !followOK || !spellAdmissionOK || !moveRangeOK || !targetValidityOK || !playerEngageOK || !playerWeaponSkillOK || !abilityRecastOK || !playerActionGateOK || !playerAbilityGateOK || !trustFollowOK || !trustTickOK || !trustTargetSyncOK || !trustEngageOK || !trustRoamFormationOK || !trustRecoveryOK || !trustRangedAttackOK || !trustCastCoordinationOK || !trustRepositionOK || !trustAbilityOK || !trustNonCombatMovementOK || !trustCombatMovementOK || !playerCharmRoamOK || !playerCharmCombatOK || !playerCharmTickOK || !petTickOK || !petDeaggroOK || !petHealingOK || !petBuffTickOK || !petMasterLossOK || !petImmobileOK || !petHealingRoamOK || !hideOK || !lockOK)
    {
        std::cerr << "mob controller deaggro 3946 self-test failed\n";
    }
    return scentOK && detectionOK && readinessOK && movementOK && aggroOK && tpTriggerOK && followOK && spellAdmissionOK && moveRangeOK && targetValidityOK && playerEngageOK && playerWeaponSkillOK && abilityRecastOK && playerActionGateOK && playerAbilityGateOK && trustFollowOK && trustTickOK && trustTargetSyncOK && trustEngageOK && trustRoamFormationOK && trustRecoveryOK && trustRangedAttackOK && trustCastCoordinationOK && trustRepositionOK && trustAbilityOK && trustNonCombatMovementOK && trustCombatMovementOK && playerCharmRoamOK && playerCharmCombatOK && playerCharmTickOK && petTickOK && petDeaggroOK && petHealingOK && petBuffTickOK && petMasterLossOK && petImmobileOK && petHealingRoamOK && hideOK && lockOK;
}
