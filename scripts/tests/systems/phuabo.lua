local phuabo = require('scripts/mixins/families/phuabo')

describe('Phuabo mixin', function()
    it('hides while idle and exposes itself when engaged', function()
        local hidden = xi.mix.phuabo.statePlan(false)
        assert(hidden.hidden and hidden.untargetable and hidden.animationSub == 5 and hidden.waitMs == 2000)
        local engaged = xi.mix.phuabo.statePlan(true)
        assert(not engaged.hidden and not engaged.untargetable and engaged.animationSub == 6 and engaged.waitMs == 2000)

        local mob = { listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:hideName(value) self.hidden = value end
        function mob:setUntargetable(value) self.untargetable = value end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:wait(value) self.waitMs = value end

        phuabo(mob)
        mob.listeners.SPAWN(mob)
        assert(mob.hidden and mob.untargetable and mob.animationSub == 5 and mob.waitMs == 2000)
        mob.listeners.ENGAGE(mob)
        assert(not mob.hidden and not mob.untargetable and mob.animationSub == 6 and mob.waitMs == 2000)
        mob.listeners.DISENGAGE(mob)
        assert(mob.hidden and mob.untargetable and mob.animationSub == 5 and mob.waitMs == 2000)
    end)
end)
