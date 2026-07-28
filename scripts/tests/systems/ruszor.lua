local ruszor = require('scripts/mixins/families/ruszor')

describe('Ruszor mixin', function()
    it('selects elemental absorption from Stoneskin and clears it on the next tick', function()
        local ice = xi.mix.ruszor.combatPlan(1, true, 0, 100)
        assert(ice.iceAbsorb == 100 and ice.waterAbsorb == 0 and ice.animationSub == nil)
        assert(xi.mix.ruszor.combatPlan(1, true, 100, 0) == nil)
        local water = xi.mix.ruszor.combatPlan(2, true, 100, 0)
        assert(water.iceAbsorb == 0 and water.waterAbsorb == 100 and water.animationSub == nil)
        assert(xi.mix.ruszor.combatPlan(2, true, 0, 100) == nil)
        local reset = xi.mix.ruszor.combatPlan(2, false, 100, 100)
        assert(reset.animationSub == 0 and reset.iceAbsorb == 0 and reset.waterAbsorb == 0)
        assert(xi.mix.ruszor.combatPlan(0, false, 100, 100) == nil)

        local mob = { animationSub = 1, listeners = {}, mods = {}, stoneskin = true }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getAnimationSub() return self.animationSub end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:hasStatusEffect() return self.stoneskin end
        function mob:getMod(mod) return self.mods[mod] or 0 end
        function mob:setMod(mod, value) self.mods[mod] = value end

        ruszor(mob)
        assert(mob.listeners.COMBAT_TICK)
        mob.mods[xi.mod.WATER_ABSORB] = 100
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.mods[xi.mod.ICE_ABSORB] == 100 and mob.mods[xi.mod.WATER_ABSORB] == 0)
        mob.animationSub = 2
        mob.mods[xi.mod.WATER_ABSORB] = 0
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.mods[xi.mod.ICE_ABSORB] == 0 and mob.mods[xi.mod.WATER_ABSORB] == 100)
        mob.stoneskin = false
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.animationSub == 0 and mob.mods[xi.mod.ICE_ABSORB] == 0 and mob.mods[xi.mod.WATER_ABSORB] == 0)
    end)
end)
