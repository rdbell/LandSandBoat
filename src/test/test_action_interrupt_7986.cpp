#include "map/action/interrupts.h"

#include <iostream>

namespace
{

auto matchesInterrupt(const action_t& action, const uint32 actorId, const uint16 spellId, const FourCC interruptID) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::MagicStart ||
        action.actionid != static_cast<uint32_t>(interruptID) ||
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
           result.param == static_cast<int32_t>(spellId) &&
           result.messageID == MsgBasic::None;
}

auto matchesFinish(const action_t& action, const uint32 actorId, const uint32 targetId, const uint16 spellId, const MsgBasic messageID) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::MagicFinish ||
        action.actionid != static_cast<uint32_t>(spellId) ||
        action.recast != 2s ||
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

auto matchesStopCast(const action_t& action, const uint32 actorId, const uint16 spellId, const FourCC interruptID, const timer::duration recast) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::MagicStart ||
        action.actionid != static_cast<uint32_t>(interruptID) ||
        action.recast != recast ||
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
           result.param == static_cast<int32_t>(spellId) &&
           result.messageID == MsgBasic::None;
}

} // namespace

auto runActionInterrupt7986SelfTests() -> bool
{
    constexpr uint32 actorId  = 0x10203040;
    constexpr uint32 targetId = 0x50607080;
    constexpr uint16 spellId  = 0x1234;
    constexpr auto   fourCC   = FourCC::BlackMagicInterrupt;

    const auto interrupt         = ActionInterrupts::detail::MagicInterruptAction(actorId, spellId, fourCC);
    const auto finishParalyzed   = ActionInterrupts::detail::MagicStatusFinishAction(actorId, targetId, spellId, MsgBasic::IsParalyzed2);
    const auto stopParalyzed     = ActionInterrupts::detail::MagicStopCastAction(actorId, spellId, fourCC, 0s);
    const auto finishIntimidated = ActionInterrupts::detail::MagicStatusFinishAction(actorId, targetId, spellId, MsgBasic::IsIntimidated);
    const auto stopIntimidated   = ActionInterrupts::detail::MagicStopCastAction(actorId, spellId, fourCC, 2s);

    if (!matchesInterrupt(interrupt, actorId, spellId, fourCC) ||
        !matchesFinish(finishParalyzed, actorId, targetId, spellId, MsgBasic::IsParalyzed2) ||
        !matchesStopCast(stopParalyzed, actorId, spellId, fourCC, 0s) ||
        !matchesFinish(finishIntimidated, actorId, targetId, spellId, MsgBasic::IsIntimidated) ||
        !matchesStopCast(stopIntimidated, actorId, spellId, fourCC, 2s))
    {
        std::cerr << "magic action interrupt 7986 self-test failed\n";
        return false;
    }
    return true;
}
