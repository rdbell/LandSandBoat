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

describe('Status effect associated lookups and resistance pure plans', function()
    it('getAssociatedImmunity has sleep light exception', function()
        assert(xi.data.statusEffect.getAssociatedImmunity(xi.effect.SLEEP_I, xi.element.LIGHT) == xi.immunity.LIGHT_SLEEP)
        assert(xi.data.statusEffect.getAssociatedImmunity(xi.effect.SLEEP_I, xi.element.DARK) == xi.immunity.DARK_SLEEP)
        assert(xi.data.statusEffect.getAssociatedImmunity(xi.effect.POISON, xi.element.WATER) == xi.immunity.POISON)
        assert(xi.data.statusEffect.getAssociatedImmunity(xi.effect.AMNESIA, xi.element.FIRE) == 0)
    end)

    it('getAssociatedResistTrait and rank modifiers', function()
        assert(xi.data.statusEffect.getAssociatedResistTraitModifier(xi.effect.POISON) == xi.mod.POISONRES)
        assert(xi.data.statusEffect.getAssociatedResistTraitModifier(xi.effect.TERROR) == 0)
        assert(xi.data.statusEffect.getAssociatedResistanceRankModifier(xi.effect.SLEEP_I, xi.element.LIGHT) == xi.mod.LIGHT_SLEEP_RES_RANK)
        assert(xi.data.statusEffect.getAssociatedResistanceRankModifier(xi.effect.BIND, xi.element.ICE) == xi.mod.BIND_RES_RANK)
    end)

    it('getAssociatedMagicEvasion and immunobreak modifiers', function()
        assert(xi.data.statusEffect.getAssociatedMagicEvasionModifier(xi.effect.POISON) == xi.mod.POISON_MEVA)
        assert(xi.data.statusEffect.getAssociatedImmunobreakModifier(xi.effect.POISON) == xi.mod.POISON_IMMUNOBREAK)
        assert(xi.data.statusEffect.getAssociatedMagicEvasionModifier(xi.effect.ATTACK_DOWN) == 0)
    end)

    it('isResistRateSuccessfull thresholds from allowed resist state', function()
        -- blindness allowed = 1 → threshold 0.5
        assert(xi.data.statusEffect.isResistRateSuccessfull(xi.effect.BLINDNESS, 0.5, 0))
        assert(not xi.data.statusEffect.isResistRateSuccessfull(xi.effect.BLINDNESS, 0.49, 0))
        -- bio allowed = 4 → 1/16
        local thr = 1 / 2 ^ 4
        assert(xi.data.statusEffect.isResistRateSuccessfull(xi.effect.BIO, thr, 0))
        assert(not xi.data.statusEffect.isResistRateSuccessfull(xi.effect.BIO, thr - 0.001, 0))
        -- bypass 3 → 0.125
        assert(xi.data.statusEffect.isResistRateSuccessfull(xi.effect.BLINDNESS, 0.125, 3))
        assert(not xi.data.statusEffect.isResistRateSuccessfull(xi.effect.BLINDNESS, 0.124, 3))
    end)

    it('isTargetImmune requires mob and matching immunity', function()
        local target = {
            isMob = function() return true end,
            immunities = { [xi.immunity.POISON] = true },
            hasImmunity = function(self, id)
                return self.immunities[id] == true
            end,
        }
        assert(xi.data.statusEffect.isTargetImmune(target, xi.effect.POISON, xi.element.WATER))
        target.immunities = {}
        assert(not xi.data.statusEffect.isTargetImmune(target, xi.effect.POISON, xi.element.WATER))
        target.isMob = function() return false end
        target.immunities = { [xi.immunity.POISON] = true }
        assert(not xi.data.statusEffect.isTargetImmune(target, xi.effect.POISON, xi.element.WATER))
    end)

    it('isTargetResistant power formula with NM half', function()
        -- Pure arithmetic mirror (RNG not exercised).
        local function planResistant(effectId, traitMod, statusRes, isNM)
            local modId = xi.data.statusEffect.getAssociatedResistTraitModifier(effectId)
            if modId == 0 then
                return { shouldRoll = false, power = 0 }
            end
            local power = traitMod + statusRes + 5
            if power <= 5 then
                return { shouldRoll = false, power = 0 }
            end
            if isNM then
                power = math.floor(power / 2)
            end
            return { shouldRoll = true, power = power }
        end
        local p = planResistant(xi.effect.TERROR, 50, 10, false)
        assert(not p.shouldRoll)
        p = planResistant(xi.effect.POISON, 20, 0, false)
        assert(p.shouldRoll and p.power == 25)
        p = planResistant(xi.effect.POISON, 0, 0, false)
        assert(not p.shouldRoll)
        p = planResistant(xi.effect.POISON, 20, 10, true)
        assert(p.shouldRoll and p.power == 17)
    end)
end)
