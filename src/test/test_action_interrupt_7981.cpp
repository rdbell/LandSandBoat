#include "map/action/interrupts.h"

#include <iostream>

namespace
{

auto matches(const action_t& action, const uint32 actorId, const uint32 targetId, const MsgBasic messageID) -> bool
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

} // namespace

auto runActionInterrupt7981SelfTests() -> bool
{
    constexpr uint32 actorId     = 0x10203040;
    constexpr uint32 targetId    = 0x50607080;
    const auto       paralyzed   = ActionInterrupts::detail::AttackInterruptAction(actorId, targetId, MsgBasic::IsParalyzed2);
    const auto       intimidated = ActionInterrupts::detail::AttackInterruptAction(actorId, targetId, MsgBasic::IsIntimidated);

    if (!matches(paralyzed, actorId, targetId, MsgBasic::IsParalyzed2) ||
        !matches(intimidated, actorId, targetId, MsgBasic::IsIntimidated))
    {
        std::cerr << "attack action interrupt 7981 self-test failed\n";
        return false;
    }
    return true;
}
