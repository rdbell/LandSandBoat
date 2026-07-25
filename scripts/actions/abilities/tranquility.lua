-----------------------------------
-- Ability: Tranquility
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveEffect(player, xi.effect.TRANQUILITY)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useMeritStratagem(player, xi.effect.TRANQUILITY, xi.merit.TRANQUILITY)
end

return abilityObject
