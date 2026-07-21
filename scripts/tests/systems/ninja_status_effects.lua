-----------------------------------
-- Pure system tests for Ninja status-effect scripts
-- (yonin, innin, mikage, sange, empty flags).
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        merits = opts.merits or {},
        jp = opts.jp or {},
        getMerit = function(self, merit)
            return self.merits[merit] or 0
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
        setPower = function(self, v)
            self.power = v
        end,
        setSubPower = function(self, v)
            self.subPower = v
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Ninja status-effect pure plans', function()
    it('yonin applies ACC/tool/enmity and decays each tick', function()
        local script = require('scripts/effects/yonin')
        local target = stubTarget({ merits = { [xi.merit.YONIN_EFFECT] = 50 } })
        local effect = stubEffect(30)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ACC] == -30)
        assert(effect.mods[xi.mod.NINJA_TOOL] == 30)
        assert(effect.mods[xi.mod.ENMITY] == 30)
        assert(effect.mods[xi.mod.YONIN_UTSUSEMI_ENMITY] == 1)
        assert(effect.mods[xi.mod.HP] == 50)
        script.onEffectTick(target, effect)
        assert(effect.power == 29)
        assert(effect.mods[xi.mod.ACC] == -29)
        assert(effect.mods[xi.mod.NINJA_TOOL] == 29)
        assert(effect.mods[xi.mod.ENMITY] == 29)
    end)

    it('yonin without merits skips HP mod', function()
        local script = require('scripts/effects/yonin')
        local effect = stubEffect(30)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.HP] == nil)
    end)

    it('innin applies nuke/eva/enmity and decays with slower enmity', function()
        local script = require('scripts/effects/innin')
        local effect = stubEffect(30, 20)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.NIN_NUKE_BONUS_INNIN] == 30)
        assert(effect.mods[xi.mod.EVA] == -30)
        assert(effect.mods[xi.mod.ENMITY] == -20)
        -- 30→29 odd: no enmity
        script.onEffectTick(stubTarget(), effect)
        assert(effect.power == 29)
        assert(effect.subPower == 20)
        assert(effect.mods[xi.mod.NIN_NUKE_BONUS_INNIN] == 29)
        assert(effect.mods[xi.mod.EVA] == -29)
        assert(effect.mods[xi.mod.ENMITY] == -20)
        -- 29→28 even: enmity restores 1
        script.onEffectTick(stubTarget(), effect)
        assert(effect.power == 28)
        assert(effect.subPower == 19)
        assert(effect.mods[xi.mod.ENMITY] == -19)
        -- decay to floor 10
        while effect.power > 10 do
            script.onEffectTick(stubTarget(), effect)
        end
        assert(effect.power == 10)
        local nuke = effect.mods[xi.mod.NIN_NUKE_BONUS_INNIN]
        script.onEffectTick(stubTarget(), effect)
        assert(effect.power == 10)
        assert(effect.mods[xi.mod.NIN_NUKE_BONUS_INNIN] == nuke)
    end)

    it('mikage applies ATT equal to JP times 3', function()
        local script = require('scripts/effects/mikage')
        local target = stubTarget({ jp = { [xi.jp.MIKAGE_EFFECT] = 4 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ATT] == 12)
    end)

    it('sange applies 100 daken and RACC from merit', function()
        local script = require('scripts/effects/sange')
        local target = stubTarget({ merits = { [xi.merit.SANGE] = 25 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.DAKEN] == 100)
        assert(effect.mods[xi.mod.RACC] == 25)
    end)

    it('futae issekigan migawari have empty bodies', function()
        for _, name in ipairs({ 'futae', 'issekigan', 'migawari' }) do
            local script = require('scripts/effects/' .. name)
            local effect = stubEffect(1)
            script.onEffectGain(stubTarget(), effect)
            script.onEffectTick(stubTarget(), effect)
            script.onEffectLose(stubTarget(), effect)
            local count = 0
            for _ in pairs(effect.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
