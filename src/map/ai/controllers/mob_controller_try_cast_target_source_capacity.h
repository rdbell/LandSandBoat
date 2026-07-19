#pragma once

namespace mobcontrollertrycasttargetsource
{
enum class Source
{
    BattleTarget,
    Self,
    Override,
};

// Select reports the source for TryCastSpell's final cast target.
constexpr auto Select(const bool selfTarget, const bool hasOverride) -> Source
{
    if (hasOverride)
    {
        return Source::Override;
    }
    return selfTarget ? Source::Self : Source::BattleTarget;
}
} // namespace mobcontrollertrycasttargetsource
