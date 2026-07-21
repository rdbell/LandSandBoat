-----------------------------------
-- Pure system tests for Samurai ability status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
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
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Samurai status-effect pure plans', function()
    it('meditate applies REGAIN equal to power times 10', function()
        local script = require('scripts/effects/meditate')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect(12))
        assert(target.mods[xi.mod.REGAIN] == 120)
        script.onEffectLose(target, stubEffect(12))
        assert(target.mods[xi.mod.REGAIN] == 0)
    end)

    it('meikyo shisui applies SKILLCHAINDMG equal to 200 times JP', function()
        local script = require('scripts/effects/meikyo_shisui')
        local target = stubTarget({ jp = { [xi.jp.MEIKYO_SHISUI_EFFECT] = 3 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.SKILLCHAINDMG] == 600)
    end)

    it('hagakure applies SAVETP and TP_BONUS with JP', function()
        local script = require('scripts/effects/hagakure')
        local target = stubTarget({ jp = { [xi.jp.HAGAKURE_EFFECT] = 4 } })
        local effect = stubEffect(1000, 500)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.SAVETP] == 1000)
        assert(effect.mods[xi.mod.TP_BONUS] == 540)
    end)

    it('sekkanoki yaegasumi and sengikori are empty', function()
        for _, name in ipairs({ 'sekkanoki', 'yaegasumi', 'sengikori' }) do
            local script = require('scripts/effects/' .. name)
            local effect = stubEffect(1)
            script.onEffectGain(stubTarget(), effect)
            local count = 0
            for _ in pairs(effect.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
