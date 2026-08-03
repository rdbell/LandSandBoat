#include "map/action/interrupts.h"

#include <iostream>

namespace
{

auto matchesItemInterrupt(const action_t& action, const uint32 actorId) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::ItemStart ||
        action.actionid != static_cast<uint32_t>(FourCC::ItemInterrupt) ||
        action.recast != 0s ||
        action.spellgroup != SPELLGROUP_NONE ||
        action.targets.size() != 1 ||
        action.targets[0].actorId != actorId ||
        action.targets[0].results.size() != 1)
    {
        return false;
    }

    const auto& result = action.targets[0].results[0];
    return result.resolution == ActionResolution::Hit &&
           result.animation == ActionAnimation::None &&
           result.param == 0 &&
           result.messageID == MsgBasic::None;
}

auto matchesStatusFinish(const action_t& action, const uint32 actorId, const uint32 targetId, const MsgBasic messageID) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::MagicFinish ||
        action.actionid != 0 ||
        action.recast != 0s ||
        action.spellgroup != SPELLGROUP_NONE ||
        action.targets.size() != 1 ||
        action.targets[0].actorId != targetId ||
        action.targets[0].results.size() != 1)
    {
        return false;
    }

    const auto& result = action.targets[0].results[0];
    return result.resolution == ActionResolution::Hit &&
           result.animation == ActionAnimation::SkillInterrupt &&
           result.param == 0 &&
           result.messageID == messageID;
}

auto matchesStop(const action_t& action, const uint32 actorId) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::ItemStart ||
        action.actionid != static_cast<uint32_t>(FourCC::ItemInterrupt) ||
        action.recast != 0s ||
        action.spellgroup != SPELLGROUP_NONE ||
        action.targets.size() != 1 ||
        action.targets[0].actorId != actorId ||
        action.targets[0].results.size() != 1)
    {
        return false;
    }

    const auto& result = action.targets[0].results[0];
    return result.resolution == ActionResolution::Hit &&
           result.animation == ActionAnimation::SkillInterrupt &&
           result.param == 0 &&
           result.messageID == MsgBasic::None;
}

} // namespace

auto runActionInterrupt7987SelfTests() -> bool
{
    constexpr uint32 actorId  = 0x10203040;
    constexpr uint32 targetId = 0x50607080;

    const auto itemInterrupt     = ActionInterrupts::detail::ItemInterruptAction(actorId);
    const auto finishParalyzed   = ActionInterrupts::detail::ItemStatusFinishAction(actorId, targetId, MsgBasic::IsParalyzed2);
    const auto finishIntimidated = ActionInterrupts::detail::ItemStatusFinishAction(actorId, targetId, MsgBasic::IsIntimidated);
    const auto itemStop          = ActionInterrupts::detail::ItemStopAction(actorId);

    if (!matchesItemInterrupt(itemInterrupt, actorId) ||
        !matchesStatusFinish(finishParalyzed, actorId, targetId, MsgBasic::IsParalyzed2) ||
        !matchesStatusFinish(finishIntimidated, actorId, targetId, MsgBasic::IsIntimidated) ||
        !matchesStop(itemStop, actorId))
    {
        std::cerr << "item action interrupt 7987 self-test failed\n";
        return false;
    }
    return true;
}
