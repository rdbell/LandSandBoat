-----------------------------------
-- Pure system tests for main-stat catalog, evasion, and magic combat-stat
-- status-effect scripts (complements combat_stat_effects.lua).
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        stats = opts.stats or {},
        deletedEffects = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getStat = function(self, mod)
            return self.stats[mod] or 0
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
    }
end

local function stubEffect(power, subPower)
    return {
        power = power,
        subPower = subPower or 0,
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        setPower = function(self, v)
            self.power = v
        end,
        addMod = function(self, mod, delta)
            self._mod = { mod = mod, delta = delta }
        end,
    }
end

describe('Main-stat, evasion, and magic status-effect pure plans', function()
    local mainBoosts = {
        { 'dex_boost', xi.mod.DEX },
        { 'vit_boost', xi.mod.VIT },
        { 'int_boost', xi.mod.INT },
        { 'mnd_boost', xi.mod.MND },
        { 'chr_boost', xi.mod.CHR },
    }

    for _, entry in ipairs(mainBoosts) do
        local name, mod = entry[1], entry[2]
        it(name .. ' gains/ticks/loses shared stat-boost pattern', function()
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            local effect = stubEffect(4, 0)
            script.onEffectGain(target, effect)
            assert(target.mods[mod] == 4)
            script.onEffectTick(target, effect)
            assert(effect.power == 3 and target.mods[mod] == 3)
            script.onEffectLose(target, effect)
            assert(target.mods[mod] == 0)
        end)
    end

    local mainDowns = {
        { 'dex_down', xi.mod.DEX, xi.effect.DEX_DOWN },
        { 'vit_down', xi.mod.VIT, xi.effect.VIT_DOWN },
        { 'int_down', xi.mod.INT, xi.effect.INT_DOWN },
        { 'mnd_down', xi.mod.MND, xi.effect.MND_DOWN },
        { 'chr_down', xi.mod.CHR, xi.effect.CHR_DOWN },
    }

    for _, entry in ipairs(mainDowns) do
        local name, mod, effectID = entry[1], entry[2], entry[3]
        it(name .. ' clamps to current stat and deletes at power 0', function()
            local script = require('scripts/effects/' .. name)
            local target = stubTarget({ stats = { [mod] = 8 } })
            local effect = stubEffect(20, 0)
            script.onEffectGain(target, effect)
            assert(effect.power == 8)
            assert(target.mods[mod] == -8)
            effect.power = 0
            script.onEffectTick(target, effect)
            assert(target.deletedEffects[1] == effectID)
        end)
    end

    it('evasion boost applies EVA power', function()
        local script = require('scripts/effects/evasion_boost')
        local target = stubTarget()
        local effect = stubEffect(25, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.EVA] == 25)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.EVA] == 0)
    end)

    it('evasion down clamps to current EVA and restores min(power,10) per tick', function()
        local script = require('scripts/effects/evasion_down')
        local target = stubTarget({ stats = { [xi.mod.EVA] = 40 } })
        local effect = stubEffect(50, 0)
        script.onEffectGain(target, effect)
        assert(effect.power == 40)
        -- delMod(EVA, 40) → mods[EVA] = -40
        assert(target.mods[xi.mod.EVA] == -40)
        script.onEffectTick(target, effect)
        assert(effect.power == 30)
        assert(target.mods[xi.mod.EVA] == -30) -- addMod 10
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.EVA] == 0)
    end)

    it('magic atk boost normalizes power over 100 to 50', function()
        local script = require('scripts/effects/magic_atk_boost')
        local target = stubTarget()
        local effect = stubEffect(120, 0)
        script.onEffectGain(target, effect)
        assert(effect.power == 50 and target.mods[xi.mod.MATT] == 50)
    end)

    it('magic def boost ticks down MDEF by 1', function()
        local script = require('scripts/effects/magic_def_boost')
        local target = stubTarget()
        local effect = stubEffect(5, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.MDEF] == 5)
        script.onEffectTick(target, effect)
        assert(effect.power == 4 and target.mods[xi.mod.MDEF] == 4)
    end)

    it('magic evasion boost applies MEVA', function()
        local script = require('scripts/effects/magic_evasion_boost')
        local target = stubTarget()
        local effect = stubEffect(12, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.MEVA] == 12)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.MEVA] == 0)
    end)

    it('magic atk/def/acc down use effect:addMod (empty lose)', function()
        local matt = require('scripts/effects/magic_atk_down')
        local effect = stubEffect(20, 0)
        matt.onEffectGain(stubTarget(), effect)
        assert(effect._mod.mod == xi.mod.MATT and effect._mod.delta == -20)

        local mdef = require('scripts/effects/magic_def_down')
        effect = stubEffect(15, 0)
        mdef.onEffectGain(stubTarget(), effect)
        assert(effect._mod.mod == xi.mod.MDEF and effect._mod.delta == -15)

        local macc = require('scripts/effects/magic_acc_down')
        effect = stubEffect(10, 0)
        macc.onEffectGain(stubTarget(), effect)
        assert(effect._mod.mod == xi.mod.MACC and effect._mod.delta == -10)
    end)

    it('magic evasion down uses delMod/addMod pair', function()
        local script = require('scripts/effects/magic_evasion_down')
        local target = stubTarget()
        local effect = stubEffect(8, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.MEVA] == -8)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.MEVA] == 0)
    end)
end)
