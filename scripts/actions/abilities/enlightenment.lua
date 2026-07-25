-----------------------------------
-- Ability: Enlightenment
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveEffect(player, xi.effect.ENLIGHTENMENT)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useEnlightenment(player, target, ability)
end

return abilityObject
