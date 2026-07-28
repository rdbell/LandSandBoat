local orobon = require('scripts/mixins/families/orobon')

describe('Orobon mixin', function()
    it('breaks eyestalks on a qualifying critical hit and guarantees the lure drop', function()
        assert(xi.mix.orobon.shouldBreakEyestalks(0, 10))
        assert(not xi.mix.orobon.shouldBreakEyestalks(0, 11))
        assert(not xi.mix.orobon.shouldBreakEyestalks(1, 1))
        assert(xi.mix.orobon.hasBrokenEyestalks(1))
        assert(not xi.mix.orobon.hasBrokenEyestalks(0))

        local mob = { animationSub = 0, listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        local loot = {}
        function loot:addItem(item, rate) self.item, self.rate = item, rate end

        orobon(mob)
        assert(mob.listeners.CRITICAL_TAKE and mob.listeners.ITEM_DROPS)

        local random = math.random
        math.random = function() return 10 end
        mob.listeners.CRITICAL_TAKE(mob)
        math.random = random
        assert(mob.animationSub == 1)
        mob.listeners.ITEM_DROPS(mob, loot)
        assert(loot.item == xi.item.OROBON_LURE and loot.rate == xi.drop_rate.GUARANTEED)

        mob.animationSub = 0
        math.random = function() return 11 end
        mob.listeners.CRITICAL_TAKE(mob)
        math.random = random
        assert(mob.animationSub == 0)
        loot.item, loot.rate = nil, nil
        mob.listeners.ITEM_DROPS(mob, loot)
        assert(loot.item == nil and loot.rate == nil)
    end)
end)
