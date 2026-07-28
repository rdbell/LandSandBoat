-- Dragon family variant introduced in ToAU and found various zones.

require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.red_dragon = xi.mix.red_dragon or {}

xi.mix.red_dragon.regainDelta = function(effectType, silence)
    if effectType == silence then return 150 end
    return 0
end

xi.mix.red_dragon.tpDelta = function(damageType, fire, water)
    if damageType == fire then return 500 end
    if damageType == water then return -300 end
    return 0
end

g_mixins.families.red_dragon = function(redDragonMob)
    -- mob has a regain of 150 per tick while silence effect is on
    redDragonMob:addListener('EFFECT_GAIN', 'RED_DRAGON_EFFECT_GAIN', function(mob, effect)
        local delta = xi.mix.red_dragon.regainDelta(effect:getEffectType(), xi.effect.SILENCE)
        if delta ~= 0 then
            mob:addMod(xi.mod.REGAIN, delta)
        end
    end)

    redDragonMob:addListener('EFFECT_LOSE', 'RED_DRAGON_EFFECT_LOSE', function(mob, effect)
        local delta = xi.mix.red_dragon.regainDelta(effect:getEffectType(), xi.effect.SILENCE)
        if delta ~= 0 then
            mob:delMod(xi.mod.REGAIN, delta)
        end
    end)

    -- fire damage will increase mob tp by 500 while water damage will decrease mob tp by 300
    redDragonMob:addListener('TAKE_DAMAGE', 'RED_DRAGON_TAKE_DAMAGE', function(mob, amount, attacker, attackType, damageType)
        local delta = xi.mix.red_dragon.tpDelta(damageType, xi.damageType.FIRE, xi.damageType.WATER)
        if delta > 0 then
            mob:addTP(delta)
        elseif delta < 0 then
            mob:delTP(-delta)
        end
    end)
end

return g_mixins.families.red_dragon
