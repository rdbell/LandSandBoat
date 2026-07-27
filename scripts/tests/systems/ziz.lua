local ziz = require('scripts/mixins/families/ziz')

describe('Ziz mixin', function()
    it('sleeps at night and wakes during the day', function()
        local night = xi.mix.ziz.sleepPlan(xi.time.NIGHT, 1)
        assert(night.animationSub == 3 and night.aggressive == false and night.noMove == 1)
        assert(xi.mix.ziz.sleepPlan(xi.time.MIDNIGHT, 3) == nil)

        local day = xi.mix.ziz.sleepPlan(xi.time.DAY, 3)
        assert(day.animationSub == 1 and day.aggressive == true and day.noMove == 0)
        assert(xi.mix.ziz.sleepPlan(xi.time.DAY, 1) == nil)

        local mob = { animationSub = 3, aggressive = false, listeners = {}, mobMods = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:setAggressive(value) self.aggressive = value end
        function mob:setMobMod(mod, value) self.mobMods[mod] = value end

        ziz(mob)
        mob.listeners.ENGAGE(mob)
        assert(mob.animationSub == 1 and mob.aggressive == true and mob.mobMods[xi.mobMod.NO_MOVE] == 0)
    end)
end)
