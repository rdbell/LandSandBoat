#include "map/action/interrupts.h"
#include "map/petskill.h"

#include <iostream>

namespace
{

auto matchesFinish(const action_t& action, const uint32 actorId, const uint32 targetId, const uint16 skillID, const ActionResolution resolution) -> bool
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
    return result.resolution == resolution &&
           result.animation == ActionAnimation::SkillInterrupt &&
           result.param == skillID &&
           result.messageID == MsgBasic::TooFarAwayRed;
}

auto matchesInterrupt(const action_t& action, const uint32 actorId) -> bool
{
    if (action.actorId != actorId ||
        action.actiontype != ActionCategory::SkillStart ||
        action.actionid != static_cast<uint32_t>(FourCC::SkillInterrupt) ||
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

auto runActionInterrupt7988SelfTests() -> bool
{
    constexpr uint32 actorId    = 0x10203040;
    constexpr uint32 targetId   = 0x50607080;
    constexpr uint16 petSkillID = 0x1234;
    constexpr uint16 mobSkillID = 0x2345;

    CPetSkill  petSkill(petSkillID);
    const auto avatarPetID = ActionInterrupts::detail::AvatarOutOfRangeAction(actorId, targetId, &petSkill);
    const auto wyvernPetID = ActionInterrupts::detail::WyvernOutOfRangeAction(actorId, targetId, &petSkill);
    const auto interrupt   = ActionInterrupts::detail::PetSkillInterruptAction(actorId);

    petSkill.setMobSkillID(mobSkillID);
    const auto avatarMobID = ActionInterrupts::detail::AvatarOutOfRangeAction(actorId, targetId, &petSkill);
    const auto wyvernMobID = ActionInterrupts::detail::WyvernOutOfRangeAction(actorId, targetId, &petSkill);

    if (!matchesFinish(avatarPetID, actorId, targetId, petSkillID, ActionResolution::Hit) ||
        !matchesFinish(wyvernPetID, actorId, targetId, petSkillID, ActionResolution::Miss) ||
        !matchesFinish(avatarMobID, actorId, targetId, mobSkillID, ActionResolution::Hit) ||
        !matchesFinish(wyvernMobID, actorId, targetId, mobSkillID, ActionResolution::Miss) ||
        !matchesInterrupt(interrupt, actorId))
    {
        std::cerr << "pet out-of-range action interrupt 7988 self-test failed\n";
        return false;
    }
    return true;
}
