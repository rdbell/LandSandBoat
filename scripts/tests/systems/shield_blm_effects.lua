-----------------------------------
-- Pure system tests for shield and Black Mage ability status effects.
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
        mods = {},
        getPower = function(self)
            return self.power
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Shield and BLM ability status-effect pure plans', function()
    it('magic shield applies tiered magic damage and absorb mods', function()
        local script = require('scripts/effects/magic_shield')
        local effect = stubEffect(0)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.DMGMAGIC] == -5000)

        effect = stubEffect(1)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.UDMGMAGIC] == -10000)

        effect = stubEffect(3)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.MAGIC_ABSORB] == 100)

        effect = stubEffect(2)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.FIRE_ABSORB] == 100)
        assert(effect.mods[xi.mod.EARTH_ABSORB] == 100)
        assert(effect.mods[xi.mod.WATER_ABSORB] == 100)
        assert(effect.mods[xi.mod.WIND_ABSORB] == 100)
        assert(effect.mods[xi.mod.ICE_ABSORB] == 100)
        assert(effect.mods[xi.mod.LTNG_ABSORB] == 100)
        assert(effect.mods[xi.mod.LIGHT_ABSORB] == 100)
        assert(effect.mods[xi.mod.DARK_ABSORB] == 100)
    end)

    it('physical shield applies tiered physical damage and absorb mods', function()
        local script = require('scripts/effects/physical_shield')
        local effect = stubEffect(0)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.DMGPHYS] == -5000)

        effect = stubEffect(1)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.UDMGPHYS] == -10000)

        effect = stubEffect(2)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.PHYS_ABSORB] == 100)
    end)

    it('arrow shield blocks all ranged damage', function()
        local script = require('scripts/effects/arrow_shield')
        local effect = stubEffect()
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.UDMGRANGE] == -10000)
    end)

    it('manawell and mana wall apply MP free cast and DMG reduction', function()
        local manawell = require('scripts/effects/manawell')
        local target = stubTarget()
        manawell.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.NO_SPELL_MP_DEPLETION] == 100)
        manawell.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.NO_SPELL_MP_DEPLETION] == 0)

        local wall = require('scripts/effects/mana_wall')
        local effect = stubEffect()
        wall.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.DMG] == -5000)
    end)

    it('cascade and manafont have empty bodies', function()
        for _, name in ipairs({ 'cascade', 'manafont' }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            script.onEffectGain(target, stubEffect())
            script.onEffectLose(target, stubEffect())
            local count = 0
            for _ in pairs(target.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
