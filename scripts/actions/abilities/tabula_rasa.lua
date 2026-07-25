-----------------------------------
-- Ability: Tabula Rasa
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkTabulaRasa(player, target, ability)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useTabulaRasa(player, target, ability)
end

return abilityObject
