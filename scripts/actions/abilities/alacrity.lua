-----------------------------------
-- Ability: Alacrity
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveEffect(player, xi.effect.ALACRITY)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useFixedStratagem(player, xi.effect.ALACRITY)
end

return abilityObject
