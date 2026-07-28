local redDragon = require('scripts/mixins/families/red_dragon')
describe('Red Dragon mixin', function()
    it('grants regain for Silence and changes TP for Fire and Water', function()
        assert(xi.mix.red_dragon.regainDelta(xi.effect.SILENCE, xi.effect.SILENCE) == 150)
        assert(xi.mix.red_dragon.regainDelta(0, xi.effect.SILENCE) == 0)
        assert(xi.mix.red_dragon.tpDelta(xi.damageType.FIRE, xi.damageType.FIRE, xi.damageType.WATER) == 500)
        assert(xi.mix.red_dragon.tpDelta(xi.damageType.WATER, xi.damageType.FIRE, xi.damageType.WATER) == -300)
        assert(xi.mix.red_dragon.tpDelta(0, xi.damageType.FIRE, xi.damageType.WATER) == 0)
        assert(redDragon ~= nil)

        local mob = { listeners = {}, regain = 0, tp = 1000 }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:addMod(_, value) self.regain = self.regain + value end
        function mob:delMod(_, value) self.regain = self.regain - value end
        function mob:addTP(value) self.tp = self.tp + value end
        function mob:delTP(value) self.tp = self.tp - value end

        redDragon(mob)
        assert(mob.listeners.EFFECT_GAIN and mob.listeners.EFFECT_LOSE and mob.listeners.TAKE_DAMAGE)

        local silence = { getEffectType = function() return xi.effect.SILENCE end }
        local otherEffect = { getEffectType = function() return 0 end }
        mob.listeners.EFFECT_GAIN(mob, silence)
        assert(mob.regain == 150)
        mob.listeners.EFFECT_GAIN(mob, otherEffect)
        assert(mob.regain == 150)
        mob.listeners.EFFECT_LOSE(mob, silence)
        assert(mob.regain == 0)

        mob.listeners.TAKE_DAMAGE(mob, 0, nil, nil, xi.damageType.FIRE)
        assert(mob.tp == 1500)
        mob.listeners.TAKE_DAMAGE(mob, 0, nil, nil, xi.damageType.WATER)
        assert(mob.tp == 1200)
        mob.listeners.TAKE_DAMAGE(mob, 0, nil, nil, xi.damageType.NONE)
        assert(mob.tp == 1200)
    end)
end)
