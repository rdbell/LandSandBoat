#pragma once

namespace trustmobskillfinishedhelpers
{

template <typename BaseMobSkillFinished>
inline void Apply(BaseMobSkillFinished&& baseMobSkillFinished)
{
    baseMobSkillFinished();
}

} // namespace trustmobskillfinishedhelpers
