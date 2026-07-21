-----------------------------------
-- Pure system tests for weakness, max HP/MP, daze, imagery, and
-- sigil/sanction food-duration branches.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        latents = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setMod = function(self, mod, value)
            self.mods[mod] = value
        end,
        addLatent = function(self, latent, req, mod, value)
            table.insert(self.latents, { latent = latent, req = req, mod = mod, value = value, op = 'add' })
        end,
        delLatent = function(self, latent, req, mod, value)
            table.insert(self.latents, { latent = latent, req = req, mod = mod, value = value, op = 'del' })
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

describe('Misc status-effect pure plans', function()
    it('weakness applies PCT slow and optional double-weakness mods', function()
        local script = require('scripts/effects/weakness')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 1 }))
        assert(target.mods[xi.mod.WEAKNESS_PCT] == -75)
        assert(target.mods[xi.mod.HASTE_MAGIC] == -10000)
        assert(target.mods[xi.mod.RACC] == nil)

        target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 2 }))
        assert(target.mods[xi.mod.RACC] == -999)
        assert(target.mods[xi.mod.MATT] == -999)
    end)

    it('max hp/mp boost and down apply percent and flat mods', function()
        local hp = require('scripts/effects/max_hp_boost')
        local effect = stubEffect({ power = 20, subPower = 100 })
        hp.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.HPP] == 20)
        assert(effect.mods[xi.mod.HP] == 100)

        local mp = require('scripts/effects/max_mp_boost')
        local target = stubTarget()
        mp.onEffectGain(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.MPP] == 15)

        local hpd = require('scripts/effects/max_hp_down')
        target = stubTarget()
        hpd.onEffectGain(target, stubEffect({ power = 25 }))
        assert(target.mods[xi.mod.HPP] == -25)

        local mpd = require('scripts/effects/max_mp_down')
        target = stubTarget()
        mpd.onEffectGain(target, stubEffect({ power = 10 }))
        assert(target.mods[xi.mod.MPP] == -10)
    end)

    it('daze formulas match power scaling', function()
        local leth = require('scripts/effects/lethargic_daze_1')
        local target = stubTarget()
        leth.onEffectGain(target, stubEffect({ power = 4 }))
        assert(target.mods[xi.mod.EVA] == -20)

        local slug = require('scripts/effects/sluggish_daze_1')
        target = stubTarget()
        slug.onEffectGain(target, stubEffect({ power = 5 }))
        assert(target.mods[xi.mod.DEFP] == -13)

        local weak = require('scripts/effects/weakened_daze_1')
        target = stubTarget()
        weak.onEffectGain(target, stubEffect({ power = 3 }))
        assert(target.mods[xi.mod.MEVA] == -9)

        local bew = require('scripts/effects/bewildered_daze_1')
        local effect = stubEffect({ power = 5 })
        bew.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.CRITICAL_HIT_EVASION] == -5)
    end)

    it('craft imagery applies skill and material-loss mods', function()
        local alch = require('scripts/effects/alchemy_imagery')
        local effect = stubEffect({ power = 3, subPower = 5 })
        alch.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ALCHEMY] == 3)
        assert(effect.mods[xi.mod.SYNTH_MATERIAL_LOSS_ALCHEMY] == 5)

        local fish = require('scripts/effects/fishing_imagery')
        local target = stubTarget()
        fish.onEffectGain(target, stubEffect({ power = 10 }))
        assert(target.mods[xi.mod.FISH] == 10)
    end)

    it('sigil and sanction food-duration and latent branches', function()
        local sigil = require('scripts/effects/sigil')
        local target = stubTarget()
        -- bits 1 and 3: regen latent + food duration (getBit uses 1-based bit index)
        -- utils.mask.getBit(power, 1) checks bit position 1
        local power = bit.lshift(1, 1) + bit.lshift(1, 3) -- bits 1 and 3
        sigil.onEffectGain(target, stubEffect({ power = power }))
        assert(target.mods[xi.mod.FOOD_DURATION] == 100)
        assert(#target.latents >= 1)

        local sanction = require('scripts/effects/sanction')
        target = stubTarget()
        sanction.onEffectGain(target, stubEffect({ power = 3 }))
        assert(target.mods[xi.mod.FOOD_DURATION] == 100)

        target = stubTarget()
        sanction.onEffectGain(target, stubEffect({ power = 1 }))
        assert(target.latents[1].mod == xi.mod.REGEN)
    end)

    it('str boost ii applies power without tick decay body', function()
        local script = require('scripts/effects/str_boost_ii')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 12 }))
        assert(target.mods[xi.mod.STR] == 12)
        script.onEffectLose(target, stubEffect({ power = 12 }))
        assert(target.mods[xi.mod.STR] == 0)
    end)
end)
