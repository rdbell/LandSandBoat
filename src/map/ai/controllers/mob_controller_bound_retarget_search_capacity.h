#pragma once

namespace mobcontrollerboundretargetsearch
{
// ShouldSearch reports whether Bind retargeting may scan the enmity list.
constexpr auto ShouldSearch(const bool hasCurrentTarget, const bool canAttackCurrentTarget, const bool hasEnmityContainer) -> bool
{
    return hasCurrentTarget && !canAttackCurrentTarget && hasEnmityContainer;
}
} // namespace mobcontrollerboundretargetsearch
