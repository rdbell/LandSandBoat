#pragma once

namespace petconstructionhelpers
{

template <typename SetObjectType, typename SetEcosystem, typename SetAllegiance,
          typename ClearMobSkillList, typename EnableTargetIDRelease,
          typename SetSpecialSpawnAnimation, typename CreateAI>
inline void Apply(
    SetObjectType&& setObjectType,
    SetEcosystem&& setEcosystem,
    SetAllegiance&& setAllegiance,
    ClearMobSkillList&& clearMobSkillList,
    EnableTargetIDRelease&& enableTargetIDRelease,
    SetSpecialSpawnAnimation&& setSpecialSpawnAnimation,
    CreateAI&& createAI)
{
    setObjectType();
    setEcosystem();
    setAllegiance();
    clearMobSkillList();
    enableTargetIDRelease();
    setSpecialSpawnAnimation();
    createAI();
}

} // namespace petconstructionhelpers
