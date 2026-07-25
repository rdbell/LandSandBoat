-----------------------------------
-- Ability: Modus Veritas
-- Dual-wired via xi.job_utils.scholar (slice 6749)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return 0, 0
end

abilityObject.onUseAbility = function(player, target, ability)
    return xi.job_utils.scholar.useModusVeritas(player, target, ability)
end

return abilityObject
