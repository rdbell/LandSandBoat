-----------------------------------
-- xi.effect.AVATARS_FAVOR
-- Tick power dual-wired via xi.avatarsFavor.tickPowerFromParams (slice 6725).
-----------------------------------
require('scripts/globals/avatars_favor')
-----------------------------------
---@type TEffect
local effectObject = {}

effectObject.onEffectGain = function(target, effect)
    xi.avatarsFavor.applyAvatarsFavorAuraToPet(target, effect)
    xi.avatarsFavor.applyAvatarsFavorDebuffsToPet(target)
end

effectObject.onEffectTick = function(target, effect)
    local newPower = xi.avatarsFavor.tickPowerFromParams({
        currentPower   = effect:getPower(),
        summoningSkill = target:getSkillLevel(xi.skill.SUMMONING_MAGIC),
        gearEnhance    = target:getMaxGearMod(xi.mod.AVATARS_FAVOR_ENHANCE),
    })
    effect:setPower(newPower)

    -- TODO add Job Point Gift Bonus
    -- if GET PLAYERS JP TOTAL >= 550 then
    --    effect:setPower(effect:getPower() + 1)
    -- end

    xi.avatarsFavor.applyAvatarsFavorAuraToPet(target, effect)
end

effectObject.onEffectLose = function(target, effect)
    xi.avatarsFavor.removeAvatarsFavorAuraFromPet(target)
end

return effectObject
