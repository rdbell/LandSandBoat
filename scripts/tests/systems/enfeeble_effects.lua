-----------------------------------
-- Pure system tests for enfeeble status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        baseSpeed = opts.baseSpeed or 40,
        tp = opts.tp or 1000,
        uncharmed = false,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setMod = function(self, mod, value)
            self.mods[mod] = value
        end,
        getBaseSpeed = function(self)
            return self.baseSpeed
        end,
        setBaseSpeed = function(self, v)
            self.baseSpeed = v
        end,
        setTP = function(self, v)
            self.tp = v
        end,
        uncharm = function(self)
            self.uncharmed = true
        end,
        setHP = function(self, v)
            self.hp = v
        end,
        messagePublic = function() end,
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
        setPower = function(self, v)
            self.power = v
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        getTimeRemaining = function(self)
            return self.timeRemaining or 0
        end,
    }
end

describe('Enfeeble status-effect pure plans', function()
    it('paralysis adds PARALYZE and resets immunobreak', function()
        local script = require('scripts/effects/paralysis')
        local target = stubTarget()
        local effect = stubEffect(25, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.PARALYZE] == 25)
        assert(target.mods[xi.mod.PARALYZE_IMMUNOBREAK] == 0)
    end)

    it('silence sleep petrification only reset immunobreak', function()
        local silence = require('scripts/effects/silence')
        local target = stubTarget()
        silence.onEffectGain(target, stubEffect(0, 0))
        assert(target.mods[xi.mod.SILENCE_IMMUNOBREAK] == 0)

        local sleep = require('scripts/effects/sleep')
        target = stubTarget()
        sleep.onEffectGain(target, stubEffect(0, 0))
        assert(target.mods[xi.mod.SLEEP_IMMUNOBREAK] == 0)

        local petrify = require('scripts/effects/petrification')
        target = stubTarget()
        petrify.onEffectGain(target, stubEffect(0, 0))
        assert(target.mods[xi.mod.PETRIFY_IMMUNOBREAK] == 0)
    end)

    it('blindness subtracts ACC/RACC and restores on lose', function()
        local script = require('scripts/effects/blindness')
        local target = stubTarget()
        local effect = stubEffect(15, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.ACC] == -15 and target.mods[xi.mod.RACC] == -15)
        assert(target.mods[xi.mod.BLIND_IMMUNOBREAK] == 0)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.ACC] == 0 and target.mods[xi.mod.RACC] == 0)
    end)

    it('bind stores base speed and zeros it', function()
        local script = require('scripts/effects/bind')
        local target = stubTarget({ baseSpeed = 50 })
        local effect = stubEffect(0, 0)
        script.onEffectGain(target, effect)
        assert(effect.power == 50)
        assert(target.baseSpeed == 0)
        assert(target.mods[xi.mod.BIND_IMMUNOBREAK] == 0)
        script.onEffectLose(target, effect)
        assert(target.baseSpeed == 50)
    end)

    it('weight applies MOVE_SPEED_WEIGHT_PENALTY and gravity immunobreak', function()
        local script = require('scripts/effects/weight')
        local target = stubTarget()
        local effect = stubEffect(50, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 50)
        assert(target.mods[xi.mod.GRAVITY_IMMUNOBREAK] == 0)
    end)

    it('plague applies ceil(power/2) REFRESH_DOWN and power*10 REGAIN_DOWN', function()
        local script = require('scripts/effects/plague')
        local target = stubTarget()
        local effect = stubEffect(5, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.REFRESH_DOWN] == 3)
        assert(target.mods[xi.mod.REGAIN_DOWN] == 50)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.REFRESH_DOWN] == 0)
        assert(target.mods[xi.mod.REGAIN_DOWN] == 0)
    end)

    it('curse applies CURSE_PCT and weight penalty', function()
        local script = require('scripts/effects/curse')
        local target = stubTarget()
        local effect = stubEffect(75, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.CURSE_PCT] == -75)
        assert(target.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 75)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.CURSE_PCT] == 0)
        assert(target.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 0)
    end)

    it('addle applies -MACC and -FASTCAST with immunobreak', function()
        local script = require('scripts/effects/addle')
        local target = stubTarget()
        local effect = stubEffect(30, 20)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.MACC] == -30)
        assert(effect.mods[xi.mod.FASTCAST] == -20)
        assert(target.mods[xi.mod.ADDLE_IMMUNOBREAK] == 0)
    end)

    it('doom tick remaining formula and lose kills at zero remaining', function()
        -- pure formula pin
        local remainingTicks = math.floor(9000 / 1000 - 0.5) / 3
        assert(remainingTicks == 8 / 3)
        local script = require('scripts/effects/doom')
        local target = stubTarget()
        local effect = stubEffect(0, 0)
        effect.timeRemaining = 0
        script.onEffectLose(target, effect)
        assert(target.hp == 0)
        target = stubTarget()
        effect.timeRemaining = 1000
        script.onEffectLose(target, effect)
        assert(target.hp == nil)
    end)

    it('charm optional REGEN_DOWN and lose clears TP and uncharms', function()
        local script = require('scripts/effects/charm')
        local target = stubTarget()
        local effect = stubEffect(0, 5)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.REGEN_DOWN] == 5)
        script.onEffectLose(target, effect)
        assert(target.tp == 0 and target.uncharmed == true)
    end)

    it('disease stun amnesia terror have empty gain/lose bodies', function()
        for _, name in ipairs({ 'disease', 'stun', 'amnesia', 'terror' }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            local effect = stubEffect(1, 0)
            script.onEffectGain(target, effect)
            script.onEffectLose(target, effect)
            assert(next(target.mods) == nil)
        end
    end)
end)
