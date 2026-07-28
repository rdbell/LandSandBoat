local qutrub = require('scripts/mixins/families/qutrub')

describe('Qutrub mixin', function()
    it('breaks weapons and swaps the surviving weapon on a strict timer', function()
        assert(xi.mix.qutrub.swapPlan(160, 160, 1) == nil)
        local firstSwap = xi.mix.qutrub.swapPlan(161, 160, 1)
        assert(firstSwap.animationSub == 2 and firstSwap.swapTime == 221)
        local secondSwap = xi.mix.qutrub.swapPlan(222, 221, 2)
        assert(secondSwap.animationSub == 1 and secondSwap.swapTime == 282)
        assert(xi.mix.qutrub.swapPlan(300, 0, 1) == nil)
        assert(xi.mix.qutrub.swapPlan(300, 299, 3) == nil)

        assert(xi.mix.qutrub.breakPlan(100, 0, 10, 11) == nil)
        local firstBreak = xi.mix.qutrub.breakPlan(100, 0, 10, 10)
        assert(firstBreak.animationSub == 1 and firstBreak.swapTime == 160)
        local alternateFirstBreak = xi.mix.qutrub.breakPlan(100, 4, 100, 1)
        assert(alternateFirstBreak.animationSub == 1 and alternateFirstBreak.swapTime == 160)
        local secondBreak = xi.mix.qutrub.breakPlan(100, 2, 100, 1)
        assert(secondBreak.animationSub == 3 and secondBreak.swapTime == 0)
        assert(xi.mix.qutrub.breakPlan(100, 1, 100, 1) == nil)

        local mob = { animationSub = 0, localVars = {}, listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:setLocalVar(name, value) self.localVars[name] = value end

        qutrub(mob)
        assert(mob.listeners.SPAWN and mob.listeners.COMBAT_TICK and mob.listeners.CRITICAL_TAKE)
        mob.listeners.SPAWN(mob)
        assert(mob.localVars.qutrubBreakChance == 10)
        mob.localVars.qutrubBreakChance = 100
        mob.listeners.CRITICAL_TAKE(mob)
        assert(mob.animationSub == 1 and mob.localVars.swapTime > 0)
        mob.localVars.swapTime = 0
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.animationSub == 1)
    end)
end)
