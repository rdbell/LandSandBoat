-----------------------------------
-- Pure system tests for White Mage status-effect scripts
-- (afflatus_solace/misery, auspice, divine_caress, asylum,
-- sacrosanctity, divine_seal/emblem, elemental_seal).
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    local effectObjs = opts.effectObjs or {}
    return {
        mods = opts.mods or {},
        effects = effects,
        effectObjs = effectObjs,
        deletedEffects = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setMod = function(self, mod, value)
            self.mods[mod] = value
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] == true
        end,
        getStatusEffect = function(self, effect)
            return self.effectObjs[effect]
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
    }
end

local function stubEffect(power, subPower)
    return {
        power = power or 0,
        subPower = subPower or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        setSubPower = function(self, v)
            self.subPower = v
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

local statusMevaMods = {
    xi.mod.SLEEP_MEVA,
    xi.mod.POISON_MEVA,
    xi.mod.PARALYZE_MEVA,
    xi.mod.BLIND_MEVA,
    xi.mod.SILENCE_MEVA,
    xi.mod.VIRUS_MEVA,
    xi.mod.PETRIFY_MEVA,
    xi.mod.BIND_MEVA,
    xi.mod.CURSE_MEVA,
    xi.mod.GRAVITY_MEVA,
    xi.mod.SLOW_MEVA,
    xi.mod.STUN_MEVA,
    xi.mod.CHARM_MEVA,
    xi.mod.AMNESIA_MEVA,
    xi.mod.LULLABY_MEVA,
}

describe('WHM status-effect pure plans', function()
    it('afflatus solace applies solace flag and barspell mdef bonus', function()
        local script = require('scripts/effects/afflatus_solace')
        local target = stubTarget()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.AFFLATUS_SOLACE] == 0)
        assert(target.mods[xi.mod.BARSPELL_MDEF_BONUS] == 5)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.AFFLATUS_SOLACE] == 0)
        assert(target.mods[xi.mod.BARSPELL_MDEF_BONUS] == 0)
    end)

    it('afflatus misery sets pool and adds enspell when auspice active', function()
        local script = require('scripts/effects/afflatus_misery')
        local target = stubTarget({
            effects = { [xi.effect.AUSPICE] = true },
            effectObjs = {
                [xi.effect.AUSPICE] = stubEffect(42, 0),
            },
        })
        local effect = stubEffect(8, 15)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.AFFLATUS_MISERY] == 0)
        assert(target.mods[xi.mod.ENSPELL] == 18)
        assert(target.mods[xi.mod.ENSPELL_DMG] == 42)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.ACC] == -15)
        assert(target.mods[xi.mod.ENSPELL] == 0)
        assert(target.mods[xi.mod.ENSPELL_DMG] == 0)
    end)

    it('afflatus misery without auspice only sets pool and restores ACC on lose', function()
        local script = require('scripts/effects/afflatus_misery')
        local target = stubTarget()
        local effect = stubEffect(8, 20)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.AFFLATUS_MISERY] == 0)
        assert(target.mods[xi.mod.ENSPELL] == nil)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.ACC] == -20)
    end)

    it('auspice applies subtle blow and misery enspell combo', function()
        local script = require('scripts/effects/auspice')
        local misery = stubEffect(8, 5)
        local target = stubTarget({
            mods = { [xi.mod.AUSPICE_EFFECT] = 7 },
            effects = { [xi.effect.AFFLATUS_MISERY] = true },
            effectObjs = { [xi.effect.AFFLATUS_MISERY] = misery },
        })
        local effect = stubEffect(33, 0)
        script.onEffectGain(target, effect)
        -- subtle blow = 10 + AUSPICE_EFFECT 7 = 17
        assert(effect.subPower == 17)
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 17)
        assert(misery.subPower == 0)
        assert(target.mods[xi.mod.ENSPELL] == 18)
        assert(target.mods[xi.mod.ENSPELL_DMG] == 33)
        -- lose: ACC cleanup uses current misery subPower (0 after gain reset)
        misery.subPower = 12 -- simulate combat-built ACC bonus
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 0)
        assert(target.mods[xi.mod.ACC] == -12)
        assert(misery.subPower == 0)
        assert(target.mods[xi.mod.ENSPELL] == 0)
        assert(target.mods[xi.mod.ENSPELL_DMG] == 0)
    end)

    it('auspice without misery only applies subtle blow', function()
        local script = require('scripts/effects/auspice')
        local target = stubTarget({ mods = { [xi.mod.AUSPICE_EFFECT] = 0 } })
        local effect = stubEffect(10, 0)
        script.onEffectGain(target, effect)
        assert(effect.subPower == 10)
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 10)
        assert(target.mods[xi.mod.ENSPELL] == nil)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 0)
    end)

    it('divine caress applies 98 to all status MEVA mods on target', function()
        local script = require('scripts/effects/divine_caress')
        local target = stubTarget()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        for _, mod in ipairs(statusMevaMods) do
            assert(target.mods[mod] == 98, 'meva mod ' .. tostring(mod))
        end
        script.onEffectLose(target, effect)
        for _, mod in ipairs(statusMevaMods) do
            assert(target.mods[mod] == 0, 'meva lose ' .. tostring(mod))
        end
    end)

    it('asylum applies 98 status MEVA as effect-owned mods', function()
        local script = require('scripts/effects/asylum')
        local target = stubTarget()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        for _, mod in ipairs(statusMevaMods) do
            assert(effect.mods[mod] == 98, 'asylum effect mod ' .. tostring(mod))
            assert(target.mods[mod] == nil, 'asylum not on target')
        end
        -- lose is empty; effect-owned mods auto-clean
        script.onEffectLose(target, effect)
    end)

    it('sacrosanctity applies MDEF 75', function()
        local script = require('scripts/effects/sacrosanctity')
        local target = stubTarget()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.MDEF] == 75)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.MDEF] == 0)
    end)

    it('divine seal clears dark seal, divine emblem, elemental seal', function()
        local script = require('scripts/effects/divine_seal')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(#target.deletedEffects == 3)
        assert(target.deletedEffects[1] == xi.effect.DARK_SEAL)
        assert(target.deletedEffects[2] == xi.effect.DIVINE_EMBLEM)
        assert(target.deletedEffects[3] == xi.effect.ELEMENTAL_SEAL)
    end)

    it('divine emblem clears dark seal, divine seal, elemental seal', function()
        local script = require('scripts/effects/divine_emblem')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.deletedEffects[1] == xi.effect.DARK_SEAL)
        assert(target.deletedEffects[2] == xi.effect.DIVINE_SEAL)
        assert(target.deletedEffects[3] == xi.effect.ELEMENTAL_SEAL)
    end)

    it('elemental seal clears dark seal, divine emblem, divine seal', function()
        local script = require('scripts/effects/elemental_seal')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.deletedEffects[1] == xi.effect.DARK_SEAL)
        assert(target.deletedEffects[2] == xi.effect.DIVINE_EMBLEM)
        assert(target.deletedEffects[3] == xi.effect.DIVINE_SEAL)
    end)
end)
