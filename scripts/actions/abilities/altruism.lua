-----------------------------------
-- Ability: Altruism
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveEffect(player, xi.effect.ALTRUISM)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useMeritStratagem(player, xi.effect.ALTRUISM, xi.merit.ALTRUISM)
end

return abilityObject
