#pragma once

#include <cstdint>
#include <functional>

namespace charitemfinishtargetshelpers
{

template <typename Target, typename ResetTargetFind, typename FindWithinArea, typename ProcessTarget>
inline void Apply(const bool aoe,
                  Target initialTarget,
                  const std::uint8_t findFlags,
                  const std::uint16_t validTarget,
                  ResetTargetFind&& resetTargetFind,
                  FindWithinArea&&  findWithinArea,
                  ProcessTarget&&   processTarget)
{
    if (aoe)
    {
        std::invoke(resetTargetFind);
        auto&& targets = std::invoke(findWithinArea, 10.0F, findFlags, validTarget);
        for (auto&& target : targets)
        {
            std::invoke(processTarget, target);
        }
        return;
    }
    std::invoke(processTarget, initialTarget);
}

} // namespace charitemfinishtargetshelpers
