require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.orobon = xi.mix.orobon or {}

xi.mix.orobon.shouldBreakEyestalks = function(animationSub, roll)
    return animationSub == 0 and roll <= 10
end

xi.mix.orobon.hasBrokenEyestalks = function(animationSub)
    return animationSub == 1
end

g_mixins.families.orobon = function(orobonMob)
    -- 10% chance to break eyestalks on critical hit
    orobonMob:addListener('CRITICAL_TAKE', 'OROBON_CRITICAL_TAKE', function(mob)
        if xi.mix.orobon.shouldBreakEyestalks(mob:getAnimationSub(), math.random(1, 100)) then
            mob:setAnimationSub(1)
        end
    end)

    -- Add Orobon Lures to loot pool if eyestalks are broken
    orobonMob:addListener('ITEM_DROPS', 'OROBON_ITEM_DROPS', function(mob, loot)
        if xi.mix.orobon.hasBrokenEyestalks(mob:getAnimationSub()) then
            loot:addItem(xi.item.OROBON_LURE, xi.drop_rate.GUARANTEED)
        end
    end)
end

return g_mixins.families.orobon
