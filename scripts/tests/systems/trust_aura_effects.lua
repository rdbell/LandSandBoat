-----------------------------------
-- Pure system tests for Trust aura status-effect scripts.
-----------------------------------

local function stubEffect(opts)
    opts = opts or {}
    return {
        power = opts.power or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Trust aura status-effect pure plans', function()
    it('acc aura applies DEX and ACC/RACC via auraValue', function()
        local script = require('scripts/effects/trust_aura_acc')
        local effect = stubEffect({ power = 99 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.DEX] == xi.trust.auraValue(99, 4))
        assert(effect.mods[xi.mod.ACC] == xi.trust.auraValue(99, 24))
        assert(effect.mods[xi.mod.RACC] == xi.trust.auraValue(99, 24))
        assert(effect.mods[xi.mod.DEX] == 4)
        assert(effect.mods[xi.mod.ACC] == 24)
    end)

    it('chr aura applies DEFP float curve and CHR/MDEF', function()
        local script = require('scripts/effects/trust_aura_chr')
        local effect = stubEffect({ power = 99 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.DEFP] == xi.trust.auraValue(99, 9.7))
        assert(effect.mods[xi.mod.DEFP] == 9)
        assert(effect.mods[xi.mod.CHR] == 5)
        assert(effect.mods[xi.mod.MDEF] == 5)

        effect = stubEffect({ power = 50 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.DEFP] == 4)
        assert(effect.mods[xi.mod.CHR] == 2)
    end)

    it('exp aura applies EXP_BONUS power', function()
        local script = require('scripts/effects/trust_aura_exp')
        local effect = stubEffect({ power = 20 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.EXP_BONUS] == 20)
    end)

    it('haste aura applies static HASTE_MAGIC and ACC ladder', function()
        local script = require('scripts/effects/trust_aura_haste')
        local effect = stubEffect({ power = 99 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.HASTE_MAGIC] == 2000)
        assert(effect.mods[xi.mod.ACC] == 30)
        assert(effect.mods[xi.mod.RACC] == 30)
        assert(effect.mods[xi.mod.MACC] == 30)
    end)

    it('magic attack aura uses maxVal 6 for MATT/MACC', function()
        local script = require('scripts/effects/trust_aura_magic_attack')
        local effect = stubEffect({ power = 99 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.MATT] == 6)
        assert(effect.mods[xi.mod.MACC] == 6)
        assert(effect.mods[xi.mod.MATT] == xi.trust.auraValue(99, 6))
    end)

    it('refresh aura applies REFRESH curve and MAGIC_SKILLUP_RATE 20', function()
        local script = require('scripts/effects/trust_aura_refresh')
        local effect = stubEffect({ power = 99 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.REFRESH] == 3)
        assert(effect.mods[xi.mod.MAGIC_SKILLUP_RATE] == 20)
    end)

    it('regen aura applies REGEN curve and COMBAT_SKILLUP_RATE 20', function()
        local script = require('scripts/effects/trust_aura_regen')
        local effect = stubEffect({ power = 99 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.REGEN] == 6)
        assert(effect.mods[xi.mod.COMBAT_SKILLUP_RATE] == 20)
    end)

    it('lvl 1 auraValue floors to minimum 1', function()
        local script = require('scripts/effects/trust_aura_acc')
        local effect = stubEffect({ power = 1 })
        script.onEffectGain({}, effect)
        assert(effect.mods[xi.mod.DEX] == 1)
        assert(effect.mods[xi.mod.ACC] == 1)
        assert(effect.mods[xi.mod.RACC] == 1)
    end)
end)
