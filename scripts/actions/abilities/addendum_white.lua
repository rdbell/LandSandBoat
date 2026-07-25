-----------------------------------
-- Ability: Addendum: White
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return xi.job_utils.scholar.checkAddendumWhite(player, target, ability)
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useAddendumWhite(player, target, ability)
end

return abilityObject
