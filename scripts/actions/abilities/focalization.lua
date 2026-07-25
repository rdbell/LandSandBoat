-----------------------------------
-- Ability: Focalization
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveEffect(player, xi.effect.FOCALIZATION)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useMeritStratagem(player, xi.effect.FOCALIZATION, xi.merit.FOCALIZATION)
end

return abilityObject
