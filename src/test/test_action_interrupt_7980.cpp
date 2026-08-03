#include "map/action/interrupts.h"

#include <iostream>

auto runActionInterrupt7980SelfTests() -> bool
{
    constexpr uint32 actorId  = 0x10203040;
    constexpr uint32 targetId = 0x50607080;
    const auto       action   = ActionInterrupts::detail::OutOfRangeAction(actorId, targetId);

    const auto valid = action.actorId == actorId &&
                       action.actiontype == ActionCategory::MagicFinish &&
                       action.actionid == 0 &&
                       action.recast == 0s &&
                       action.spellgroup == SPELLGROUP_NONE &&
                       action.targets.size() == 1 &&
                       action.targets[0].actorId == targetId &&
                       action.targets[0].results.size() == 1;
    if (!valid)
    {
        std::cerr << "out-of-range action interrupt 7980 self-test failed\n";
        return false;
    }

    const auto& result  = action.targets[0].results[0];
    const auto  matches = result.resolution == ActionResolution::Hit &&
                          result.animation == ActionAnimation::SkillInterrupt &&
                          result.param == 0 &&
                          result.messageID == MsgBasic::TooFarAway;
    if (!matches)
    {
        std::cerr << "out-of-range action interrupt 7980 result fields failed\n";
    }
    return matches;
}
