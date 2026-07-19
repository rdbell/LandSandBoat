#pragma once

namespace mobcontrollerboundretargetadmission
{
// ShouldAttempt reports whether a bound mob should search its enmity list for a new target.
constexpr auto ShouldAttempt(const bool isMob, const bool hasBind, const bool isAttacking) -> bool
{
    return isMob && hasBind && isAttacking;
}
} // namespace mobcontrollerboundretargetadmission
