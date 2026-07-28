require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.khimaira = xi.mix.khimaira or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.khimaira.shouldDropWings = function(random, animationSub)
    return random <= 5 and animationSub == 0
end

g_mixins.families.khimaira = function(khimairaMob)
    -- 5% chance to bring wings down on critical hit
    -- TODO: Unknown if 5% chance is correct
    khimairaMob:addListener('CRITICAL_TAKE', 'KHIMAIRA_CRITICAL_TAKE', function(mob)
        local random = math.random(1, 100)

        if xi.mix.khimaira.shouldDropWings(random, mob:getAnimationSub()) then
            mob:setAnimationSub(1)
        end
    end)
end

return g_mixins.families.khimaira
