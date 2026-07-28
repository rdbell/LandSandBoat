local gnole = require('scripts/mixins/families/gnole')

describe('Gnole mixin', function()
    it('toggles form at roaming and combat transform boundaries', function()
        assert(xi.mix.gnole.spawnPlan(1000).transformTime == 1000)
        assert(xi.mix.gnole.transformPlan(1299, 1000, 0, 300) == nil)
        local roam = xi.mix.gnole.transformPlan(1300, 1000, 0, 300)
        assert(roam.animationSub == 1 and roam.transformTime == 1300)
        local combat = xi.mix.gnole.transformPlan(1360, 1300, 1, 60)
        assert(combat.animationSub == 0 and combat.transformTime == 1360)
        assert(xi.mix.gnole.transformPlan(1299, 1300, 1, 60) == nil)
        assert(xi.mix.gnole.transformPlan(100, 0, 2, 60).animationSub == 1)

        local mob = { animationSub = 0, localVars = {}, listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:setLocalVar(name, value) self.localVars[name] = value end

        gnole(mob)
        assert(mob.listeners.SPAWN and mob.listeners.ROAM_TICK and mob.listeners.COMBAT_TICK)
        mob.listeners.SPAWN(mob)
        local spawnedAt = mob.localVars.transformTime
        assert(spawnedAt > 0)
        mob.listeners.ROAM_TICK(mob)
        assert(mob.animationSub == 0 and mob.localVars.transformTime == spawnedAt)
        mob.localVars.transformTime = 0
        mob.listeners.ROAM_TICK(mob)
        assert(mob.animationSub == 1 and mob.localVars.transformTime > 0)
        mob.localVars.transformTime = 0
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.animationSub == 0 and mob.localVars.transformTime > 0)
    end)
end)
