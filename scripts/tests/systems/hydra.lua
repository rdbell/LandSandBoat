local hydra = require('scripts/mixins/families/hydra')

describe('Hydra mixin', function()
    it('breaks heads and regrows them using the source timing range', function()
        local defaultMin, defaultMax = xi.mix.hydra.regrowDelayRange(0, 0)
        assert(defaultMin == 120 and defaultMax == 240)

        -- The source reads headRegrowMin for both ends when a custom minimum
        -- is configured; retain that observable fixed-delay behavior.
        local customMin, customMax = xi.mix.hydra.regrowDelayRange(45, 180)
        assert(customMin == 45 and customMax == 45)

        assert(xi.mix.hydra.shouldBreakHead(0, 15, 15))
        assert(not xi.mix.hydra.shouldBreakHead(0, 15, 16))
        assert(not xi.mix.hydra.shouldBreakHead(2, 100, 1))
        assert(xi.mix.hydra.regrowAnimation(100, 99, 2) == nil)
        assert(xi.mix.hydra.regrowAnimation(100, 100, 2) == nil)
        assert(xi.mix.hydra.regrowAnimation(100, 101, 2) == 1)
        assert(xi.mix.hydra.regrowAnimation(100, 101, 0) == nil)

        local mob = { animationSub = 0, localVars = {}, listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:setLocalVar(name, value) self.localVars[name] = value end

        hydra(mob)
        assert(mob.listeners.CRITICAL_TAKE and mob.listeners.ROAM_TICK and mob.listeners.COMBAT_TICK)

        -- A guaranteed critical break installs a future head-regrow time.
        mob.localVars.headBreakChance = 100
        mob.listeners.CRITICAL_TAKE(mob)
        assert(mob.animationSub == 1 and mob.localVars.headgrow > 0)

        -- A past timer regrows one head, but an equal timestamp does not.
        mob.localVars.headgrow = 0
        mob.listeners.ROAM_TICK(mob)
        assert(mob.animationSub == 0 and mob.localVars.headgrow > 0)
    end)
end)
