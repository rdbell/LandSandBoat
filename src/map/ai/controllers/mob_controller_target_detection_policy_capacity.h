#pragma once

#include "mob_controller_action_state_detection_capacity.h"
#include "mob_controller_ambush_detection_capacity.h"
#include "mob_controller_close_detection_range_capacity.h"
#include "mob_controller_hearing_detection_capacity.h"
#include "mob_controller_low_hp_detection_capacity.h"
#include "mob_controller_magic_detection_capacity.h"
#include "mob_controller_sight_detection_capacity.h"

#include <cstdint>

namespace mobcontrollertargetdetectionpolicy
{

// Input contains the resolved state used by the ordered target-detection policy.
struct Input
{
    bool         detectSight{};
    bool         ambushBehavior{};
    bool         detectHearing{};
    bool         detectMagic{};
    bool         detectLowHP{};
    bool         detectWeaponSkill{};
    bool         detectJobAbility{};
    bool         hasInvisible{};
    bool         hasSneak{};
    float        distance{};
    float        sightRange{};
    float        soundRange{};
    float        magicRange{};
    bool         facingTarget{};
    bool         targetInMeleeRange{};
    std::uint8_t hpPercent{};
};

// CanDetect evaluates the controller's target-detection branches in source order.
template <typename CanSeeTarget, typename TargetCastingMPCostSpell, typename TargetUsingWeaponSkill, typename TargetUsingJobAbility>
constexpr auto CanDetect(
    const Input&               input,
    CanSeeTarget&&             canSeeTarget,
    TargetCastingMPCostSpell&& targetCastingMPCostSpell,
    TargetUsingWeaponSkill&&   targetUsingWeaponSkill,
    TargetUsingJobAbility&&    targetUsingJobAbility) -> bool
{
    if (mobcontrollersightdetection::CanDetect(
            input.detectSight, input.hasInvisible, input.distance, input.sightRange, input.facingTarget, input.targetInMeleeRange, canSeeTarget))
    {
        return true;
    }

    if (mobcontrollerambushdetection::CanDetect(input.ambushBehavior, input.distance, input.hasSneak))
    {
        return true;
    }

    if (mobcontrollerhearingdetection::CanDetect(
            input.detectHearing, input.distance, input.soundRange, input.hasSneak, input.targetInMeleeRange, canSeeTarget))
    {
        return true;
    }

    if (mobcontrollermagicdetection::CanDetect(
            input.detectMagic, input.distance, input.magicRange, input.targetInMeleeRange, targetCastingMPCostSpell, canSeeTarget))
    {
        return true;
    }

    // Low-HP and action-state branches share the controller's 20-yalm cutoff.
    if (!mobcontrollerclosedetectionrange::IsInRange(input.distance))
    {
        return false;
    }

    if (mobcontrollerlowhpdetection::CanDetect(
            input.detectLowHP, input.hpPercent, input.targetInMeleeRange, canSeeTarget))
    {
        return true;
    }

    if (mobcontrolleractionstatedetection::CanDetect(
            input.detectWeaponSkill, input.targetInMeleeRange, targetUsingWeaponSkill, canSeeTarget))
    {
        return true;
    }

    return mobcontrolleractionstatedetection::CanDetect(
        input.detectJobAbility, input.targetInMeleeRange, targetUsingJobAbility, canSeeTarget);
}

} // namespace mobcontrollertargetdetectionpolicy
