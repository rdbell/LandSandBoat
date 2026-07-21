-----------------------------------
-- Pure system tests for combat ability status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        objType = opts.objType or xi.objType.PC,
        att = opts.att or 0,
        acc = opts.acc or 0,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getObjType = function(self)
            return self.objType
        end,
        getStat = function(self, mod)
            if mod == xi.mod.ATT then
                return self.att
            end
            return 0
        end,
        getACC = function(self)
            return self.acc
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
        tier = opts.tier or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        getTier = function(self)
            return self.tier
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

describe('Combat ability status-effect pure plans', function()
    it('inhibit tp and subtle blow plus apply power mods', function()
        local inh = require('scripts/effects/inhibit_tp')
        local target = stubTarget()
        inh.onEffectGain(target, stubEffect({ power = 25 }))
        assert(target.mods[xi.mod.INHIBIT_TP] == 25)

        local sb = require('scripts/effects/subtle_blow_plus')
        target = stubTarget()
        sb.onEffectGain(target, stubEffect({ power = 10 }))
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 10)
    end)

    it('multi strikes uses double or triple attack by tier', function()
        local script = require('scripts/effects/multi_strikes')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 40, tier = 1 }))
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 40)
        assert(target.mods[xi.mod.TRIPLE_ATTACK] == nil)

        target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 15, tier = 2 }))
        assert(target.mods[xi.mod.TRIPLE_ATTACK] == 15)
    end)

    it('overload applies only to pets; overkill applies multi-shot mods', function()
        local ovl = require('scripts/effects/overload')
        local effect = stubEffect()
        ovl.onEffectGain(stubTarget({ objType = xi.objType.PC }), effect)
        assert(effect.mods[xi.mod.HASTE_MAGIC] == nil)

        effect = stubEffect()
        ovl.onEffectGain(stubTarget({ objType = xi.objType.PET }), effect)
        assert(effect.mods[xi.mod.HASTE_MAGIC] == -5000)
        assert(effect.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 50)

        local ovk = require('scripts/effects/overkill')
        local target = stubTarget()
        ovk.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.DOUBLE_SHOT_RATE] == 100)
        assert(target.mods[xi.mod.TRIPLE_ATTACK] == 33)
    end)

    it('potency and subtle sorcery apply crit and macc mods', function()
        local pot = require('scripts/effects/potency')
        local target = stubTarget()
        pot.onEffectGain(target, stubEffect({ subPower = 12 }))
        assert(target.mods[xi.mod.CRITHITRATE] == 12)

        local ss = require('scripts/effects/subtle_sorcery')
        target = stubTarget({ jp = { [xi.jp.SUBTLE_SORCERY_EFFECT] = 5 } })
        ss.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.MACC] == 100)
        assert(target.mods[xi.mod.UFASTCAST] == 5)
    end)

    it('sepulcher and hamanoha apply accuracy/evasion catalogs', function()
        local sep = require('scripts/effects/sepulcher')
        local target = stubTarget()
        sep.onEffectGain(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.ACC] == -15)
        assert(target.mods[xi.mod.STORETP] == -15)
        sep.onEffectLose(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.ACC] == 0)

        local ham = require('scripts/effects/hamanoha')
        target = stubTarget()
        ham.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.ACC] == 20)
        assert(target.mods[xi.mod.REGAIN_DOWN] == 20)
    end)

    it('inner strength unleash and intervene apply fixed or captured mods', function()
        local inn = require('scripts/effects/inner_strength')
        local target = stubTarget()
        inn.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.HPP] == 100)
        assert(target.mods[xi.mod.PERFECT_COUNTER_ATT] == 100)

        local unl = require('scripts/effects/unleash')
        target = stubTarget()
        unl.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.TAME] == 98)

        local intv = require('scripts/effects/intervene')
        target = stubTarget({ att = 200, acc = 150 })
        local effect = stubEffect()
        intv.onEffectGain(target, effect)
        assert(effect.power == 200)
        assert(effect.subPower == 150)
        assert(target.mods[xi.mod.ATT] == -200)
        assert(target.mods[xi.mod.ACC] == -150)
        intv.onEffectLose(target, effect)
        assert(target.mods[xi.mod.ATT] == 0)
        assert(target.mods[xi.mod.ACC] == 0)
    end)
end)
