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
    if (!scentOK || !detectionOK || !readinessOK || !movementOK || !aggroOK || !tpTriggerOK || !followOK || !spellAdmissionOK || !moveRangeOK || !targetValidityOK || !playerEngageOK || !hideOK || !lockOK)
    {
        std::cerr << "mob controller deaggro 3946 self-test failed\n";
    }
    return scentOK && detectionOK && readinessOK && movementOK && aggroOK && tpTriggerOK && followOK && spellAdmissionOK && moveRangeOK && targetValidityOK && playerEngageOK && hideOK && lockOK;
}
