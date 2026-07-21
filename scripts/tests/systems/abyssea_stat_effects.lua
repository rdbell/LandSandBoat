-----------------------------------
-- Pure system tests for Abyssea stat status-effect scripts.
-----------------------------------

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

describe('Abyssea stat status-effect pure plans', function()
    it('main-stat abyssea effects apply effect-owned power', function()
        local cases = {
            { 'abyssea_str', xi.mod.STR },
            { 'abyssea_dex', xi.mod.DEX },
            { 'abyssea_vit', xi.mod.VIT },
            { 'abyssea_agi', xi.mod.AGI },
            { 'abyssea_int', xi.mod.INT },
            { 'abyssea_mnd', xi.mod.MND },
            { 'abyssea_chr', xi.mod.CHR },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local effect = stubEffect(40)
            script.onEffectGain(nil, effect)
            assert(effect.mods[c[2]] == 40, c[1])
        end
    end)

    it('abyssea hp and mp apply HPP and MPP', function()
        local hp = require('scripts/effects/abyssea_hp')
        local effect = stubEffect(20)
        hp.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.HPP] == 20)
        local mp = require('scripts/effects/abyssea_mp')
        effect = stubEffect(10)
        mp.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.MPP] == 10)
    end)
end)
