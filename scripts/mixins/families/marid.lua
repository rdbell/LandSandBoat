require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.marid = xi.mix.marid or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.marid.shouldBreakTusk = function(random, brokenTusks)
    return random <= 20 and brokenTusks < 2
end

xi.mix.marid.tuskDropCount = function(brokenTusks)
    return brokenTusks
end

g_mixins.families.marid = function(maridMob)
    -- 20% chance to break tusk on critical hit
    maridMob:addListener('CRITICAL_TAKE', 'MARID_CRITICAL_TAKE', function(mob)
        local brokenTusks = mob:getAnimationSub()

        if
            xi.mix.marid.shouldBreakTusk(math.random(1, 100), brokenTusks)
        then
            mob:setAnimationSub(brokenTusks + 1)
        end
    end)

    -- Add Tusks to loot pool depending on number of broken tusks.
    maridMob:addListener('ITEM_DROPS', 'MARID_ITEM_DROPS', function(mob, loot)
        local brokenTusks = mob:getAnimationSub()

        for _ = 1, xi.mix.marid.tuskDropCount(brokenTusks) do
            loot:addItem(xi.item.MARID_TUSK, xi.drop_rate.GUARANTEED)
        end
    end)
end

return g_mixins.families.marid
