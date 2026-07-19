#pragma once

namespace mobcontrollerpartylinkmembereligibility
{
// IsEligible reports whether a party member can be considered for a link.
template <typename HasMaster, typename IsDead>
constexpr auto IsEligible(const bool isMob, HasMaster&& hasMaster, IsDead&& isDead) -> bool
{
    return isMob && !hasMaster() && !isDead();
}
} // namespace mobcontrollerpartylinkmembereligibility
