local flan = require('scripts/mixins/families/flan')

describe('Flan mixin', function()
    it('adapts to accumulated physical or magical damage at strict thresholds', function()
        assert(xi.mix.flan.damagePlan(1000, 299, 0, 1, true) == nil)
        local physical = xi.mix.flan.damagePlan(1000, 300, 0, 1, true)
        assert(physical.animationSub == 2 and physical.physical == 0 and physical.magical == nil and physical.damage == 0)
        local ranged = xi.mix.flan.damagePlan(1000, 0, 0, 101, true)
        assert(ranged.animationSub == 2)
        local magical = xi.mix.flan.damagePlan(1000, 0, 300, 1, false)
        assert(magical.animationSub == 1 and magical.magical == 0 and magical.physical == nil and magical.damage == 1)

        local mob = { maxHP = 1000, localVars = { physical = 300 }, mods = {}, listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getMaxHP() return self.maxHP end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:setLocalVar(name, value) self.localVars[name] = value end
        function mob:setAnimationSub(value) self.animationSub = value end
        function mob:setMod(mod, value) self.mods[mod] = value end

        flan(mob)
        assert(mob.listeners.TAKE_DAMAGE)
        mob.listeners.TAKE_DAMAGE(mob, 1, nil, xi.attackType.PHYSICAL)
        assert(mob.animationSub == 2 and mob.localVars.physical == 0 and mob.localVars.Damage == 0)
        assert(mob.mods[xi.mod.DMGPHYS] == -5000 and mob.mods[xi.mod.DMGRANGE] == -5000 and mob.mods[xi.mod.DMGMAGIC] == 0)
    end)
end)
