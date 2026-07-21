-----------------------------------
-- Pure system tests for elemental enfeeble DoTs and Warrior-family job effects.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        deletedEffects = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
    }
end

local function stubEffect(power, subPower)
    return {
        power = power,
        subPower = subPower or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Elemental DoT and Warrior job status-effect pure plans', function()
    local elementals = {
        { 'burn', xi.mod.INT, xi.effect.FROST },
        { 'frost', xi.mod.AGI, xi.effect.CHOKE },
        { 'choke', xi.mod.VIT, xi.effect.RASP },
        { 'rasp', xi.mod.DEX, xi.effect.SHOCK },
        { 'shock', xi.mod.MND, xi.effect.DROWN },
        { 'drown', xi.mod.STR, xi.effect.BURN },
    }

    for _, entry in ipairs(elementals) do
        local name, statMod, clear = entry[1], entry[2], entry[3]
        it(name .. ' applies REGEN_DOWN and -stat, clears cycle partner', function()
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            local effect = stubEffect(3, 0)
            script.onEffectGain(target, effect)
            -- reduction = (3-1)*2+5 = 9
            assert(effect.mods[xi.mod.REGEN_DOWN] == 3)
            assert(effect.mods[statMod] == -9)
            assert(target.deletedEffects[1] == clear)
        end)
    end

    it('berserk applies ATTP/RATTP power, DEFP -25, JP ATT/RATT', function()
        local script = require('scripts/effects/berserk')
        local target = stubTarget({ jp = { [xi.jp.BERSERK_EFFECT] = 3 } })
        local effect = stubEffect(35, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ATTP] == 35 and effect.mods[xi.mod.RATTP] == 35)
        assert(effect.mods[xi.mod.DEFP] == -25)
        assert(effect.mods[xi.mod.ATT] == 6 and effect.mods[xi.mod.RATT] == 6)
    end)

    it('defender applies DEFP power, ATTP/RATTP -25, JP DEF', function()
        local script = require('scripts/effects/defender')
        local target = stubTarget({ jp = { [xi.jp.DEFENDER_EFFECT] = 2 } })
        local effect = stubEffect(30, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.DEFP] == 30)
        assert(effect.mods[xi.mod.ATTP] == -25 and effect.mods[xi.mod.RATTP] == -25)
        assert(effect.mods[xi.mod.DEF] == 6)
    end)

    it('aggressor applies RACC power+JP, ACC 25+JP, EVA -25', function()
        local script = require('scripts/effects/aggressor')
        local target = stubTarget({ jp = { [xi.jp.AGGRESSOR_EFFECT] = 2 } })
        local effect = stubEffect(10, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.RACC] == 12)
        assert(effect.mods[xi.mod.ACC] == 27)
        assert(effect.mods[xi.mod.EVA] == -25)
    end)

    it('warcry applies ATTP/RATTP, TP_BONUS subPower, JP ATT/RATT', function()
        local script = require('scripts/effects/warcry')
        local target = stubTarget({ jp = { [xi.jp.WARCRY_EFFECT] = 4 } })
        local effect = stubEffect(20, 15)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ATTP] == 20 and effect.mods[xi.mod.RATTP] == 20)
        assert(effect.mods[xi.mod.TP_BONUS] == 15)
        assert(effect.mods[xi.mod.ATT] == 12 and effect.mods[xi.mod.RATT] == 12)
    end)

    it('mighty strikes applies crit rate and JP ACC on gain/lose', function()
        local script = require('scripts/effects/mighty_strikes')
        local target = stubTarget({ jp = { [xi.jp.MIGHTY_STRIKES_EFFECT] = 5 } })
        local effect = stubEffect(1, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.CRITHITRATE] == 100)
        assert(target.mods[xi.mod.ACC] == 10 and target.mods[xi.mod.RACC] == 10)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.CRITHITRATE] == 0)
        assert(target.mods[xi.mod.ACC] == 0 and target.mods[xi.mod.RACC] == 0)
    end)

    it('invincible applies and removes UDMGPHYS/UDMGRANGE -10000', function()
        local script = require('scripts/effects/invincible')
        local target = stubTarget()
        local effect = stubEffect(1, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.UDMGPHYS] == -10000)
        assert(target.mods[xi.mod.UDMGRANGE] == -10000)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.UDMGPHYS] == 0)
        assert(target.mods[xi.mod.UDMGRANGE] == 0)
    end)
end)
