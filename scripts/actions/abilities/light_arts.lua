-----------------------------------
-- Ability: Light Arts
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkLightArts(player, target, ability)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useLightArts(player, target, ability)
end

return abilityObject
