-----------------------------------
-- Ability: Embolden
-- Dual-wired via xi.job_utils.rune_fencer (slice 6751)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    return 0, 0
end

abilityObject.onUseAbility = function(player, target, ability, action)
    local p = xi.job_utils.rune_fencer.emboldenFromParams()
    target:addStatusEffect(xi.effect.EMBOLDEN, { duration = p.duration, origin = player })
end

return abilityObject
