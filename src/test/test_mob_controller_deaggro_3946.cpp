#include "test_mob_controller_deaggro_3946.h"

#include "map/ai/controllers/mob_controller_deaggro_capacity.h"
#include "map/ai/controllers/mob_controller_detection_capacity.h"
#include "map/ai/controllers/mob_controller_readiness_capacity.h"
#include "map/ai/controllers/mob_controller_movement_capacity.h"
#include "map/ai/controllers/mob_controller_aggro_capacity.h"
#include "map/ai/helpers/gambits_tp_trigger_capacity.h"
#include "map/ai/controllers/mob_controller_follow_capacity.h"
#include "map/ai/controllers/mob_controller_spell_admission_capacity.h"
#include "map/ai/controllers/mob_controller_special_skill_target_capacity.h"
#include "map/ai/controllers/mob_controller_special_skill_admission_capacity.h"
#include "map/ai/controllers/mob_controller_spell_selection_capacity.h"
#include "map/ai/controllers/mob_controller_spell_target_range_capacity.h"
#include "map/ai/controllers/mob_controller_teleport_window_capacity.h"
#include "map/ai/controllers/mob_controller_type_two_teleport_capacity.h"
#include "map/ai/controllers/mob_controller_bound_target_candidate_capacity.h"
#include "map/ai/controllers/mob_controller_shared_target_selection_capacity.h"
#include "map/ai/controllers/mob_controller_roam_engage_gate_capacity.h"
#include "map/ai/controllers/mob_controller_roam_owner_engage_capacity.h"
#include "map/ai/controllers/mob_controller_roam_despawn_capacity.h"
#include "map/ai/controllers/mob_controller_roam_follow_leader_capacity.h"
#include "map/ai/controllers/mob_controller_roam_rest_gate_capacity.h"
#include "map/ai/controllers/mob_controller_roam_home_gate_capacity.h"
#include "map/ai/controllers/mob_controller_despawn_policy_capacity.h"
#include "map/ai/controllers/mob_controller_dead_master_despawn_capacity.h"
#include "map/ai/controllers/mob_controller_worm_roam_action_capacity.h"
#include "map/ai/controllers/mob_controller_roam_path_result_capacity.h"
#include "map/ai/controllers/mob_controller_roam_script_cadence_capacity.h"
#include "map/ai/controllers/mob_controller_wait_capacity.h"
#include "map/ai/controllers/mob_controller_ability_capacity.h"
#include "map/ai/controllers/mob_controller_mob_skill_target_capacity.h"
#include "map/ai/controllers/mob_controller_mob_skill_admission_capacity.h"
#include "map/ai/controllers/mob_controller_spell_cast_route_capacity.h"
#include "map/ai/controllers/mob_controller_spell_target_source_capacity.h"
#include "map/ai/controllers/mob_controller_owner_declaim_capacity.h"
#include "map/ai/controllers/mob_controller_run_away_capacity.h"
#include "map/ai/controllers/mob_controller_combat_action_gate_capacity.h"
#include "map/ai/controllers/mob_controller_ranged_attack_admission_capacity.h"
#include "map/ai/controllers/mob_controller_face_target_capacity.h"
#include "map/ai/controllers/mob_controller_scripted_path_capacity.h"
#include "map/ai/controllers/mob_controller_attack_range_source_capacity.h"
#include "map/ai/controllers/mob_controller_share_position_capacity.h"
#include "map/ai/controllers/mob_controller_type_one_teleport_admission_capacity.h"
#include "map/ai/controllers/mob_controller_movement_entry_capacity.h"
#include "map/ai/controllers/mob_controller_chase_movement_admission_capacity.h"
#include "map/ai/controllers/mob_controller_chase_path_refresh_capacity.h"
#include "map/ai/controllers/mob_controller_chase_path_start_capacity.h"
#include "map/ai/controllers/mob_controller_overlap_reposition_capacity.h"
#include "map/ai/controllers/mob_controller_overlap_reposition_point_capacity.h"
#include "map/ai/controllers/mob_controller_bound_retarget_admission_capacity.h"
#include "map/ai/controllers/mob_controller_bound_retarget_search_capacity.h"
#include "map/ai/controllers/mob_controller_roam_follow_ranges_capacity.h"
#include "map/ai/controllers/mob_controller_roam_home_action_capacity.h"
#include "map/ai/controllers/mob_controller_roam_action_dispatch_capacity.h"
#include "map/ai/controllers/mob_controller_roam_pet_follow_capacity.h"
#include "map/ai/controllers/mob_controller_engage_pet_capacity.h"
#include "map/ai/controllers/mob_controller_disengage_roam_schedule_capacity.h"
#include "map/ai/controllers/mob_controller_idle_despawn_capacity.h"
#include "map/ai/controllers/mob_controller_engage_delay_capacity.h"
#include "map/ai/controllers/mob_controller_fomor_aggro_context_capacity.h"
#include "map/ai/controllers/mob_controller_cast_stop_cooldown_capacity.h"
#include "map/ai/controllers/mob_controller_reset_capacity.h"
#include "map/ai/controllers/mob_controller_roam_reset_facing_capacity.h"
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
#include "map/ai/controllers/pet_controller_special_healing_roam_capacity.h"
#include "map/ai/controllers/pet_controller_state_change_roam_capacity.h"
#include "map/ai/controllers/pet_controller_ability_capacity.h"
#include "map/ai/controllers/pet_controller_pet_skill_capacity.h"
#include "map/ai/controllers/automaton_controller_stand_back_capacity.h"
#include "map/ai/controllers/automaton_controller_cooldown_capacity.h"
#include "map/ai/controllers/automaton_controller_maneuvers_capacity.h"
#include "map/ai/controllers/automaton_controller_master_loss_capacity.h"
#include "map/ai/controllers/automaton_controller_move_capacity.h"
#include "map/ai/controllers/automaton_controller_action_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_shield_bash_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_spell_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_healing_threshold_capacity.h"
#include "map/ai/controllers/automaton_controller_healing_target_capacity.h"
#include "map/ai/controllers/automaton_controller_cure_tier_capacity.h"
#include "map/ai/controllers/automaton_controller_elemental_tier_capacity.h"
#include "map/ai/controllers/automaton_controller_resistance_order_capacity.h"
#include "map/ai/controllers/automaton_controller_enfeeble_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_status_removal_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_enhance_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_ranged_attack_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skill_type_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skill_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skill_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skillchain_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_selection_fallback_capacity.h"
#include "map/ai/controllers/automaton_controller_spell_permission_capacity.h"
#include "map/ai/controllers/automaton_controller_cast_admission_capacity.h"
#include "map/ai/controllers/automaton_controller_mob_skill_admission_capacity.h"
#include "map/ai/controllers/automaton_controller_attachment_check_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_move_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_disengage_stand_back_capacity.h"
#include "map/ai/controllers/automaton_controller_healing_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_elemental_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_low_hp_elemental_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_enfeeble_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_stormwaker_ice_elemental_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_stormwaker_elemental_fallback_capacity.h"
#include "map/ai/controllers/automaton_controller_combat_party_heal_target_capacity.h"
#include "map/ai/controllers/automaton_controller_noncombat_party_heal_target_capacity.h"
#include "map/ai/controllers/automaton_controller_healing_hate_capacity.h"
#include "map/ai/controllers/automaton_controller_healing_master_distance_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_party_heal_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_party_status_removal_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_dispel_status_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_dia_bio_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_aspir_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_drain_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_absorb_int_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_dia_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_poison_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_silence_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_slow_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_paralyze_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_addle_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_slow_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_poison_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_blind_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_dia_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_bio_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_silence_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_paralyze_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_soulsoother_addle_priority_capacity.h"
#include "map/ai/controllers/automaton_controller_spiritreaver_enhancement_capacity.h"
#include "map/ai/controllers/automaton_controller_regen_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_enhancement_self_target_capacity.h"
#include "map/ai/controllers/automaton_controller_party_enhancement_threshold_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skill_selection_result_capacity.h"
#include "map/ai/controllers/automaton_controller_enfeeble_admission_capacity.h"
#include "map/ai/controllers/automaton_controller_spell_admission_capacity.h"
#include "map/ai/controllers/automaton_controller_erase_fallback_capacity.h"
#include "map/ai/controllers/automaton_controller_status_removal_candidate_capacity.h"
#include "map/ai/controllers/automaton_controller_master_enhancement_target_capacity.h"
#include "map/ai/controllers/automaton_controller_party_enhancement_target_capacity.h"
#include "map/ai/controllers/automaton_controller_enhancement_enmity_target_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skillchain_resonance_gate_capacity.h"
#include "map/ai/controllers/automaton_controller_tp_skillchain_resonance_properties_capacity.h"
#include "map/ai/controllers/automaton_controller_status_removal_decode_capacity.h"
#include "map/ai/controllers/pet_controller_follow_path_capacity.h"
#include "map/ai/controllers/pet_controller_follow_distance_capacity.h"
#include "map/ai/controllers/pet_controller_path_fallback_capacity.h"

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
    const bool followAdmissionOK = mobcontrollerfollow::CanFollow(true, true, true, true, true) &&
                                   !mobcontrollerfollow::CanFollow(false, true, true, true, true) &&
                                   !mobcontrollerfollow::CanFollow(true, false, true, true, true) &&
                                   !mobcontrollerfollow::CanFollow(true, true, false, true, true) &&
                                   !mobcontrollerfollow::CanFollow(true, true, true, false, true) &&
                                   !mobcontrollerfollow::CanFollow(true, true, true, true, false);
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
    const bool petSpecialHealingRoamOK = petcontrollerspecialhealingroam::ShouldHold(true, false, true) &&
                                         petcontrollerspecialhealingroam::ShouldHold(false, true, true) &&
                                         !petcontrollerspecialhealingroam::ShouldHold(true, false, false) &&
                                         !petcontrollerspecialhealingroam::ShouldHold(false, false, true);
    const bool petStateChangeRoamOK = petcontrollerstatechangeroam::ShouldHold(false) &&
                                      !petcontrollerstatechangeroam::ShouldHold(true);
    const bool petAbilityOK = petcontrollerability::ShouldDelegate(true) &&
                              !petcontrollerability::ShouldDelegate(false);
    const auto petSkillWithOwner = petcontrollerpetskill::Resolve(true);
    const auto petSkillWithoutOwner = petcontrollerpetskill::Resolve(false);
    const bool petSkillOK = petSkillWithOwner.faceTarget && petSkillWithOwner.emitBeforeUse && petSkillWithOwner.delegate &&
                            !petSkillWithoutOwner.faceTarget && !petSkillWithoutOwner.emitBeforeUse && !petSkillWithoutOwner.delegate;
    const bool automatonStandBackOK = automatoncontrollerstandback::ShouldStandBack(true, true, true, false) &&
                                      !automatoncontrollerstandback::ShouldStandBack(true, false, true, true) &&
                                      !automatoncontrollerstandback::ShouldStandBack(false, false, true, true) &&
                                      automatoncontrollerstandback::ShouldStandBack(false, false, false, true) &&
                                      !automatoncontrollerstandback::ShouldStandBack(false, false, false, false);
    const auto automatonHarlequinCooldown = automatoncontrollercooldown::Magic(0x01);
    const auto automatonValoredgeCooldown = automatoncontrollercooldown::Magic(0x02);
    const auto automatonSharpshotCooldown = automatoncontrollercooldown::Magic(0x03);
    const auto automatonStormwakerCooldown = automatoncontrollercooldown::Magic(0x04);
    const auto automatonSoulsootherCooldown = automatoncontrollercooldown::Magic(0x05);
    const auto automatonSpiritreaverCooldown = automatoncontrollercooldown::Magic(0x06);
    const bool automatonCooldownOK = automatonHarlequinCooldown.magic == 10 && automatonHarlequinCooldown.enfeeble == 12 && automatonHarlequinCooldown.heal == 12 &&
                                     automatonValoredgeCooldown.magic == 10 && automatonValoredgeCooldown.heal == 20 &&
                                     automatonSharpshotCooldown.magic == 10 && automatonSharpshotCooldown.enfeeble == 12 && automatonSharpshotCooldown.heal == 20 &&
                                     automatonStormwakerCooldown.magic == 8 && automatonStormwakerCooldown.enfeeble == 10 && automatonStormwakerCooldown.heal == 20 && automatonStormwakerCooldown.elemental == 25 && automatonStormwakerCooldown.enhance == 25 &&
                                     automatonSoulsootherCooldown.magic == 8 && automatonSoulsootherCooldown.enfeeble == 10 && automatonSoulsootherCooldown.heal == 10 && automatonSoulsootherCooldown.status == 10 && automatonSoulsootherCooldown.enhance == 25 &&
                                     automatonSpiritreaverCooldown.magic == 8 && automatonSpiritreaverCooldown.enfeeble == 10 && automatonSpiritreaverCooldown.elemental == 30 && automatonSpiritreaverCooldown.enhance == 35;
    const bool automatonFrameCooldownOK = automatoncontrollercooldown::Frame(0x22, 0x03).ranged == 20 &&
                                          automatoncontrollercooldown::Frame(0x22, 0x01).ranged == 25 &&
                                          automatoncontrollercooldown::Frame(0x22, 0x04).ranged == 36 &&
                                          automatoncontrollercooldown::Frame(0x21, 0x01).shieldBash == 180;
    const auto automatonManeuvers = automatoncontrollermaneuvers::Current({ 1, 2, 3, 4, 5, 6, 7, 8 });
    const bool automatonManeuversOK = automatonManeuvers.fire == 1 && automatonManeuvers.ice == 2 && automatonManeuvers.wind == 3 && automatonManeuvers.earth == 4 &&
                                      automatonManeuvers.thunder == 5 && automatonManeuvers.water == 6 && automatonManeuvers.light == 7 && automatonManeuvers.dark == 8;
    const bool automatonMasterLossOK = automatoncontrollermasterloss::ShouldDie(false, false, true) &&
                                       automatoncontrollermasterloss::ShouldDie(true, true, true) &&
                                       !automatoncontrollermasterloss::ShouldDie(true, false, true) &&
                                       !automatoncontrollermasterloss::ShouldDie(false, false, false);
    const bool automatonMoveOK = automatoncontrollermove::ShouldClearStandBack(true, false, 10, 10) &&
                                 !automatoncontrollermove::ShouldClearStandBack(true, true, 10, 10) &&
                                 automatoncontrollermove::ShouldClearStandBack(false, true, 7, 9) &&
                                 !automatoncontrollermove::ShouldClearStandBack(false, true, 8, 9) &&
                                 !automatoncontrollermove::ShouldClearStandBack(false, true, 7, 8);
    const bool automatonActionGateOK = automatoncontrolleractiongate::CanTakeAction(3001ms, 0ms, 3000ms, 0) &&
                                       !automatoncontrolleractiongate::CanTakeAction(3000ms, 0ms, 3000ms, 0) &&
                                       automatoncontrolleractiongate::CanTakeAction(2991ms, 0ms, 3000ms, 1) &&
                                       !automatoncontrolleractiongate::CanTakeAction(2990ms, 0ms, 3000ms, 1);
    const bool automatonShieldBashGateOK = automatoncontrollershieldbashgate::CanUseShieldBash(3001ms, 0ms, 3000ms, true, 0) &&
                                           !automatoncontrollershieldbashgate::CanUseShieldBash(3000ms, 0ms, 3000ms, true, 0) &&
                                           !automatoncontrollershieldbashgate::CanUseShieldBash(4000ms, 0ms, 0ms, true, 0) &&
                                           !automatoncontrollershieldbashgate::CanUseShieldBash(4000ms, 0ms, 3000ms, false, 0) &&
                                           automatoncontrollershieldbashgate::CanUseShieldBash(2001ms, 0ms, 3000ms, true, 1) &&
                                           !automatoncontrollershieldbashgate::CanUseShieldBash(2000ms, 0ms, 3000ms, true, 1);
    const bool automatonSpellGateOK = automatoncontrollerspellgate::CanCastSpell(3001ms, 0ms, 3000ms, true, true, 0) &&
                                      !automatoncontrollerspellgate::CanCastSpell(3000ms, 0ms, 3000ms, true, true, 0) &&
                                      !automatoncontrollerspellgate::CanCastSpell(4000ms, 0ms, 3000ms, false, true, 0) &&
                                      !automatoncontrollerspellgate::CanCastSpell(4000ms, 0ms, 0ms, true, true, 0) &&
                                      !automatoncontrollerspellgate::CanCastSpell(4000ms, 0ms, 3000ms, true, false, 0) &&
                                      automatoncontrollerspellgate::CanCastSpell(4001ms, 0ms, 3000ms, true, true, 1) &&
                                      !automatoncontrollerspellgate::CanCastSpell(4000ms, 0ms, 3000ms, true, true, 1);
    const bool automatonHealingThresholdOK = automatoncontrollerhealingthreshold::HealingThreshold(0, 0) == 30.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(1, 0) == 40.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(2, 0) == 50.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(3, 0) == 75.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(4, 0) == 30.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(1, 20) == 60.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(0, -1) == 30.0f &&
                                             automatoncontrollerhealingthreshold::HealingThreshold(3, 16) == 90.0f;
    const bool automatonHealingTargetOK = automatoncontrollerhealingtarget::Select(true, 50, 20, 30.0f, 0.0f) == automatoncontrollerhealingtarget::Target::Automaton &&
                                          automatoncontrollerhealingtarget::Select(true, 51, 30, 30.0f, 19.9f) == automatoncontrollerhealingtarget::Target::Master &&
                                          automatoncontrollerhealingtarget::Select(true, 51, 30, 30.0f, 20.0f) == automatoncontrollerhealingtarget::Target::None &&
                                          automatoncontrollerhealingtarget::Select(false, 20, 30, 30.0f, 0.0f) == automatoncontrollerhealingtarget::Target::Master &&
                                          automatoncontrollerhealingtarget::Select(false, 50, 31, 30.0f, 0.0f) == automatoncontrollerhealingtarget::Target::Automaton &&
                                          automatoncontrollerhealingtarget::Select(false, 51, 31, 30.0f, 0.0f) == automatoncontrollerhealingtarget::Target::None;
    const bool automatonCureTierOK = automatoncontrollercuretier::Select(0) == automatoncontrollercuretier::Tier::I &&
                                       automatoncontrollercuretier::Select(120) == automatoncontrollercuretier::Tier::I &&
                                       automatoncontrollercuretier::Select(121) == automatoncontrollercuretier::Tier::II &&
                                       automatoncontrollercuretier::Select(190) == automatoncontrollercuretier::Tier::II &&
                                       automatoncontrollercuretier::Select(191) == automatoncontrollercuretier::Tier::III &&
                                       automatoncontrollercuretier::Select(350) == automatoncontrollercuretier::Tier::III &&
                                       automatoncontrollercuretier::Select(351) == automatoncontrollercuretier::Tier::IV &&
                                       automatoncontrollercuretier::Select(600) == automatoncontrollercuretier::Tier::IV &&
                                       automatoncontrollercuretier::Select(601) == automatoncontrollercuretier::Tier::V &&
                                       automatoncontrollercuretier::Select(850) == automatoncontrollercuretier::Tier::V &&
                                       automatoncontrollercuretier::Select(851) == automatoncontrollercuretier::Tier::VI;
    const bool automatonElementalTierOK = automatoncontrollerelementaltier::Select(1000, 3) == -1 &&
                                          automatoncontrollerelementaltier::Select(50, 200) == 0 &&
                                          automatoncontrollerelementaltier::Select(1000, 15) == 0 &&
                                          automatoncontrollerelementaltier::Select(150, 200) == 1 &&
                                          automatoncontrollerelementaltier::Select(1000, 39) == 1 &&
                                          automatoncontrollerelementaltier::Select(200, 200) == 2 &&
                                          automatoncontrollerelementaltier::Select(1000, 87) == 2 &&
                                          automatoncontrollerelementaltier::Select(600, 200) == 3 &&
                                          automatoncontrollerelementaltier::Select(1000, 155) == 3 &&
                                          automatoncontrollerelementaltier::Select(601, 156) == 4;
    auto automatonResistances = std::vector<std::pair<int, int16>>{
        { 1, 2 },
        { 2, -1 },
        { 3, 2 },
        { 4, 0 },
    };
    automatoncontrollerresistanceorder::Sort(automatonResistances);
    const bool automatonResistanceOrderOK = automatonResistances[0].first == 2 &&
                                             automatonResistances[1].first == 4 &&
                                             automatonResistances[2].first == 1 &&
                                             automatonResistances[3].first == 3;
    const bool automatonEnfeebleGateOK = automatoncontrollerenfeeblegate::CanEnfeeble(3001ms, 0ms, 3000ms, true) &&
                                         !automatoncontrollerenfeeblegate::CanEnfeeble(3000ms, 0ms, 3000ms, true) &&
                                         !automatoncontrollerenfeeblegate::CanEnfeeble(4000ms, 0ms, 3000ms, false) &&
                                         !automatoncontrollerenfeeblegate::CanEnfeeble(4000ms, 0ms, 0ms, true);
    const bool automatonStatusRemovalGateOK = automatoncontrollerstatusremovalgate::CanRemoveStatus(3001ms, 0ms, 3000ms, true) &&
                                              !automatoncontrollerstatusremovalgate::CanRemoveStatus(3000ms, 0ms, 3000ms, true) &&
                                              !automatoncontrollerstatusremovalgate::CanRemoveStatus(4000ms, 0ms, 3000ms, false) &&
                                              !automatoncontrollerstatusremovalgate::CanRemoveStatus(4000ms, 0ms, 0ms, true);
    const bool automatonSoulsootherPartyStatusRemovalGateOK = automatoncontrollersoulsootherpartystatusremovalgate::CanConsiderPartyStatusRemoval(true, true, true) &&
                                                               !automatoncontrollersoulsootherpartystatusremovalgate::CanConsiderPartyStatusRemoval(false, true, true) &&
                                                               !automatoncontrollersoulsootherpartystatusremovalgate::CanConsiderPartyStatusRemoval(true, false, true) &&
                                                               !automatoncontrollersoulsootherpartystatusremovalgate::CanConsiderPartyStatusRemoval(true, true, false);
    const bool automatonSpiritreaverEnhancementOK = automatoncontrollerspiritreaverenhancement::CanCastDreadSpikes(true) &&
                                                    !automatoncontrollerspiritreaverenhancement::CanCastDreadSpikes(false);
    const bool automatonRegenCandidateOK = automatoncontrollerregencandidate::CanCastRegen(true, false, false) &&
                                           !automatoncontrollerregencandidate::CanCastRegen(false, false, false) &&
                                           !automatoncontrollerregencandidate::CanCastRegen(true, true, false) &&
                                           !automatoncontrollerregencandidate::CanCastRegen(true, false, true);
    const bool automatonEnhancementSelfTargetOK = automatoncontrollerenhancementselftarget::CanSelectSelf(false, false) &&
                                                  !automatoncontrollerenhancementselftarget::CanSelectSelf(true, false) &&
                                                  !automatoncontrollerenhancementselftarget::CanSelectSelf(false, true);
    const bool automatonEnhancementPartyTargetOK = automatoncontrollerenhancementselftarget::CanSelectTarget(false, false) &&
                                                   !automatoncontrollerenhancementselftarget::CanSelectTarget(true, false) &&
                                                   !automatoncontrollerenhancementselftarget::CanSelectTarget(false, true);
    const bool automatonPartyEnhancementThresholdOK = automatoncontrollerpartyenhancementthreshold::CanCastPartyEnhancement(4, 0) &&
                                                       automatoncontrollerpartyenhancementthreshold::CanCastPartyEnhancement(5, 1) &&
                                                       !automatoncontrollerpartyenhancementthreshold::CanCastPartyEnhancement(4, 1) &&
                                                       automatoncontrollerpartyenhancementthreshold::CanCastPartyEnhancement(0, 1);
    const bool automatonTPSkillSelectionResultOK = !automatoncontrollertpskillselectionresult::HasSelectedTPSkill(-1) &&
                                                   automatoncontrollertpskillselectionresult::HasSelectedTPSkill(0) &&
                                                   automatoncontrollertpskillselectionresult::HasSelectedTPSkill(3);
    const auto skillchainStart = timer::time_point{} + std::chrono::seconds(10);
    const bool automatonTPSkillchainResonanceGateOK = automatoncontrollertpskillchainresonancegate::CanUseResonance(
                                                         true, skillchainStart, skillchainStart + std::chrono::seconds(4)) &&
                                                     !automatoncontrollertpskillchainresonancegate::CanUseResonance(
                                                         false, skillchainStart, skillchainStart + std::chrono::seconds(4)) &&
                                                     !automatoncontrollertpskillchainresonancegate::CanUseResonance(
                                                         true, skillchainStart, skillchainStart + std::chrono::seconds(3)) &&
                                                     !automatoncontrollertpskillchainresonancegate::CanUseResonance(
                                                         true, skillchainStart, skillchainStart + std::chrono::seconds(2));
    const auto automatonResonanceProperties = automatoncontrollertpskillchainresonanceproperties::Decode(0xABCD);
    const bool automatonTPSkillchainResonancePropertiesOK = automatonResonanceProperties[0] == 13 &&
                                                            automatonResonanceProperties[1] == 12 &&
                                                            automatonResonanceProperties[2] == 0xAB;
    const auto automatonStatusRemovals = automatoncontrollerstatusremovaldecode::Decode(0x030001);
    const bool automatonStatusRemovalDecodeOK = automatonStatusRemovals == std::vector<std::uint16_t>{ 1, 0, 3 } &&
                                                automatoncontrollerstatusremovaldecode::Decode(0).empty();
    const bool mobSpecialSkillTargetOK = mobcontrollerspecialskilltarget::CanSelectTarget(true, false, false) &&
                                         mobcontrollerspecialskilltarget::CanSelectTarget(false, true, true) &&
                                         !mobcontrollerspecialskilltarget::CanSelectTarget(false, false, false) &&
                                         !mobcontrollerspecialskilltarget::CanSelectTarget(false, true, false);
    const bool mobSpecialSkillAdmissionOK = mobcontrollerspecialskilladmission::CanAttempt(true, true, false, false) &&
                                            !mobcontrollerspecialskilladmission::CanAttempt(false, true, false, false) &&
                                            !mobcontrollerspecialskilladmission::CanAttempt(true, false, false, false) &&
                                            !mobcontrollerspecialskilladmission::CanAttempt(true, true, true, false) &&
                                            mobcontrollerspecialskilladmission::CanAttempt(true, true, true, true);
    const bool mobSpellSelectionOK = mobcontrollerspellselection::Select(false, true, false) == mobcontrollerspellselection::Source::Buff &&
                                     mobcontrollerspellselection::Select(false, false, false) == mobcontrollerspellselection::Source::Random &&
                                     mobcontrollerspellselection::Select(true, false, true) == mobcontrollerspellselection::Source::Aggro &&
                                     mobcontrollerspellselection::Select(true, true, false) == mobcontrollerspellselection::Source::Random;
    const bool mobSpellTargetRangeOK = mobcontrollerspelltargetrange::IsInRange(10.0f, 10.0f, 0.0f, 0.0f) &&
                                       mobcontrollerspelltargetrange::IsInRange(12.0f, 10.0f, 0.75f, 1.25f) &&
                                       !mobcontrollerspelltargetrange::IsInRange(12.1f, 10.0f, 0.75f, 1.25f);
    const bool mobTeleportWindowOK = mobcontrollerteleportwindow::CanStartTypeOne(true, true, true) &&
                                     !mobcontrollerteleportwindow::CanStartTypeOne(false, true, true) &&
                                     !mobcontrollerteleportwindow::CanStartTypeOne(true, false, true) &&
                                     !mobcontrollerteleportwindow::CanStartTypeOne(true, true, false);
    const bool mobTypeTwoTeleportOK = mobcontrollertypetwoteleport::CanStart(true, true) &&
                                      !mobcontrollertypetwoteleport::CanStart(false, true) &&
                                      !mobcontrollertypetwoteleport::CanStart(true, false);
    const bool mobBoundTargetCandidateOK = mobcontrollerboundtargetcandidate::ShouldSelect(10, 11, 5.0f, 4.0f, true) &&
                                           !mobcontrollerboundtargetcandidate::ShouldSelect(10, 10, 5.0f, 4.0f, true) &&
                                           !mobcontrollerboundtargetcandidate::ShouldSelect(10, 11, 5.0f, 5.0f, true) &&
                                           !mobcontrollerboundtargetcandidate::ShouldSelect(10, 11, 5.0f, 4.0f, false);
    const bool mobSharedTargetSelectionOK = mobcontrollersharedtargetselection::Select(true, true, true) == mobcontrollersharedtargetselection::Source::Partner &&
                                            mobcontrollersharedtargetselection::Select(true, false, true) == mobcontrollersharedtargetselection::Source::Enmity &&
                                            mobcontrollersharedtargetselection::Select(false, false, true) == mobcontrollersharedtargetselection::Source::Enmity &&
                                            mobcontrollersharedtargetselection::Select(false, false, false) == mobcontrollersharedtargetselection::Source::None;
    const bool mobRoamEngageGateOK = mobcontrollerroamengagegate::ShouldEngageFromEnmity(true, false) &&
                                     !mobcontrollerroamengagegate::ShouldEngageFromEnmity(true, true) &&
                                     !mobcontrollerroamengagegate::ShouldEngageFromEnmity(false, false);
    const bool mobRoamOwnerEngageOK = mobcontrollerroamownerengage::ShouldAttempt(true, false) &&
                                      !mobcontrollerroamownerengage::ShouldAttempt(false, false) &&
                                      !mobcontrollerroamownerengage::ShouldAttempt(true, true);
    const bool mobRoamDespawnOK = mobcontrollerroamdespawn::ShouldDespawn(true, true) &&
                                  !mobcontrollerroamdespawn::ShouldDespawn(false, true) &&
                                  !mobcontrollerroamdespawn::ShouldDespawn(true, false);
    const bool mobRoamFollowLeaderOK = mobcontrollerroamfollowleader::ShouldPath(true, true) &&
                                        !mobcontrollerroamfollowleader::ShouldPath(false, true) &&
                                        !mobcontrollerroamfollowleader::ShouldPath(true, false);
    const auto mobRoamFollowDefaultRanges = mobcontrollerroamfollowranges::Resolve(0, 0);
    const auto mobRoamFollowLeashRanges = mobcontrollerroamfollowranges::Resolve(7, 0);
    const auto mobRoamFollowStopRanges = mobcontrollerroamfollowranges::Resolve(-1, 3);
    const bool mobRoamFollowRangesOK = mobRoamFollowDefaultRanges.leash == 4.0f && mobRoamFollowDefaultRanges.stop == 2.0f &&
                                       mobRoamFollowLeashRanges.leash == 7.0f && mobRoamFollowLeashRanges.stop == 2.0f &&
                                       mobRoamFollowStopRanges.leash == 4.0f && mobRoamFollowStopRanges.stop == 3.0f;
    const bool mobRoamHomeActionOK = mobcontrollerroamhomeaction::Resolve(false, true, true) == mobcontrollerroamhomeaction::Action::None &&
                                     mobcontrollerroamhomeaction::Resolve(true, true, true) == mobcontrollerroamhomeaction::Action::PathHome &&
                                     mobcontrollerroamhomeaction::Resolve(true, false, true) == mobcontrollerroamhomeaction::Action::Despawn &&
                                     mobcontrollerroamhomeaction::Resolve(true, false, false) == mobcontrollerroamhomeaction::Action::None;
    const bool mobRoamActionDispatchOK = mobcontrollerroamactiondispatch::Resolve(true, true, true) == mobcontrollerroamactiondispatch::Action::SpecialSkill &&
                                         mobcontrollerroamactiondispatch::Resolve(false, true, true) == mobcontrollerroamactiondispatch::Action::Scripted &&
                                         mobcontrollerroamactiondispatch::Resolve(false, false, true) == mobcontrollerroamactiondispatch::Action::Roam &&
                                         mobcontrollerroamactiondispatch::Resolve(false, false, false) == mobcontrollerroamactiondispatch::Action::None;
    const bool mobRoamPetFollowOK = mobcontrollerroampetfollow::ShouldFollow(true, true, false) &&
                                    !mobcontrollerroampetfollow::ShouldFollow(false, true, false) &&
                                    !mobcontrollerroampetfollow::ShouldFollow(true, false, false) &&
                                    !mobcontrollerroampetfollow::ShouldFollow(true, true, true);
    const bool mobEngagePetOK = mobcontrollerengagepet::ShouldEngage(true, true, false) &&
                                !mobcontrollerengagepet::ShouldEngage(false, true, false) &&
                                !mobcontrollerengagepet::ShouldEngage(true, false, false) &&
                                !mobcontrollerengagepet::ShouldEngage(true, true, true);
    const auto disengageRoamTick = base + std::chrono::seconds(20);
    const bool mobDisengageRoamScheduleOK =
        mobcontrollerdisengageroamschedule::Schedule(disengageRoamTick, std::chrono::seconds(30)) == base &&
        mobcontrollerdisengageroamschedule::Schedule(disengageRoamTick, std::chrono::seconds(10)) == disengageRoamTick &&
        mobcontrollerdisengageroamschedule::Schedule(disengageRoamTick, std::chrono::seconds(5)) == base + std::chrono::seconds(25);
    const auto noIdleDespawn = mobcontrolleridledespawn::Resolve(0);
    const auto idleDespawn = mobcontrolleridledespawn::Resolve(60);
    const bool mobIdleDespawnOK = !noIdleDespawn.shouldSet && noIdleDespawn.duration == std::chrono::seconds(0) &&
                                  idleDespawn.shouldSet && idleDespawn.duration == std::chrono::seconds(60);
    const auto engageDelayTick = base + std::chrono::seconds(20);
    const bool mobEngageDelayOK =
        mobcontrollerengagedelay::ScheduleMagic(engageDelayTick, std::chrono::seconds(0), std::chrono::seconds(0)) == engageDelayTick &&
        mobcontrollerengagedelay::ScheduleMagic(engageDelayTick, std::chrono::seconds(3), std::chrono::seconds(2)) == base + std::chrono::seconds(25) &&
        mobcontrollerengagedelay::ScheduleSpecial(engageDelayTick, std::chrono::seconds(0), std::chrono::seconds(0)) == engageDelayTick &&
        mobcontrollerengagedelay::ScheduleSpecial(engageDelayTick, std::chrono::seconds(3), std::chrono::seconds(2)) == base + std::chrono::seconds(15);
    const bool mobFollowEngageClearOK = mobcontrollerfollow::ShouldClearOnEngage(true, true) &&
                                        !mobcontrollerfollow::ShouldClearOnEngage(false, true) &&
                                        !mobcontrollerfollow::ShouldClearOnEngage(true, false);
    const auto fomorAggroContext = mobcontrollerfomoraggrocontext::Resolve(true, false, true, true);
    const bool mobFomorAggroContextOK = fomorAggroContext.usesTargetHate &&
                                        fomorAggroContext.fallbackHate == mobcontrollerfomoraggrocontext::DefaultHate &&
                                        !mobcontrollerfomoraggrocontext::Resolve(false, false, true, true).usesTargetHate &&
                                        !mobcontrollerfomoraggrocontext::Resolve(true, true, true, true).usesTargetHate &&
                                        !mobcontrollerfomoraggrocontext::Resolve(true, false, false, true).usesTargetHate &&
                                        !mobcontrollerfomoraggrocontext::Resolve(true, false, true, false).usesTargetHate;
    const auto evenCastStopCooldown = mobcontrollercaststopcooldown::Resolve(60);
    const auto oddCastStopCooldown = mobcontrollercaststopcooldown::Resolve(5);
    const auto zeroCastStopCooldown = mobcontrollercaststopcooldown::Resolve(0);
    const bool mobCastStopCooldownOK = evenCastStopCooldown.lower == 30 && evenCastStopCooldown.upper == 60 &&
                                       oddCastStopCooldown.lower == 2 && oddCastStopCooldown.upper == 5 &&
                                       zeroCastStopCooldown.lower == 0 && zeroCastStopCooldown.upper == 0;
    const auto resetTick = base + std::chrono::seconds(30);
    const auto delayedReset = mobcontrollerreset::Resolve(resetTick, std::chrono::seconds(7));
    const auto immediateReset = mobcontrollerreset::Resolve(resetTick, std::chrono::seconds(0));
    const bool mobResetOK = delayedReset.lastAction == base + std::chrono::seconds(23) && delayedReset.neutral &&
                            delayedReset.neutralTime == resetTick && delayedReset.clearTarget && delayedReset.clearFollowTarget &&
                            immediateReset.lastAction == resetTick;
    const bool mobRoamResetFacingOK = !mobcontrollerroamresetfacing::ShouldReset(false, 1.0f, 2.0f) &&
                                      mobcontrollerroamresetfacing::ShouldReset(true, 1.0f, 2.0f) &&
                                      mobcontrollerroamresetfacing::ShouldReset(true, 2.0f, 2.0f) &&
                                      !mobcontrollerroamresetfacing::ShouldReset(true, 2.1f, 2.0f);
    const bool mobRoamRestGateOK = mobcontrollerroamrestgate::CanRest(true, false, true) &&
                                   !mobcontrollerroamrestgate::CanRest(false, false, true) &&
                                   !mobcontrollerroamrestgate::CanRest(true, true, true) &&
                                   !mobcontrollerroamrestgate::CanRest(true, false, false);
    const bool mobRoamHomeGateOK = mobcontrollerroamhomegate::ShouldPathHome(false, true, true) &&
                                   !mobcontrollerroamhomegate::ShouldPathHome(true, true, true) &&
                                   !mobcontrollerroamhomegate::ShouldPathHome(false, false, true) &&
                                   !mobcontrollerroamhomegate::ShouldPathHome(false, true, false);
    const bool mobDespawnPolicyOK = mobcontrollerdespawnpolicy::CanDespawn(false, false) &&
                                     !mobcontrollerdespawnpolicy::CanDespawn(true, false) &&
                                     !mobcontrollerdespawnpolicy::CanDespawn(false, true);
    const bool mobDeadMasterDespawnOK = mobcontrollerdeadmasterdespawn::ShouldDespawn(true, true, false) &&
                                        !mobcontrollerdeadmasterdespawn::ShouldDespawn(false, true, false) &&
                                        !mobcontrollerdeadmasterdespawn::ShouldDespawn(true, false, false) &&
                                        !mobcontrollerdeadmasterdespawn::ShouldDespawn(true, true, true);
    const bool mobWormRoamActionOK = mobcontrollerwormroamaction::Select(true, false, false) == mobcontrollerwormroamaction::Action::Burrow &&
                                     mobcontrollerwormroamaction::Select(true, false, true) == mobcontrollerwormroamaction::Action::Wait &&
                                     mobcontrollerwormroamaction::Select(true, true, false) == mobcontrollerwormroamaction::Action::RoamAround &&
                                     mobcontrollerwormroamaction::Select(false, false, false) == mobcontrollerwormroamaction::Action::RoamAround;
    const bool mobRoamPathResultOK = mobcontrollerroampathresult::Resolve(false, false) == mobcontrollerroampathresult::Result::RecordAction &&
                                     mobcontrollerroampathresult::Resolve(false, true) == mobcontrollerroampathresult::Result::RecordAction &&
                                     mobcontrollerroampathresult::Resolve(true, false) == mobcontrollerroampathresult::Result::Follow &&
                                     mobcontrollerroampathresult::Resolve(true, true) == mobcontrollerroampathresult::Result::Conceal;
    const auto roamScriptLast = base + std::chrono::seconds(10);
    const bool mobRoamScriptCadenceOK = !mobcontrollerroamscriptcadence::ShouldRun(roamScriptLast + std::chrono::seconds(3) - std::chrono::nanoseconds(1), roamScriptLast) &&
                                        mobcontrollerroamscriptcadence::ShouldRun(roamScriptLast + std::chrono::seconds(3), roamScriptLast) &&
                                        mobcontrollerroamscriptcadence::ShouldRun(roamScriptLast + std::chrono::seconds(3) + std::chrono::nanoseconds(1), roamScriptLast);
    const auto elapsedWait = mobcontrollerwait::Apply(base + std::chrono::seconds(11), base + std::chrono::seconds(10), std::chrono::seconds(3));
    const auto equalWait = mobcontrollerwait::Apply(base + std::chrono::seconds(10), base + std::chrono::seconds(10), std::chrono::seconds(3));
    const auto activeWait = mobcontrollerwait::Apply(base + std::chrono::seconds(9), base + std::chrono::seconds(10), std::chrono::seconds(3));
    const bool mobWaitOK = elapsedWait.tick == base + std::chrono::seconds(14) &&
                           elapsedWait.waitUntil == base + std::chrono::seconds(14) &&
                           equalWait.tick == base + std::chrono::seconds(10) &&
                           equalWait.waitUntil == base + std::chrono::seconds(13) &&
                           activeWait.tick == base + std::chrono::seconds(9) &&
                           activeWait.waitUntil == base + std::chrono::seconds(13);
    const bool mobAbilityOK = mobcontrollerability::CanUse(false, true) &&
                              !mobcontrollerability::CanUse(true, true) &&
                              !mobcontrollerability::CanUse(false, false) &&
                              !mobcontrollerability::CanUse(true, false);
    if (!mobWaitOK)
    {
        std::cerr << "mob wait-state self-test failed\n";
        return false;
    }
    if (!mobAbilityOK)
    {
        std::cerr << "mob ability admission self-test failed\n";
        return false;
    }
    const bool mobSkillTargetOK = mobcontrollermobskilltarget::Select(true, false) == mobcontrollermobskilltarget::Target::Enemy &&
                                  mobcontrollermobskilltarget::Select(false, true) == mobcontrollermobskilltarget::Target::Self &&
                                  mobcontrollermobskilltarget::Select(false, false) == mobcontrollermobskilltarget::Target::None &&
                                  mobcontrollermobskilltarget::Select(true, true) == mobcontrollermobskilltarget::Target::Enemy;
    if (!mobSkillTargetOK)
    {
        std::cerr << "mob skill target self-test failed\n";
        return false;
    }
    const bool mobSkillAdmissionOK = mobcontrollermobskilladmission::CanDispatch(true, false, true, true) &&
                                     !mobcontrollermobskilladmission::CanDispatch(false, false, true, true) &&
                                     !mobcontrollermobskilladmission::CanDispatch(true, true, true, true) &&
                                     !mobcontrollermobskilladmission::CanDispatch(true, false, false, true) &&
                                     !mobcontrollermobskilladmission::CanDispatch(true, false, true, false);
    if (!mobSkillAdmissionOK)
    {
        std::cerr << "mob skill admission self-test failed\n";
        return false;
    }
    const bool mobSpellCastRouteOK = mobcontrollerspellcastroute::Select(false, false) == mobcontrollerspellcastroute::Route::FindTarget &&
                                     mobcontrollerspellcastroute::Select(false, true) == mobcontrollerspellcastroute::Route::FindTarget &&
                                     mobcontrollerspellcastroute::Select(true, false) == mobcontrollerspellcastroute::Route::FindTarget &&
                                     mobcontrollerspellcastroute::Select(true, true) == mobcontrollerspellcastroute::Route::Direct;
    if (!mobSpellCastRouteOK)
    {
        std::cerr << "mob spell cast route self-test failed\n";
        return false;
    }
    const bool mobSpellTargetSourceOK = mobcontrollerspelltargetsource::Select(false, false, false, false, false) == mobcontrollerspelltargetsource::Source::Enemy &&
                                        mobcontrollerspelltargetsource::Select(true, false, false, false, false) == mobcontrollerspelltargetsource::Source::Self &&
                                        mobcontrollerspelltargetsource::Select(true, true, true, true, false) == mobcontrollerspelltargetsource::Source::Master &&
                                        mobcontrollerspelltargetsource::Select(true, true, true, false, true) == mobcontrollerspelltargetsource::Source::PartyCandidate &&
                                        mobcontrollerspelltargetsource::Select(true, true, true, false, false) == mobcontrollerspelltargetsource::Source::Self &&
                                        mobcontrollerspelltargetsource::Select(true, true, false, true, true) == mobcontrollerspelltargetsource::Source::PartyCandidate;
    if (!mobSpellTargetSourceOK)
    {
        std::cerr << "mob spell target source self-test failed\n";
        return false;
    }
    const bool mobOwnerDeclaimOK = mobcontrollerownerdeclaim::ShouldClear(true, false, true) &&
                                   !mobcontrollerownerdeclaim::ShouldClear(false, false, true) &&
                                   !mobcontrollerownerdeclaim::ShouldClear(true, true, true) &&
                                   !mobcontrollerownerdeclaim::ShouldClear(true, false, false);
    if (!mobOwnerDeclaimOK)
    {
        std::cerr << "mob owner declaim self-test failed\n";
        return false;
    }
    const auto noRunAway = mobcontrollerrunaway::Resolve(false, false, false);
    const auto startRunAway = mobcontrollerrunaway::Resolve(true, true, false);
    const auto followRunAway = mobcontrollerrunaway::Resolve(true, true, true);
    const auto arriveRunAway = mobcontrollerrunaway::Resolve(true, false, false);
    const bool mobRunAwayOK = !noRunAway.handled &&
                              startRunAway.handled && startRunAway.startPath && startRunAway.followPath &&
                              followRunAway.handled && !followRunAway.startPath && followRunAway.followPath &&
                              arriveRunAway.handled && arriveRunAway.notifyArrival && arriveRunAway.clearTarget;
    if (!mobRunAwayOK)
    {
        std::cerr << "mob run-away self-test failed\n";
        return false;
    }
    const bool mobCombatActionGateOK = mobcontrollercombatactiongate::CanAct(false, true) &&
                                       !mobcontrollercombatactiongate::CanAct(true, true) &&
                                       !mobcontrollercombatactiongate::CanAct(false, false) &&
                                       !mobcontrollercombatactiongate::CanAct(true, false);
    if (!mobCombatActionGateOK)
    {
        std::cerr << "mob combat action gate self-test failed\n";
        return false;
    }
    const bool mobRangedAttackAdmissionOK = mobcontrollerrangedattackadmission::CanAttempt(true, true, true, true) &&
                                            !mobcontrollerrangedattackadmission::CanAttempt(false, true, true, true) &&
                                            !mobcontrollerrangedattackadmission::CanAttempt(true, false, true, true) &&
                                            !mobcontrollerrangedattackadmission::CanAttempt(true, true, false, true) &&
                                            !mobcontrollerrangedattackadmission::CanAttempt(true, true, true, false);
    if (!mobRangedAttackAdmissionOK)
    {
        std::cerr << "mob ranged attack admission self-test failed\n";
        return false;
    }
    const bool mobFaceTargetOK = mobcontrollerfacetarget::ResolveTargetID(42, 7) == 42 &&
                                 mobcontrollerfacetarget::ResolveTargetID(0, 7) == 7 &&
                                 mobcontrollerfacetarget::ShouldLookAt(false, true) &&
                                 !mobcontrollerfacetarget::ShouldLookAt(true, true) &&
                                 !mobcontrollerfacetarget::ShouldLookAt(false, false);
    if (!mobFaceTargetOK)
    {
        std::cerr << "mob face-target self-test failed\n";
        return false;
    }
    const bool mobScriptedPathOK = mobcontrollerscriptedpath::ShouldFollow(true, true) &&
                                   !mobcontrollerscriptedpath::ShouldFollow(false, true) &&
                                   !mobcontrollerscriptedpath::ShouldFollow(true, false) &&
                                   !mobcontrollerscriptedpath::ShouldFollow(false, false);
    if (!mobScriptedPathOK)
    {
        std::cerr << "mob scripted-path self-test failed\n";
        return false;
    }
    const bool mobAttackRangeSourceOK = mobcontrollerattackrangesource::Resolve(5, false, 9, false, 20) == 5 &&
                                        mobcontrollerattackrangesource::Resolve(5, true, 9, false, 20) == 9 &&
                                        mobcontrollerattackrangesource::Resolve(5, false, 9, true, 20) == 20 &&
                                        mobcontrollerattackrangesource::Resolve(5, true, 9, true, 20) == 20;
    if (!mobAttackRangeSourceOK)
    {
        std::cerr << "mob attack-range source self-test failed\n";
        return false;
    }
    const bool mobSharePositionOK = mobcontrollershareposition::Resolve(false, false) == mobcontrollershareposition::Action::None &&
                                   mobcontrollershareposition::Resolve(true, true) == mobcontrollershareposition::Action::Copy &&
                                   mobcontrollershareposition::Resolve(true, false) == mobcontrollershareposition::Action::Warn;
    if (!mobSharePositionOK)
    {
        std::cerr << "mob share-position self-test failed\n";
        return false;
    }
    const bool mobTypeOneTeleportAdmissionOK = mobcontrollertypeoneteleportadmission::CanDispatch(true, true) &&
                                               !mobcontrollertypeoneteleportadmission::CanDispatch(false, true) &&
                                               !mobcontrollertypeoneteleportadmission::CanDispatch(true, false) &&
                                               !mobcontrollertypeoneteleportadmission::CanDispatch(false, false);
    if (!mobTypeOneTeleportAdmissionOK)
    {
        std::cerr << "mob type-one teleport admission self-test failed\n";
        return false;
    }
    const bool mobMovementEntryOK = mobcontrollermovemententry::ShouldEnter(true, false, true) &&
                                    mobcontrollermovemententry::ShouldEnter(false, true, true) &&
                                    !mobcontrollermovemententry::ShouldEnter(false, false, true) &&
                                    !mobcontrollermovemententry::ShouldEnter(true, false, false);
    if (!mobMovementEntryOK)
    {
        std::cerr << "mob movement-entry self-test failed\n";
        return false;
    }
    const bool mobChaseMovementAdmissionOK = mobcontrollerchasemovementadmission::CanEnter(true, true, true) &&
                                             !mobcontrollerchasemovementadmission::CanEnter(false, true, true) &&
                                             !mobcontrollerchasemovementadmission::CanEnter(true, false, true) &&
                                             !mobcontrollerchasemovementadmission::CanEnter(true, true, false);
    if (!mobChaseMovementAdmissionOK)
    {
        std::cerr << "mob chase-movement admission self-test failed\n";
        return false;
    }
    const bool mobChasePathRefreshOK = mobcontrollerchasepathrefresh::ShouldRefresh(false) &&
                                       !mobcontrollerchasepathrefresh::ShouldRefresh(true);
    if (!mobChasePathRefreshOK)
    {
        std::cerr << "mob chase-path refresh self-test failed\n";
        return false;
    }
    const bool mobChasePathStartOK = mobcontrollerchasepathstart::ShouldStart(false, true) &&
                                     !mobcontrollerchasepathstart::ShouldStart(true, true) &&
                                     !mobcontrollerchasepathstart::ShouldStart(false, false);
    if (!mobChasePathStartOK)
    {
        std::cerr << "mob chase-path start self-test failed\n";
        return false;
    }
    const bool mobOverlapRepositionOK = mobcontrolleroverlapreposition::ShouldReposition(false, false, true) &&
                                        !mobcontrolleroverlapreposition::ShouldReposition(true, false, true) &&
                                        !mobcontrolleroverlapreposition::ShouldReposition(false, true, true) &&
                                        !mobcontrolleroverlapreposition::ShouldReposition(false, false, false);
    if (!mobOverlapRepositionOK)
    {
        std::cerr << "mob overlap-reposition self-test failed\n";
        return false;
    }
    const auto mobOverlapPoint = mobcontrolleroverlaprepositionpoint::Destination(
        position_t{ 0.0f, 8.0f, 0.0f, 9, 42 }, position_t{ 10.0f, 3.0f, 0.0f, 8, 7 });
    const auto mobOverlapNearPoint = mobcontrolleroverlaprepositionpoint::Destination(
        position_t{ 0.0f, 8.0f, 0.0f, 9, 64 }, position_t{ 0.0f, 3.0f, 0.0f, 8, 7 });
    const bool mobOverlapRepositionPointOK = std::abs(mobOverlapPoint.x) < 0.001f &&
                                              std::abs(mobOverlapPoint.y - 3.0f) < 0.001f &&
                                              std::abs(mobOverlapPoint.z - 1.5f) < 0.001f &&
                                              mobOverlapPoint.moving == 0 && mobOverlapPoint.rotation == 0 &&
                                              std::abs(mobOverlapNearPoint.x - 1.5f) < 0.001f &&
                                              std::abs(mobOverlapNearPoint.y - 3.0f) < 0.001f &&
                                              std::abs(mobOverlapNearPoint.z) < 0.001f &&
                                              mobOverlapNearPoint.moving == 0 && mobOverlapNearPoint.rotation == 0;
    if (!mobOverlapRepositionPointOK)
    {
        std::cerr << "mob overlap-reposition point self-test failed\n";
        return false;
    }
    const bool mobBoundRetargetAdmissionOK = mobcontrollerboundretargetadmission::ShouldAttempt(true, true, true) &&
                                             !mobcontrollerboundretargetadmission::ShouldAttempt(false, true, true) &&
                                             !mobcontrollerboundretargetadmission::ShouldAttempt(true, false, true) &&
                                             !mobcontrollerboundretargetadmission::ShouldAttempt(true, true, false);
    if (!mobBoundRetargetAdmissionOK)
    {
        std::cerr << "mob bound-retarget admission self-test failed\n";
        return false;
    }
    const bool mobBoundRetargetSearchOK = mobcontrollerboundretargetsearch::ShouldSearch(true, false, true) &&
                                          !mobcontrollerboundretargetsearch::ShouldSearch(false, false, true) &&
                                          !mobcontrollerboundretargetsearch::ShouldSearch(true, true, true) &&
                                          !mobcontrollerboundretargetsearch::ShouldSearch(true, false, false);
    if (!mobBoundRetargetSearchOK)
    {
        std::cerr << "mob bound-retarget search self-test failed\n";
        return false;
    }
    const bool automatonEnfeebleAdmissionOK = automatoncontrollerenfeebleadmission::CanUseEnfeeble(false, false) &&
                                              !automatoncontrollerenfeebleadmission::CanUseEnfeeble(true, false) &&
                                              !automatoncontrollerenfeebleadmission::CanUseEnfeeble(false, true);
    const bool automatonSpellAdmissionOK = automatoncontrollerspelladmission::CanUseSpell(100, 100, 1, 0x01) &&
                                           automatoncontrollerspelladmission::CanUseSpell(101, 100, 6, 0x20) &&
                                           !automatoncontrollerspelladmission::CanUseSpell(99, 100, 1, 0x01) &&
                                           !automatoncontrollerspelladmission::CanUseSpell(100, 100, 2, 0x01);
    const bool automatonEraseFallbackOK = automatoncontrollererasefallback::ShouldSelectErase(true) &&
                                          !automatoncontrollererasefallback::ShouldSelectErase(false);
    const auto automatonStatusRemovalCandidates = std::vector<uint16>{ 1, 3, 5 };
    const bool automatonStatusRemovalCandidateOK = automatoncontrollerstatusremovalcandidate::CanMatchStatusRemoval(automatonStatusRemovalCandidates, uint16{ 3 }) &&
                                                   !automatoncontrollerstatusremovalcandidate::CanMatchStatusRemoval(automatonStatusRemovalCandidates, uint16{ 4 }) &&
                                                   !automatoncontrollerstatusremovalcandidate::CanMatchStatusRemoval(std::vector<uint16>{}, uint16{ 1 });
    const bool automatonMasterEnhancementTargetOK = !automatoncontrollermasterenhancementtarget::CanConsiderMasterEnhancement(true, false, false) &&
                                                    automatoncontrollermasterenhancementtarget::CanConsiderMasterEnhancement(true, true, true) &&
                                                    !automatoncontrollermasterenhancementtarget::CanConsiderMasterEnhancement(false, true, true) &&
                                                    !automatoncontrollermasterenhancementtarget::CanConsiderMasterEnhancement(true, true, false);
    const bool automatonPartyEnhancementTargetOK = automatoncontrollerpartyenhancementtarget::CanConsiderPartyEnhancement(true, true, false, false) &&
                                                   automatoncontrollerpartyenhancementtarget::CanConsiderPartyEnhancement(true, true, true, true) &&
                                                   !automatoncontrollerpartyenhancementtarget::CanConsiderPartyEnhancement(false, true, false, false) &&
                                                   !automatoncontrollerpartyenhancementtarget::CanConsiderPartyEnhancement(true, false, false, false) &&
                                                   !automatoncontrollerpartyenhancementtarget::CanConsiderPartyEnhancement(true, true, true, false);
    const bool automatonEnhancementEnmityTargetOK = automatoncontrollerenhancementenmitytarget::CanSelectHigherEnmity(100, 101) &&
                                                    !automatoncontrollerenhancementenmitytarget::CanSelectHigherEnmity(100, 100) &&
                                                    !automatoncontrollerenhancementenmitytarget::CanSelectHigherEnmity(100, 99);
    const bool automatonEnhanceGateOK = automatoncontrollerenhancegate::CanEnhance(3001ms, 0ms, 3000ms, true) &&
                                       !automatoncontrollerenhancegate::CanEnhance(3000ms, 0ms, 3000ms, true) &&
                                       !automatoncontrollerenhancegate::CanEnhance(4000ms, 0ms, 3000ms, false) &&
                                       !automatoncontrollerenhancegate::CanEnhance(4000ms, 0ms, 0ms, true);
    const bool automatonRangedAttackGateOK = !automatoncontrollerrangedattackgate::CanRangedAttack(11000ms, 0ms, 10000ms, false, false, 0) &&
                                             automatoncontrollerrangedattackgate::CanRangedAttack(5001ms, 0ms, 10000ms, true, true, 5) &&
                                             automatoncontrollerrangedattackgate::CanRangedAttack(10001ms, 0ms, 10000ms, true, false, 5) &&
                                             !automatoncontrollerrangedattackgate::CanRangedAttack(5000ms, 0ms, 10000ms, true, true, 5) &&
                                             automatoncontrollerrangedattackgate::CanRangedAttack(8001ms, 0ms, 10000ms, true, true, 2) &&
                                             !automatoncontrollerrangedattackgate::CanRangedAttack(11000ms, 0ms, 0ms, true, true, 0);
    const bool automatonTPSkillTypeOK = !automatoncontrollertpskilltype::UsesRangedWeaponSkill(false) &&
                                        automatoncontrollertpskilltype::UsesRangedWeaponSkill(true);
    const bool automatonTPSkillCandidateOK = automatoncontrollertpskillcandidate::CanUseTPSkillCandidate(true, 201, 200, 4.9f, 5.0f) &&
                                             !automatoncontrollertpskillcandidate::CanUseTPSkillCandidate(false, 201, 200, 4.9f, 5.0f) &&
                                             !automatoncontrollertpskillcandidate::CanUseTPSkillCandidate(true, 200, 200, 4.9f, 5.0f) &&
                                             !automatoncontrollertpskillcandidate::CanUseTPSkillCandidate(true, 201, -1, 4.9f, 5.0f) &&
                                             !automatoncontrollertpskillcandidate::CanUseTPSkillCandidate(true, 201, 200, 5.0f, 5.0f);
    const bool automatonTPSkillPriorityOK = automatoncontrollertpskillpriority::ShouldSelectTPSkill(0, -1, 100, -1) &&
                                            automatoncontrollertpskillpriority::ShouldSelectTPSkill(2, 1, 100, 200) &&
                                            automatoncontrollertpskillpriority::ShouldSelectTPSkill(1, 1, 201, 200) &&
                                            !automatoncontrollertpskillpriority::ShouldSelectTPSkill(-1, -1, 100, -1) &&
                                            !automatoncontrollertpskillpriority::ShouldSelectTPSkill(1, 2, 300, 200) &&
                                            !automatoncontrollertpskillpriority::ShouldSelectTPSkill(1, 1, 200, 200);
    const bool automatonTPSkillchainCandidateOK = automatoncontrollertpskillchaincandidate::ShouldSelectTPSkillchainCandidate(201, 200, true) &&
                                                  automatoncontrollertpskillchaincandidate::ShouldSelectTPSkillchainCandidate(0, -1, true) &&
                                                  !automatoncontrollertpskillchaincandidate::ShouldSelectTPSkillchainCandidate(201, 200, false) &&
                                                  !automatoncontrollertpskillchaincandidate::ShouldSelectTPSkillchainCandidate(200, 200, true) &&
                                                  !automatoncontrollertpskillchaincandidate::ShouldSelectTPSkillchainCandidate(199, 200, true);
    const bool automatonTPSkillSelectionFallbackOK = automatoncontrollertpselectionfallback::ShouldUseNormalTPSkillSelection(false, -1, false, 0, 0) &&
                                                     automatoncontrollertpselectionfallback::ShouldUseNormalTPSkillSelection(true, -1, true, 999, 1000) &&
                                                     !automatoncontrollertpselectionfallback::ShouldUseNormalTPSkillSelection(true, 1, true, 999, 1000) &&
                                                     !automatoncontrollertpselectionfallback::ShouldUseNormalTPSkillSelection(true, -1, false, 0, 1000) &&
                                                     !automatoncontrollertpselectionfallback::ShouldUseNormalTPSkillSelection(true, -1, true, 1000, 1000);
    const bool automatonSpellPermissionOK = automatoncontrollerspellpermission::CanCastSpells(false, false, true, false, true) &&
                                             !automatoncontrollerspellpermission::CanCastSpells(true, false, true, false, true) &&
                                             !automatoncontrollerspellpermission::CanCastSpells(false, true, true, false, true) &&
                                             !automatoncontrollerspellpermission::CanCastSpells(false, false, false, false, true) &&
                                             automatoncontrollerspellpermission::CanCastSpells(false, false, false, true, true) &&
                                             !automatoncontrollerspellpermission::CanCastSpells(false, false, true, true, false);
    const bool automatonCastAdmissionOK = automatoncontrollercastadmission::CanUseCast(true, false) &&
                                          !automatoncontrollercastadmission::CanUseCast(false, false) &&
                                          !automatoncontrollercastadmission::CanUseCast(true, true);
    const bool automatonMobSkillAdmissionOK = automatoncontrollermobskilladmission::CanUseMobSkill(false) &&
                                              !automatoncontrollermobskilladmission::CanUseMobSkill(true);
    if (!automatonMobSkillAdmissionOK)
    {
        std::cerr << "automaton mob-skill admission self-test failed\n";
        return false;
    }
    const bool automatonAttachmentCheckGateOK = automatoncontrollerattachmentcheckgate::CanCheckAttachments(true) &&
                                                !automatoncontrollerattachmentcheckgate::CanCheckAttachments(false);
    if (!automatonAttachmentCheckGateOK)
    {
        std::cerr << "automaton attachment-check gate self-test failed\n";
        return false;
    }
    const bool automatonTPMoveGateOK = !automatoncontrollertpmovegate::CanTryTPMove(999) &&
                                       automatoncontrollertpmovegate::CanTryTPMove(1000);
    if (!automatonTPMoveGateOK)
    {
        std::cerr << "automaton TP-move gate self-test failed\n";
        return false;
    }
    const bool automatonDisengageStandBackOK = automatoncontrollerdisengagestandback::ShouldRestoreStandBackOnDisengage(true) &&
                                               !automatoncontrollerdisengagestandback::ShouldRestoreStandBackOnDisengage(false);
    if (!automatonDisengageStandBackOK)
    {
        std::cerr << "automaton disengage stand-back self-test failed\n";
        return false;
    }
    const bool automatonHealingGateOK = automatoncontrollerhealinggate::CanTryHeal(true, 7001ms, 1000ms, 6000ms, 0ms) &&
                                        !automatoncontrollerhealinggate::CanTryHeal(false, 7001ms, 1000ms, 6000ms, 0ms) &&
                                        !automatoncontrollerhealinggate::CanTryHeal(true, 7001ms, 1000ms, 0ms, 0ms) &&
                                        !automatoncontrollerhealinggate::CanTryHeal(true, 7000ms, 1000ms, 6000ms, 0ms) &&
                                        automatoncontrollerhealinggate::CanTryHeal(true, 6001ms, 1000ms, 6000ms, 1000ms);
    if (!automatonHealingGateOK)
    {
        std::cerr << "automaton healing gate self-test failed\n";
        return false;
    }
    const bool automatonElementalGateOK = automatoncontrollerelementalgate::CanTryElemental(true, 7001ms, 1000ms, 6000ms) &&
                                          !automatoncontrollerelementalgate::CanTryElemental(false, 7001ms, 1000ms, 6000ms) &&
                                          !automatoncontrollerelementalgate::CanTryElemental(true, 7001ms, 1000ms, 0ms) &&
                                          !automatoncontrollerelementalgate::CanTryElemental(true, 7000ms, 1000ms, 6000ms);
    if (!automatonElementalGateOK)
    {
        std::cerr << "automaton elemental gate self-test failed\n";
        return false;
    }
    const bool automatonLowHPElemPriorityOK = automatoncontrollerlowhpelementalpriority::ShouldPrioritizeElementalForLowHP(30, 300) &&
                                              !automatoncontrollerlowhpelementalpriority::ShouldPrioritizeElementalForLowHP(31, 300) &&
                                              !automatoncontrollerlowhpelementalpriority::ShouldPrioritizeElementalForLowHP(30, 301);
    if (!automatonLowHPElemPriorityOK)
    {
        std::cerr << "automaton low-HP elemental priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverEnfeeblePriorityOK = automatoncontrollerspiritreaverenfeeblepriority::ShouldPrioritizeEnfeeble(true, 100, 100) &&
                                                         automatoncontrollerspiritreaverenfeeblepriority::ShouldPrioritizeEnfeeble(false, 74, 100) &&
                                                         automatoncontrollerspiritreaverenfeeblepriority::ShouldPrioritizeEnfeeble(false, 100, 74) &&
                                                         !automatoncontrollerspiritreaverenfeeblepriority::ShouldPrioritizeEnfeeble(false, 75, 75);
    if (!automatonSpiritreaverEnfeeblePriorityOK)
    {
        std::cerr << "automaton Spiritreaver enfeeble priority self-test failed\n";
        return false;
    }
    const bool automatonStormwakerIceElemPriorityOK = automatoncontrollerstormwakericeelementalpriority::ShouldPrioritizeElemental(true, false) &&
                                                      !automatoncontrollerstormwakericeelementalpriority::ShouldPrioritizeElemental(true, true) &&
                                                      !automatoncontrollerstormwakericeelementalpriority::ShouldPrioritizeElemental(false, false);
    if (!automatonStormwakerIceElemPriorityOK)
    {
        std::cerr << "automaton Stormwaker ice elemental priority self-test failed\n";
        return false;
    }
    const bool automatonStormwakerElemFallbackOK = automatoncontrollerstormwakerelementalfallback::ShouldUseElementalFallback(false, false) &&
                                                   !automatoncontrollerstormwakerelementalfallback::ShouldUseElementalFallback(true, false) &&
                                                   !automatoncontrollerstormwakerelementalfallback::ShouldUseElementalFallback(false, true);
    if (!automatonStormwakerElemFallbackOK)
    {
        std::cerr << "automaton Stormwaker elemental fallback self-test failed\n";
        return false;
    }
    const bool automatonCombatPartyHealTargetOK = automatoncontrollercombatpartyhealtarget::CanSelectTarget(false, true, 101, 100, 50, 50, 19.9f) &&
                                                  !automatoncontrollercombatpartyhealtarget::CanSelectTarget(true, true, 101, 0, 50, 50, 19.9f) &&
                                                  !automatoncontrollercombatpartyhealtarget::CanSelectTarget(false, false, 101, 0, 50, 50, 19.9f) &&
                                                  !automatoncontrollercombatpartyhealtarget::CanSelectTarget(false, true, 100, 100, 50, 50, 19.9f) &&
                                                  !automatoncontrollercombatpartyhealtarget::CanSelectTarget(false, true, 101, 0, 51, 50, 19.9f) &&
                                                  !automatoncontrollercombatpartyhealtarget::CanSelectTarget(false, true, 101, 0, 50, 50, 20.0f);
    if (!automatonCombatPartyHealTargetOK)
    {
        std::cerr << "automaton combat party heal-target self-test failed\n";
        return false;
    }
    const bool automatonNonCombatPartyHealTargetOK = automatoncontrollernoncombatpartyhealtarget::CanSelectTarget(false, 50, 50, 19.9f) &&
                                                     !automatoncontrollernoncombatpartyhealtarget::CanSelectTarget(true, 50, 50, 19.9f) &&
                                                     !automatoncontrollernoncombatpartyhealtarget::CanSelectTarget(false, 51, 50, 19.9f) &&
                                                     !automatoncontrollernoncombatpartyhealtarget::CanSelectTarget(false, 50, 50, 20.0f);
    if (!automatonNonCombatPartyHealTargetOK)
    {
        std::cerr << "automaton non-combat party heal-target self-test failed\n";
        return false;
    }
    const bool automatonHealingHateOK = !automatoncontrollerhealinghate::HasHealingHate(false, false, false, 0, 0) &&
                                        automatoncontrollerhealinghate::HasHealingHate(true, false, true, 0, 0) &&
                                        !automatoncontrollerhealinghate::HasHealingHate(true, true, false, 0, 0) &&
                                        automatoncontrollerhealinghate::HasHealingHate(true, true, true, 100, 101) &&
                                        !automatoncontrollerhealinghate::HasHealingHate(true, true, true, 100, 100);
    if (!automatonHealingHateOK)
    {
        std::cerr << "automaton healing-hate self-test failed\n";
        return false;
    }
    const bool automatonHealingMasterDistanceOK = automatoncontrollerhealingmasterdistance::ShouldMeasureDistance(true, 51, 50, 50.0f) &&
                                                  !automatoncontrollerhealingmasterdistance::ShouldMeasureDistance(false, 51, 50, 50.0f) &&
                                                  !automatoncontrollerhealingmasterdistance::ShouldMeasureDistance(true, 50, 50, 50.0f) &&
                                                  !automatoncontrollerhealingmasterdistance::ShouldMeasureDistance(true, 51, 51, 50.0f);
    if (!automatonHealingMasterDistanceOK)
    {
        std::cerr << "automaton healing master-distance self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherPartyHealGateOK = automatoncontrollersoulsootherpartyhealgate::CanConsiderPartyHealing(true, true, true, true) &&
                                                     !automatoncontrollersoulsootherpartyhealgate::CanConsiderPartyHealing(false, true, true, true) &&
                                                     !automatoncontrollersoulsootherpartyhealgate::CanConsiderPartyHealing(true, false, true, true) &&
                                                     !automatoncontrollersoulsootherpartyhealgate::CanConsiderPartyHealing(true, true, false, true) &&
                                                     !automatoncontrollersoulsootherpartyhealgate::CanConsiderPartyHealing(true, true, true, false);
    if (!automatonSoulsootherPartyHealGateOK)
    {
        std::cerr << "automaton Soulsoother party-heal gate self-test failed\n";
        return false;
    }
    const bool automatonDispelStatusCandidateOK = automatoncontrollerdispelstatuscandidate::CanSelectCandidate(false, true, true) &&
                                                  !automatoncontrollerdispelstatuscandidate::CanSelectCandidate(true, true, true) &&
                                                  !automatoncontrollerdispelstatuscandidate::CanSelectCandidate(false, false, true) &&
                                                  !automatoncontrollerdispelstatuscandidate::CanSelectCandidate(false, true, false);
    if (!automatonDispelStatusCandidateOK)
    {
        std::cerr << "automaton Dispel status-candidate self-test failed\n";
        return false;
    }
    const bool automatonDiaBioPriorityOK = automatoncontrollerdiabiopriority::CanPrioritize(true, true) &&
                                           !automatoncontrollerdiabiopriority::CanPrioritize(true, false) &&
                                           !automatoncontrollerdiabiopriority::CanPrioritize(false, true);
    if (!automatonDiaBioPriorityOK)
    {
        std::cerr << "automaton Dia/Bio priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverAspirCandidateOK = automatoncontrollerspiritreaveraspircandidate::CanSelectCandidate(74, 1) &&
                                                       !automatoncontrollerspiritreaveraspircandidate::CanSelectCandidate(75, 1) &&
                                                       !automatoncontrollerspiritreaveraspircandidate::CanSelectCandidate(74, 0);
    if (!automatonSpiritreaverAspirCandidateOK)
    {
        std::cerr << "automaton Spiritreaver Aspir candidate self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverDrainCandidateOK = automatoncontrollerspiritreaverdraincandidate::CanSelectCandidate(74, false) &&
                                                       !automatoncontrollerspiritreaverdraincandidate::CanSelectCandidate(75, false) &&
                                                       !automatoncontrollerspiritreaverdraincandidate::CanSelectCandidate(74, true);
    if (!automatonSpiritreaverDrainCandidateOK)
    {
        std::cerr << "automaton Spiritreaver Drain candidate self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverAbsorbIntCandidateOK = automatoncontrollerspiritreaverabsorbintcandidate::CanSelectCandidate(true, false) &&
                                                           !automatoncontrollerspiritreaverabsorbintcandidate::CanSelectCandidate(true, true) &&
                                                           !automatoncontrollerspiritreaverabsorbintcandidate::CanSelectCandidate(false, false);
    if (!automatonSpiritreaverAbsorbIntCandidateOK)
    {
        std::cerr << "automaton Spiritreaver Absorb-INT candidate self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverDiaPriorityOK = automatoncontrollerspiritreaverdia::CanPrioritize(true, 2) &&
                                                    !automatoncontrollerspiritreaverdia::CanPrioritize(true, 1) &&
                                                    !automatoncontrollerspiritreaverdia::CanPrioritize(false, 2);
    if (!automatonSpiritreaverDiaPriorityOK)
    {
        std::cerr << "automaton Spiritreaver Dia priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverPoisonPriorityOK = automatoncontrollerspiritreaverpoison::CanPrioritize(2) &&
                                                       !automatoncontrollerspiritreaverpoison::CanPrioritize(1) &&
                                                       !automatoncontrollerspiritreaverpoison::CanPrioritize(0);
    if (!automatonSpiritreaverPoisonPriorityOK)
    {
        std::cerr << "automaton Spiritreaver Poison priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverSilencePriorityOK = automatoncontrollerspiritreaversilence::CanPrioritize(2) &&
                                                        !automatoncontrollerspiritreaversilence::CanPrioritize(1) &&
                                                        !automatoncontrollerspiritreaversilence::CanPrioritize(0);
    if (!automatonSpiritreaverSilencePriorityOK)
    {
        std::cerr << "automaton Spiritreaver Silence priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverSlowPriorityOK = automatoncontrollerspiritreaverslow::CanPrioritize(2) &&
                                                     !automatoncontrollerspiritreaverslow::CanPrioritize(1) &&
                                                     !automatoncontrollerspiritreaverslow::CanPrioritize(0);
    if (!automatonSpiritreaverSlowPriorityOK)
    {
        std::cerr << "automaton Spiritreaver Slow priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverParalyzePriorityOK = automatoncontrollerspiritreaverparalyze::CanPrioritize(2) &&
                                                         !automatoncontrollerspiritreaverparalyze::CanPrioritize(1) &&
                                                         !automatoncontrollerspiritreaverparalyze::CanPrioritize(0);
    if (!automatonSpiritreaverParalyzePriorityOK)
    {
        std::cerr << "automaton Spiritreaver Paralyze priority self-test failed\n";
        return false;
    }
    const bool automatonSpiritreaverAddlePriorityOK = automatoncontrollerspiritreaveraddle::CanPrioritize(2) &&
                                                      !automatoncontrollerspiritreaveraddle::CanPrioritize(1) &&
                                                      !automatoncontrollerspiritreaveraddle::CanPrioritize(0);
    if (!automatonSpiritreaverAddlePriorityOK)
    {
        std::cerr << "automaton Spiritreaver Addle priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherSlowPriorityOK = automatoncontrollersoulsootherslow::CanPrioritize(1) &&
                                                    !automatoncontrollersoulsootherslow::CanPrioritize(0);
    if (!automatonSoulsootherSlowPriorityOK)
    {
        std::cerr << "automaton Soulsoother Slow priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherPoisonPriorityOK = automatoncontrollersoulsootherpoison::CanPrioritize(1) &&
                                                      !automatoncontrollersoulsootherpoison::CanPrioritize(0);
    if (!automatonSoulsootherPoisonPriorityOK)
    {
        std::cerr << "automaton Soulsoother Poison priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherBlindPriorityOK = automatoncontrollersoulsootherblind::CanPrioritize(1) &&
                                                     !automatoncontrollersoulsootherblind::CanPrioritize(0);
    if (!automatonSoulsootherBlindPriorityOK)
    {
        std::cerr << "automaton Soulsoother Blind priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherDiaPriorityOK = automatoncontrollersoulsootherdia::CanPrioritize(true, 1) &&
                                                   !automatoncontrollersoulsootherdia::CanPrioritize(true, 0) &&
                                                   !automatoncontrollersoulsootherdia::CanPrioritize(false, 1);
    if (!automatonSoulsootherDiaPriorityOK)
    {
        std::cerr << "automaton Soulsoother Dia priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherBioPriorityOK = automatoncontrollersoulsootherbio::CanPrioritize(true, 1) &&
                                                   !automatoncontrollersoulsootherbio::CanPrioritize(true, 0) &&
                                                   !automatoncontrollersoulsootherbio::CanPrioritize(false, 1);
    if (!automatonSoulsootherBioPriorityOK)
    {
        std::cerr << "automaton Soulsoother Bio priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherSilencePriorityOK = automatoncontrollersoulsoothersilence::CanPrioritize(1) &&
                                                       !automatoncontrollersoulsoothersilence::CanPrioritize(0);
    if (!automatonSoulsootherSilencePriorityOK)
    {
        std::cerr << "automaton Soulsoother Silence priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherParalyzePriorityOK = automatoncontrollersoulsootherparalyze::CanPrioritize(1) &&
                                                        !automatoncontrollersoulsootherparalyze::CanPrioritize(0);
    if (!automatonSoulsootherParalyzePriorityOK)
    {
        std::cerr << "automaton Soulsoother Paralyze priority self-test failed\n";
        return false;
    }
    const bool automatonSoulsootherAddlePriorityOK = automatoncontrollersoulsootheraddle::CanPrioritize(1) &&
                                                     !automatoncontrollersoulsootheraddle::CanPrioritize(0);
    if (!automatonSoulsootherAddlePriorityOK)
    {
        std::cerr << "automaton Soulsoother Addle priority self-test failed\n";
        return false;
    }
    const bool petFollowPathOK = petcontrollerfollowpath::ShouldRecalculate(false, 0.0f) &&
                                 petcontrollerfollowpath::ShouldRecalculate(true, 2.01f) &&
                                 !petcontrollerfollowpath::ShouldRecalculate(true, 2.0f) &&
                                 !petcontrollerfollowpath::ShouldRecalculate(true, 1.99f);
    const bool petPathFallbackOK = petcontrollerpathfallback::ShouldWarp(false, false) &&
                                   !petcontrollerpathfallback::ShouldWarp(true, false) &&
                                   !petcontrollerpathfallback::ShouldWarp(false, true) &&
                                   !petcontrollerpathfallback::ShouldWarp(true, true);
    const bool petFollowDistanceOK = petcontrollerfollowdistance::ShouldHold(0.0f) &&
                                     petcontrollerfollowdistance::ShouldHold(2.09f) &&
                                     petcontrollerfollowdistance::ShouldHold(2.1f) &&
                                     !petcontrollerfollowdistance::ShouldHold(2.11f);

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
    if (!automatonRegenCandidateOK)
    {
        std::cerr << "automaton Regen candidate self-test failed\n";
        return false;
    }
    if (!automatonEnhancementSelfTargetOK)
    {
        std::cerr << "automaton enhancement self-target self-test failed\n";
        return false;
    }
    if (!automatonEnhancementPartyTargetOK)
    {
        std::cerr << "automaton enhancement party-target self-test failed\n";
        return false;
    }
    if (!automatonPartyEnhancementThresholdOK)
    {
        std::cerr << "automaton party enhancement threshold self-test failed\n";
        return false;
    }
    if (!automatonTPSkillSelectionResultOK)
    {
        std::cerr << "automaton TP skill selection result self-test failed\n";
        return false;
    }
    if (!automatonTPSkillchainResonanceGateOK)
    {
        std::cerr << "automaton TP skillchain resonance gate self-test failed\n";
        return false;
    }
    if (!automatonTPSkillchainResonancePropertiesOK)
    {
        std::cerr << "automaton TP skillchain resonance properties self-test failed\n";
        return false;
    }
    if (!automatonStatusRemovalDecodeOK)
    {
        std::cerr << "automaton status-removal decode self-test failed\n";
        return false;
    }
    if (!mobSpecialSkillTargetOK)
    {
        std::cerr << "mob special-skill target self-test failed\n";
        return false;
    }
    if (!mobSpecialSkillAdmissionOK)
    {
        std::cerr << "mob special-skill admission self-test failed\n";
        return false;
    }
    if (!mobSpellSelectionOK)
    {
        std::cerr << "mob spell selection self-test failed\n";
        return false;
    }
    if (!mobSpellTargetRangeOK)
    {
        std::cerr << "mob spell target range self-test failed\n";
        return false;
    }
    if (!mobTeleportWindowOK)
    {
        std::cerr << "mob teleport window self-test failed\n";
        return false;
    }
    if (!mobTypeTwoTeleportOK)
    {
        std::cerr << "mob type-two teleport self-test failed\n";
        return false;
    }
    if (!mobBoundTargetCandidateOK)
    {
        std::cerr << "mob bound-target candidate self-test failed\n";
        return false;
    }
    if (!mobSharedTargetSelectionOK)
    {
        std::cerr << "mob shared-target selection self-test failed\n";
        return false;
    }
    if (!mobRoamEngageGateOK)
    {
        std::cerr << "mob roam enmity gate self-test failed\n";
        return false;
    }
    if (!mobRoamOwnerEngageOK)
    {
        std::cerr << "mob roam owner-engage self-test failed\n";
        return false;
    }
    if (!mobRoamDespawnOK)
    {
        std::cerr << "mob roam despawn self-test failed\n";
        return false;
    }
    if (!mobRoamFollowLeaderOK)
    {
        std::cerr << "mob roam follow-leader self-test failed\n";
        return false;
    }
    if (!mobRoamFollowRangesOK)
    {
        std::cerr << "mob roam follow-ranges self-test failed\n";
        return false;
    }
    if (!mobRoamHomeActionOK)
    {
        std::cerr << "mob roam home-action self-test failed\n";
        return false;
    }
    if (!mobRoamActionDispatchOK)
    {
        std::cerr << "mob roam action-dispatch self-test failed\n";
        return false;
    }
    if (!mobRoamPetFollowOK)
    {
        std::cerr << "mob roam pet-follow self-test failed\n";
        return false;
    }
    if (!mobEngagePetOK)
    {
        std::cerr << "mob engage-pet self-test failed\n";
        return false;
    }
    if (!mobDisengageRoamScheduleOK)
    {
        std::cerr << "mob disengage-roam schedule self-test failed\n";
        return false;
    }
    if (!mobIdleDespawnOK)
    {
        std::cerr << "mob idle-despawn self-test failed\n";
        return false;
    }
    if (!mobEngageDelayOK)
    {
        std::cerr << "mob engage-delay self-test failed\n";
        return false;
    }
    if (!mobFollowEngageClearOK)
    {
        std::cerr << "mob follow-engage clear self-test failed\n";
        return false;
    }
    if (!mobFomorAggroContextOK)
    {
        std::cerr << "mob Fomor aggro-context self-test failed\n";
        return false;
    }
    if (!mobCastStopCooldownOK)
    {
        std::cerr << "mob cast-stop cooldown self-test failed\n";
        return false;
    }
    if (!mobResetOK)
    {
        std::cerr << "mob reset self-test failed\n";
        return false;
    }
    if (!mobRoamResetFacingOK)
    {
        std::cerr << "mob roam reset-facing self-test failed\n";
        return false;
    }
    if (!mobRoamRestGateOK)
    {
        std::cerr << "mob roam-rest gate self-test failed\n";
        return false;
    }
    if (!mobRoamHomeGateOK)
    {
        std::cerr << "mob roam-home gate self-test failed\n";
        return false;
    }
    if (!mobDespawnPolicyOK)
    {
        std::cerr << "mob despawn policy self-test failed\n";
        return false;
    }
    if (!mobDeadMasterDespawnOK)
    {
        std::cerr << "mob dead-master despawn self-test failed\n";
        return false;
    }
    if (!mobWormRoamActionOK)
    {
        std::cerr << "mob worm roam-action self-test failed\n";
        return false;
    }
    if (!mobRoamPathResultOK)
    {
        std::cerr << "mob roam-path result self-test failed\n";
        return false;
    }
    if (!mobRoamScriptCadenceOK)
    {
        std::cerr << "mob roam-script cadence self-test failed\n";
        return false;
    }
    if (!mobWaitOK)
    {
        std::cerr << "mob wait-state self-test failed\n";
        return false;
    }
    if (!automatonEnfeebleAdmissionOK)
    {
        std::cerr << "automaton enfeeble admission self-test failed\n";
        return false;
    }
    if (!automatonSpellAdmissionOK)
    {
        std::cerr << "automaton spell admission self-test failed\n";
        return false;
    }
    if (!automatonEraseFallbackOK)
    {
        std::cerr << "automaton Erase fallback self-test failed\n";
        return false;
    }
    if (!automatonStatusRemovalCandidateOK)
    {
        std::cerr << "automaton status-removal candidate self-test failed\n";
        return false;
    }
    if (!automatonMasterEnhancementTargetOK)
    {
        std::cerr << "automaton master enhancement target self-test failed\n";
        return false;
    }
    if (!automatonPartyEnhancementTargetOK)
    {
        std::cerr << "automaton party enhancement target self-test failed\n";
        return false;
    }
    if (!automatonEnhancementEnmityTargetOK)
    {
        std::cerr << "automaton enhancement enmity target self-test failed\n";
        return false;
    }
    if (!scentOK || !detectionOK || !readinessOK || !movementOK || !aggroOK || !tpTriggerOK || !followOK || !followAdmissionOK || !spellAdmissionOK || !moveRangeOK || !targetValidityOK || !playerEngageOK || !playerWeaponSkillOK || !abilityRecastOK || !playerActionGateOK || !playerAbilityGateOK || !trustFollowOK || !trustTickOK || !trustTargetSyncOK || !trustEngageOK || !trustRoamFormationOK || !trustRecoveryOK || !trustRangedAttackOK || !trustCastCoordinationOK || !trustRepositionOK || !trustAbilityOK || !trustNonCombatMovementOK || !trustCombatMovementOK || !playerCharmRoamOK || !playerCharmCombatOK || !playerCharmTickOK || !petTickOK || !petDeaggroOK || !petHealingOK || !petBuffTickOK || !petMasterLossOK || !petImmobileOK || !petHealingRoamOK || !petSpecialHealingRoamOK || !petStateChangeRoamOK || !petAbilityOK || !petSkillOK || !automatonStandBackOK || !automatonCooldownOK || !automatonFrameCooldownOK || !automatonManeuversOK || !automatonMasterLossOK || !automatonMoveOK || !automatonActionGateOK || !automatonShieldBashGateOK || !automatonSpellGateOK || !automatonHealingThresholdOK || !automatonHealingTargetOK || !automatonCureTierOK || !automatonElementalTierOK || !automatonResistanceOrderOK || !automatonEnfeebleGateOK || !automatonStatusRemovalGateOK || !automatonSoulsootherPartyStatusRemovalGateOK || !automatonSpiritreaverEnhancementOK || !automatonEnhanceGateOK || !automatonRangedAttackGateOK || !automatonTPSkillTypeOK || !automatonTPSkillCandidateOK || !automatonTPSkillPriorityOK || !automatonTPSkillchainCandidateOK || !automatonTPSkillSelectionFallbackOK || !automatonSpellPermissionOK || !automatonCastAdmissionOK || !petFollowPathOK || !petPathFallbackOK || !petFollowDistanceOK || !hideOK || !lockOK)
    {
        std::cerr << "mob controller deaggro 3946 self-test failed\n";
    }
    return scentOK && detectionOK && readinessOK && movementOK && aggroOK && tpTriggerOK && followOK && followAdmissionOK && spellAdmissionOK && moveRangeOK && targetValidityOK && playerEngageOK && playerWeaponSkillOK && abilityRecastOK && playerActionGateOK && playerAbilityGateOK && trustFollowOK && trustTickOK && trustTargetSyncOK && trustEngageOK && trustRoamFormationOK && trustRecoveryOK && trustRangedAttackOK && trustCastCoordinationOK && trustRepositionOK && trustAbilityOK && trustNonCombatMovementOK && trustCombatMovementOK && playerCharmRoamOK && playerCharmCombatOK && playerCharmTickOK && petTickOK && petDeaggroOK && petHealingOK && petBuffTickOK && petMasterLossOK && petImmobileOK && petHealingRoamOK && petSpecialHealingRoamOK && petStateChangeRoamOK && petAbilityOK && petSkillOK && automatonStandBackOK && automatonCooldownOK && automatonFrameCooldownOK && automatonManeuversOK && automatonMasterLossOK && automatonMoveOK && automatonActionGateOK && automatonShieldBashGateOK && automatonSpellGateOK && automatonHealingThresholdOK && automatonHealingTargetOK && automatonCureTierOK && automatonElementalTierOK && automatonResistanceOrderOK && automatonEnfeebleGateOK && automatonStatusRemovalGateOK && automatonSoulsootherPartyStatusRemovalGateOK && automatonSpiritreaverEnhancementOK && automatonEnhanceGateOK && automatonRangedAttackGateOK && automatonTPSkillTypeOK && automatonTPSkillCandidateOK && automatonTPSkillPriorityOK && automatonTPSkillchainCandidateOK && automatonTPSkillSelectionFallbackOK && automatonSpellPermissionOK && automatonCastAdmissionOK && petFollowPathOK && petPathFallbackOK && petFollowDistanceOK && hideOK && lockOK;
}
