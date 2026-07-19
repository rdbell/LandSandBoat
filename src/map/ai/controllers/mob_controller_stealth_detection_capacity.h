#pragma once

namespace mobcontrollerstealthdetection
{
struct State
{
    bool hasInvisible;
    bool hasSneak;
};

// Resolve returns the effective base stealth statuses before Illusion is applied.
template <typename TargetInvisible, typename TargetSneak>
constexpr auto Resolve(
    const bool trueDetection,
    TargetInvisible&& targetInvisible,
    TargetSneak&& targetSneak) -> State
{
    if (trueDetection)
    {
        return { false, false };
    }
    return { targetInvisible(), targetSneak() };
}
} // namespace mobcontrollerstealthdetection
