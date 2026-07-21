-----------------------------------
-- Pure system tests for Tomahawk and Arcane Crest status-effect scripts.
-----------------------------------

local function stubTarget(mods)
    return {
        mods = mods or {},
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
    }
end

local function stubEffect()
    return {
        mods = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Tomahawk and Arcane Crest status-effect pure plans', function()
    it('tomahawk applies floor 25 percent SDT reductions on effect', function()
        local script = require('scripts/effects/tomahawk')
        local target = stubTarget({
            [xi.mod.SLASH_SDT] = 10000,
            [xi.mod.PIERCE_SDT] = 8000,
            [xi.mod.IMPACT_SDT] = 0,
            [xi.mod.HTH_SDT] = -4000,
            [xi.mod.FIRE_SDT] = 10000,
            [xi.mod.DARK_SDT] = 2000,
        })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.SLASH_SDT] == -2500)
        assert(effect.mods[xi.mod.PIERCE_SDT] == -2000)
        assert(effect.mods[xi.mod.IMPACT_SDT] == 0)
        -- floor(-4000*0.25)=-1000, addMod -adjustment → +1000
        assert(effect.mods[xi.mod.HTH_SDT] == 1000)
        assert(effect.mods[xi.mod.FIRE_SDT] == -2500)
        assert(effect.mods[xi.mod.DARK_SDT] == -500)
        -- ice unset → 0
        assert(effect.mods[xi.mod.ICE_SDT] == 0 or effect.mods[xi.mod.ICE_SDT] == nil)
    end)

    it('tomahawk negative floor parity for odd values', function()
        local script = require('scripts/effects/tomahawk')
        local target = stubTarget({ [xi.mod.SLASH_SDT] = -5 })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        -- math.floor(-1.25) = -2; addMod(SLASH, -(-2)) = +2
        assert(effect.mods[xi.mod.SLASH_SDT] == 2)
    end)

    it('arcane crest applies negative power to ACC EVA MACC MEVA STORETP on effect', function()
        local script = require('scripts/effects/arcane_crest')
        local effect = {
            power = 20,
            mods = {},
            getPower = function(self)
                return self.power
            end,
            addMod = function(self, mod, delta)
                self.mods[mod] = (self.mods[mod] or 0) + delta
            end,
        }
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ACC] == -20)
        assert(effect.mods[xi.mod.EVA] == -20)
        assert(effect.mods[xi.mod.MACC] == -20)
        assert(effect.mods[xi.mod.MEVA] == -20)
        assert(effect.mods[xi.mod.STORETP] == -20)
    end)
end)
