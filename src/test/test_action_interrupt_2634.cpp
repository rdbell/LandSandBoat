#include "map/action/interrupts.h"

#include <iostream>

auto runActionInterrupt2634SelfTests() -> bool
{
    constexpr uint32 actorId = 0x10203040;
    const auto action = ActionInterrupts::detail::RangedInterruptAction(actorId);

    const auto valid = action.actorId == actorId &&
                       action.actiontype == ActionCategory::RangedStart &&
                       action.actionid == static_cast<uint32>(FourCC::RangedInterrupt) &&
                       action.recast == 0s &&
                       action.spellgroup == SPELLGROUP_NONE &&
                       action.targets.size() == 1 &&
                       action.targets[0].actorId == actorId &&
                       action.targets[0].results.size() == 1;
    if (!valid)
    {
        std::cerr << "ranged interrupt 2634 self-test failed\n";
        return false;
    }

    const auto& result = action.targets[0].results[0];
    const auto defaults = result.resolution == ActionResolution::Hit &&
                          result.animation == ActionAnimation::SkillInterrupt &&
                          result.param == 0 &&
                          result.messageID == MsgBasic::None;
    if (!defaults)
    {
        std::cerr << "ranged interrupt 2634 result fields failed\n";
    }
    return defaults;
}
