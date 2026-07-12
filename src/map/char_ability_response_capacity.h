#pragma once

// Pure notoriety ability-response gate from CCharEntity::OnAbility.
// Host iterates notoriety, casts to mob, and invokes OnPlayerAbilityUse.

namespace charabilityresponsehelpers
{

// ShouldNotify mirrors is-mob && MOBMOD_ABILITY_RESPONSE && same zone.
constexpr auto ShouldNotify(const bool isMob, const bool hasAbilityResponse, const bool sameZone) -> bool
{
    return isMob && hasAbilityResponse && sameZone;
}

} // namespace charabilityresponsehelpers
