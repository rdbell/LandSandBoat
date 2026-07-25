-----------------------------------
-- Ability: Elemental Sforzo
-- Dual-wired via xi.job_utils.rune_fencer (slice 6751)
-----------------------------------
---@type TAbility
local abilityObject = {}

abilityObject.onAbilityCheck = function(player, target, ability)
    ability:setRecast(xi.job_utils.rune_fencer.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

abilityObject.onUseAbility = function(player, target, ability)
    local p = xi.job_utils.rune_fencer.elementalSforzoFromParams()
    player:addStatusEffect(xi.effect.ELEMENTAL_SFORZO, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })
    return xi.effect.ELEMENTAL_SFORZO
end

return abilityObject
