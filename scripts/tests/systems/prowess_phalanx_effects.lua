-----------------------------------
-- Pure system tests for Prowess, Phalanx, Damage Spikes, and Dread Spikes.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = opts.mods or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
    }
end

local function stubEffect(power)
    return {
        power = power or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Prowess Phalanx spikes status-effect pure plans', function()
    it('prowess variants apply their mod catalogs', function()
        local acc = require('scripts/effects/prowess_acc_racc')
        local target = stubTarget()
        acc.onEffectGain(target, stubEffect(15))
        assert(target.mods[xi.mod.ACC] == 15)
        assert(target.mods[xi.mod.RACC] == 15)

        local att = require('scripts/effects/prowess_att_ratt')
        target = stubTarget()
        att.onEffectGain(target, stubEffect(10))
        assert(target.mods[xi.mod.ATTP] == 10)
        assert(target.mods[xi.mod.RATTP] == 10)

        local hp = require('scripts/effects/prowess_hp_mp')
        target = stubTarget()
        hp.onEffectGain(target, stubEffect(8))
        assert(target.mods[xi.mod.HPP] == 8)
        assert(target.mods[xi.mod.MPP] == 8)

        local killer = require('scripts/effects/prowess_killer')
        target = stubTarget()
        killer.onEffectGain(target, stubEffect(4))
        assert(target.mods[xi.mod.VERMIN_KILLER] == 4)
        assert(target.mods[xi.mod.LUMINION_KILLER] == 4)
        assert(target.mods[xi.mod.EMPTY_KILLER] == 4)

        local gov = require('scripts/effects/prowess')
        target = stubTarget()
        gov.onEffectGain(target, stubEffect(3))
        assert(target.mods[xi.mod.GOV_CLEARS] == 3)
    end)

    it('phalanx applies power plus PHALANX_RECEIVED on effect', function()
        local script = require('scripts/effects/phalanx')
        local target = stubTarget({ mods = { [xi.mod.PHALANX_RECEIVED] = 5 } })
        local effect = stubEffect(20)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.PHALANX] == 25)
    end)

    it('damage spikes match blaze type; dread spikes type 3 only', function()
        local dmg = require('scripts/effects/damage_spikes')
        local target = stubTarget()
        dmg.onEffectGain(target, stubEffect(30))
        assert(target.mods[xi.mod.SPIKES] == 1)
        assert(target.mods[xi.mod.SPIKES_DMG] == 30)

        local dread = require('scripts/effects/dread_spikes')
        target = stubTarget()
        dread.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.SPIKES] == 3)
        assert(target.mods[xi.mod.SPIKES_DMG] == nil)
    end)

    it('prowess cure potency th and attack speed', function()
        local cure = require('scripts/effects/prowess_cure_potency')
        local target = stubTarget()
        cure.onEffectGain(target, stubEffect(5))
        assert(target.mods[xi.mod.CURE_POTENCY] == 5)

        local th = require('scripts/effects/prowess_th')
        target = stubTarget()
        th.onEffectGain(target, stubEffect(2))
        assert(target.mods[xi.mod.TREASURE_HUNTER] == 2)

        local haste = require('scripts/effects/prowess_attack_speed')
        target = stubTarget()
        haste.onEffectGain(target, stubEffect(500))
        assert(target.mods[xi.mod.HASTE_ABILITY] == 500)

        local mag = require('scripts/effects/prowess_macc_matk')
        target = stubTarget()
        mag.onEffectGain(target, stubEffect(12))
        assert(target.mods[xi.mod.MATT] == 12)
        assert(target.mods[xi.mod.MACC] == 12)
    end)
end)
