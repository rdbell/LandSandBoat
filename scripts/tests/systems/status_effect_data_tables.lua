-----------------------------------
-- Pure system tests for status_effect_tables nullification catalog.
-----------------------------------

describe('Status effect data table pure plans', function()
    it('getNullificatingEffect returns always-nullify column', function()
        assert(xi.data.statusEffect.getNullificatingEffect(xi.effect.STUN) == xi.effect.PETRIFICATION)
        assert(xi.data.statusEffect.getNullificatingEffect(xi.effect.BURN) == xi.effect.DROWN)
        assert(xi.data.statusEffect.getNullificatingEffect(xi.effect.NOCTURNE) == xi.effect.ADDLE)
        assert(xi.data.statusEffect.getNullificatingEffect(xi.effect.POISON) == 0)
        assert(xi.data.statusEffect.getNullificatingEffect(9999) == 0)
    end)

    it('getNullificatingEffectByTier returns tier-nullify column', function()
        assert(xi.data.statusEffect.getNullificatingEffectByTier(xi.effect.BIO) == xi.effect.DIA)
        assert(xi.data.statusEffect.getNullificatingEffectByTier(xi.effect.DIA) == xi.effect.BIO)
        assert(xi.data.statusEffect.getNullificatingEffectByTier(xi.effect.HASTE) == xi.effect.SLOW)
        assert(xi.data.statusEffect.getNullificatingEffectByTier(xi.effect.SLOW) == xi.effect.HASTE)
        assert(xi.data.statusEffect.getNullificatingEffectByTier(xi.effect.POISON) == 0)
    end)

    it('getEffectToRemove returns EFFECT_NULLIFIES column', function()
        assert(xi.data.statusEffect.getEffectToRemove(xi.effect.ADDLE) == xi.effect.NOCTURNE)
        assert(xi.data.statusEffect.getEffectToRemove(xi.effect.BURN) == xi.effect.FROST)
        assert(xi.data.statusEffect.getEffectToRemove(xi.effect.POISON) == 0)
    end)

    it('getAssociatedElement has sleep exception and table lookup', function()
        assert(xi.data.statusEffect.getAssociatedElement(xi.effect.SLEEP_I, xi.element.LIGHT) == xi.element.LIGHT)
        assert(xi.data.statusEffect.getAssociatedElement(xi.effect.SLEEP_I, xi.element.DARK) == xi.element.DARK)
        assert(xi.data.statusEffect.getAssociatedElement(xi.effect.POISON, xi.element.FIRE) == xi.element.WATER)
        assert(xi.data.statusEffect.getAssociatedElement(9999, xi.element.THUNDER) == xi.element.THUNDER)
    end)

    it('isEffectNullified always path ignores tier', function()
        local target = {
            effects = { [xi.effect.PETRIFICATION] = { tier = 0 } },
            hasStatusEffect = function(self, id)
                return self.effects[id] ~= nil
            end,
            getStatusEffect = function(self, id)
                return self.effects[id]
            end,
        }
        assert(xi.data.statusEffect.isEffectNullified(target, xi.effect.STUN, 0))
        target.effects = {}
        assert(not xi.data.statusEffect.isEffectNullified(target, xi.effect.STUN, 0))
    end)

    it('isEffectNullified same-effect higher tier blocks', function()
        local target = {
            effects = { [xi.effect.POISON] = { tier = 2, getTier = function(self) return self.tier end } },
            hasStatusEffect = function(self, id)
                return self.effects[id] ~= nil
            end,
            getStatusEffect = function(self, id)
                return self.effects[id]
            end,
        }
        assert(xi.data.statusEffect.isEffectNullified(target, xi.effect.POISON, 1))
        assert(xi.data.statusEffect.isEffectNullified(target, xi.effect.POISON, 2))
        assert(not xi.data.statusEffect.isEffectNullified(target, xi.effect.POISON, 3))
    end)

    it('isEffectNullified by-tier cross-effect path', function()
        local target = {
            effects = { [xi.effect.DIA] = { tier = 2, getTier = function(self) return self.tier end } },
            hasStatusEffect = function(self, id)
                return self.effects[id] ~= nil
            end,
            getStatusEffect = function(self, id)
                return self.effects[id]
            end,
        }
        assert(xi.data.statusEffect.isEffectNullified(target, xi.effect.BIO, 1))
        assert(xi.data.statusEffect.isEffectNullified(target, xi.effect.BIO, 2))
        assert(not xi.data.statusEffect.isEffectNullified(target, xi.effect.BIO, 3))
        -- tier 0 skips by-tier after always check
        assert(not xi.data.statusEffect.isEffectNullified(target, xi.effect.BIO, 0))
    end)

    it('catalog has 38 effect rows', function()
        local n = 0
        for _ in pairs(xi.data.statusEffect.dataTable) do
            n = n + 1
        end
        assert(n == 38)
    end)
end)
