-----------------------------------
-- Pure system tests for remaining status-effect mod scripts:
-- Bane, Dematerialize, Kaustra, Guarding Rate Boost, Ninjutsu Ele Debuff,
-- Curing Conduit, Illusion.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        costume = 0,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setCostume = function(self, id)
            self.costume = id
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
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Remaining status-effect mod pure plans', function()
    it('bane gain matches curse mods; lose has weight quirk', function()
        local script = require('scripts/effects/bane')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 75 }))
        assert(target.mods[xi.mod.CURSE_PCT] == -75)
        assert(target.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 75)

        -- Lose: delMod CURSE_PCT -75 restores to 0; addMod weight +75 doubles penalty.
        script.onEffectLose(target, stubEffect({ power = 75 }))
        assert(target.mods[xi.mod.CURSE_PCT] == 0)
        assert(target.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 150)
    end)

    it('dematerialize applies full UDMG immunity band', function()
        local script = require('scripts/effects/dematerialize')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.UDMGPHYS] == -10000)
        assert(target.mods[xi.mod.UDMGMAGIC] == -10000)
        assert(target.mods[xi.mod.UDMGBREATH] == -10000)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.UDMGPHYS] == 0)
        assert(target.mods[xi.mod.UDMGMAGIC] == 0)
        assert(target.mods[xi.mod.UDMGBREATH] == 0)
    end)

    it('kaustra applies REGEN_DOWN power', function()
        local script = require('scripts/effects/kaustra')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 12 }))
        assert(target.mods[xi.mod.REGEN_DOWN] == 12)
        script.onEffectLose(target, stubEffect({ power = 12 }))
        assert(target.mods[xi.mod.REGEN_DOWN] == 0)
    end)

    it('guarding rate boost is effect-owned ADDITIVE_GUARD', function()
        local script = require('scripts/effects/guarding_rate_boost')
        local effect = stubEffect({ power = 10 })
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ADDITIVE_GUARD] == 10)
    end)

    it('ninjutsu ele debuff uses subPower as mod ID', function()
        local script = require('scripts/effects/ninjutsu_ele_debuff')
        local target = stubTarget()
        local fireMod = xi.mod.FIRE_MEVA
        script.onEffectGain(target, stubEffect({ power = 30, subPower = fireMod }))
        assert(target.mods[fireMod] == -30)
        script.onEffectLose(target, stubEffect({ power = 30, subPower = fireMod }))
        assert(target.mods[fireMod] == 0)
    end)

    it('curing conduit applies CURE_POTENCY_RCVD', function()
        local script = require('scripts/effects/curing_conduit')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.CURE_POTENCY_RCVD] == 15)
        script.onEffectLose(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.CURE_POTENCY_RCVD] == 0)
    end)

    it('illusion sets costume and effect-owned REGEN_DOWN', function()
        local script = require('scripts/effects/illusion')
        local target = stubTarget()
        local effect = stubEffect({ power = 1234, subPower = 5 })
        script.onEffectGain(target, effect)
        assert(target.costume == 1234)
        assert(effect.mods[xi.mod.REGEN_DOWN] == 5)
        script.onEffectLose(target, effect)
        assert(target.costume == 0)
    end)
end)
