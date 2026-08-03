#include "map/action/interrupts.h"

#include <iostream>

auto runActionInterrupt7983SelfTests() -> bool
{
    constexpr uint32 actorId = 0x10203040;
    const auto       action  = ActionInterrupts::detail::RangedParalyzedAction(actorId);

    const auto valid = action.actorId == actorId &&
                       action.actiontype == ActionCategory::MagicFinish &&
                       action.actionid == 0 &&
                       action.recast == 0s &&
                       action.spellgroup == SPELLGROUP_NONE &&
                       action.targets.size() == 1 &&
                       action.targets[0].actorId == actorId &&
                       action.targets[0].results.size() == 1;
    if (!valid)
    {
        std::cerr << "ranged paralyzed action interrupt 7983 self-test failed\n";
        return false;
    }

    const auto& result  = action.targets[0].results[0];
    const auto  matches = result.resolution == ActionResolution::Hit &&
                          result.animation == ActionAnimation::SkillInterrupt &&
                          result.param == 0 &&
                          result.messageID == MsgBasic::IsParalyzed2;
    if (!matches)
    {
        std::cerr << "ranged paralyzed action interrupt 7983 result fields failed\n";
    }
    return matches;
}
