local imp = require('scripts/mixins/families/imp')

describe('Imp mixin', function()
    it('breaks and restores horns using the configured delay ladder', function()
        assert(xi.mix.imp.shouldBreakHorn(20, 4) == true)
        assert(xi.mix.imp.shouldBreakHorn(21, 4) == false)
        assert(xi.mix.imp.shouldBreakHorn(1, 5) == false)
        assert(xi.mix.imp.restoreDelay(1) == 60)
        assert(xi.mix.imp.restoreDelay(2) == 60)
        assert(xi.mix.imp.restoreDelay(3) == 30)
        assert(xi.mix.imp.restoreDelay(10) == 35)
        assert(xi.mix.imp.restoreDelay(15) == 40)
        assert(xi.mix.imp.restoreDelay(20) == 45)

        local plan = xi.mix.imp.criticalPlan(3, 4, 0)
        assert(plan.animationSub == 5 and plan.restoreDelayMs == 30000)
        assert(xi.mix.imp.criticalPlan(21, 4, 0) == nil)
        assert(xi.mix.imp.criticalPlan(3, 4, 5).restoreDelayMs == nil)
        assert(xi.mix.imp.shouldRestoreHorn(true) == true)
        assert(xi.mix.imp.shouldRestoreHorn(false) == false)

        local mob = { animationSub = 4, listeners = {}, localVars = {}, timers = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:timer(delay, callback) table.insert(self.timers, { delay, callback }) end
        function mob:isAlive() return true end

        imp(mob)
        mob.localVars.hornDisabled = 5
        assert(mob.listeners.CRITICAL_TAKE ~= nil)
    end)
end)
