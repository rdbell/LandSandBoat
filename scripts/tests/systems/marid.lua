local marid = require('scripts/mixins/families/marid')

describe('Marid mixin', function()
    it('caps tusk breaks and adds one guaranteed tusk per broken tusk', function()
        assert(xi.mix.marid.shouldBreakTusk(20, 0) == true)
        assert(xi.mix.marid.shouldBreakTusk(21, 0) == false)
        assert(xi.mix.marid.shouldBreakTusk(1, 2) == false)
        assert(xi.mix.marid.tuskDropCount(0) == 0)
        assert(xi.mix.marid.tuskDropCount(2) == 2)

        local mob = { animationSub = 2, listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end

        local loot = { items = {} }
        function loot:addItem(item, rate) table.insert(self.items, { item, rate }) end

        marid(mob)
        mob.listeners.ITEM_DROPS(mob, loot)
        assert(#loot.items == 2)
        for _, item in ipairs(loot.items) do
            assert(item[1] == xi.item.MARID_TUSK)
            assert(item[2] == xi.drop_rate.GUARANTEED)
        end
    end)
end)
