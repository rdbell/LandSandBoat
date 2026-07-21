-----------------------------------
-- Pure system tests for per-avatar favor status-effect mod products.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
    }
end

local function stubEffect(power)
    return {
        power = power or 0,
        getPower = function(self)
            return self.power
        end,
    }
end

local function runFavor(name, mod, power)
    local script = require('scripts/effects/' .. name)
    local target = stubTarget()
    script.onEffectGain(target, stubEffect(power))
    assert(target.mods[mod] == power, name .. ' gain')
    script.onEffectLose(target, stubEffect(power))
    assert(target.mods[mod] == 0, name .. ' lose')
end

describe('Avatar favor status-effect pure plans', function()
    it('each avatar favor applies its catalog mod equal to power', function()
        runFavor('carbuncles_favor', xi.mod.REGEN, 12)
        runFavor('fenrirs_favor', xi.mod.MEVA, 9)
        runFavor('ifrits_favor', xi.mod.DOUBLE_ATTACK, 23)
        runFavor('titans_favor', xi.mod.DEF, 77)
        runFavor('leviathans_favor', xi.mod.MACC, 12)
        runFavor('garudas_favor', xi.mod.EVA, 28)
        runFavor('shivas_favor', xi.mod.MATT, 30)
        runFavor('ramuhs_favor', xi.mod.CRITHITRATE, 21)
        runFavor('diaboloss_favor', xi.mod.REFRESH, 5)
        runFavor('cait_siths_favor', xi.mod.MDEF, 20)
        runFavor('sirens_favor', xi.mod.SUBTLE_BLOW, 19)
    end)
end)
