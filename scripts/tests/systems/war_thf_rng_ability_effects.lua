-----------------------------------
-- Pure system tests for WAR/THF/RNG ability status-effect scripts:
-- Blood Rage, Brazen Rush, Camouflage, Conspirator.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(_, jp)
            return (opts.jp and opts.jp[jp]) or 0
        end,
    }
end

local function stubEffect(opts)
    opts = opts or {}
    return {
        power = opts.power or 0,
        subPower = opts.subPower or 0,
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
    }
end

describe('WAR/THF/RNG ability status-effect pure plans', function()
    it('blood rage applies CRITHITRATE power on target', function()
        local script = require('scripts/effects/blood_rage')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 25 }))
        assert(target.mods[xi.mod.CRITHITRATE] == 25)
        script.onEffectLose(target, stubEffect({ power = 25 }))
        assert(target.mods[xi.mod.CRITHITRATE] == 0)
    end)

    it('brazen rush gain applies ATT 4*JP and DOUBLE_ATTACK power', function()
        local script = require('scripts/effects/brazen_rush')
        local target = stubTarget({ jp = { [xi.jp.BRAZEN_RUSH_EFFECT] = 3 } })
        script.onEffectGain(target, stubEffect({ power = 40 }))
        assert(target.mods[xi.mod.ATT] == 12)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 40)
    end)

    it('brazen rush tick decays DOUBLE_ATTACK by 10', function()
        local script = require('scripts/effects/brazen_rush')
        local target = stubTarget({ jp = { [xi.jp.BRAZEN_RUSH_EFFECT] = 2 } })
        local effect = stubEffect({ power = 40 })
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.ATT] == 8)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 40)

        script.onEffectTick(target, effect)
        assert(effect.power == 30)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 30)
        assert(target.mods[xi.mod.ATT] == 8) -- ATT unchanged on tick

        script.onEffectTick(target, effect)
        assert(effect.power == 20)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 20)
    end)

    it('brazen rush lose removes ATT 4*JP and current DOUBLE_ATTACK', function()
        local script = require('scripts/effects/brazen_rush')
        local target = stubTarget({ jp = { [xi.jp.BRAZEN_RUSH_EFFECT] = 5 } })
        local effect = stubEffect({ power = 40 })
        script.onEffectGain(target, effect)
        script.onEffectTick(target, effect) -- power 30
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.ATT] == 0)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 0)
        assert(effect.power == 30)
    end)

    it('brazen rush with zero JP only applies DOUBLE_ATTACK', function()
        local script = require('scripts/effects/brazen_rush')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 50 }))
        assert((target.mods[xi.mod.ATT] or 0) == 0)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 50)
        script.onEffectLose(target, stubEffect({ power = 50 }))
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 0)
    end)

    it('camouflage is effect-owned ENMITY/CRITHITRATE/RETAIN_CAMOUFLAGE', function()
        local script = require('scripts/effects/camouflage')
        local effect = stubEffect()
        local target = stubTarget({ jp = { [xi.jp.CAMOUFLAGE_EFFECT] = 4 } })
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ENMITY] == -25)
        assert(effect.mods[xi.mod.CRITHITRATE] == 4)
        assert(effect.mods[xi.mod.RETAIN_CAMOUFLAGE] == 1)
        -- lose is empty; effect-owned mods clear with the effect
        script.onEffectLose(target, effect)
        assert(effect.mods[xi.mod.ENMITY] == -25)
    end)

    it('camouflage zero JP still sets ENMITY and RETAIN', function()
        local script = require('scripts/effects/camouflage')
        local effect = stubEffect()
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ENMITY] == -25)
        assert((effect.mods[xi.mod.CRITHITRATE] or 0) == 0)
        assert(effect.mods[xi.mod.RETAIN_CAMOUFLAGE] == 1)
    end)

    it('conspirator applies SUBTLE_BLOW power and ACC subPower+JP', function()
        local script = require('scripts/effects/conspirator')
        local target = stubTarget({ jp = { [xi.jp.CONSPIRATOR_EFFECT] = 3 } })
        script.onEffectGain(target, stubEffect({ power = 20, subPower = 15 }))
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 20)
        assert(target.mods[xi.mod.ACC] == 18) -- 15 + 3
        script.onEffectLose(target, stubEffect({ power = 20, subPower = 15 }))
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 0)
        assert(target.mods[xi.mod.ACC] == 0)
    end)

    it('conspirator with zero JP uses subPower alone for ACC', function()
        local script = require('scripts/effects/conspirator')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 10, subPower = 25 }))
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 10)
        assert(target.mods[xi.mod.ACC] == 25)
    end)
end)
