-----------------------------------
-- Pure system tests for Geomancer geo_* and colure_active status effects.
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

describe('Geomancer status-effect pure plans', function()
    it('single-mod geo buffs apply effect-owned power', function()
        local cases = {
            { 'geo_regen', xi.mod.REGEN, 12 },
            { 'geo_refresh', xi.mod.REFRESH, 5 },
            { 'geo_haste', xi.mod.HASTE_MAGIC, 1000 },
            { 'geo_poison', xi.mod.REGEN_DOWN, 3 },
            { 'geo_paralysis', xi.mod.PARALYZE, 15 },
            { 'geo_weight', xi.mod.MOVE_SPEED_WEIGHT_PENALTY, 25 },
            { 'geo_str_boost', xi.mod.STR, 10 },
            { 'geo_dex_boost', xi.mod.DEX, 10 },
            { 'geo_vit_boost', xi.mod.VIT, 10 },
            { 'geo_agi_boost', xi.mod.AGI, 10 },
            { 'geo_int_boost', xi.mod.INT, 10 },
            { 'geo_mnd_boost', xi.mod.MND, 10 },
            { 'geo_chr_boost', xi.mod.CHR, 10 },
            { 'geo_defense_boost', xi.mod.DEFP, 20 },
            { 'geo_evasion_boost', xi.mod.EVA, 15 },
            { 'geo_magic_atk_boost', xi.mod.MATT, 12 },
            { 'geo_magic_def_boost', xi.mod.MDEF, 12 },
            { 'geo_magic_acc_boost', xi.mod.MACC, 12 },
            { 'geo_magic_evasion_boost', xi.mod.MEVA, 12 },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local effect = stubEffect(c[3])
            script.onEffectGain(nil, effect)
            assert(effect.mods[c[2]] == c[3], c[1])
        end
    end)

    it('geo slow and downs apply negated power', function()
        local slow = require('scripts/effects/geo_slow')
        local effect = stubEffect(800)
        slow.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.HASTE_MAGIC] == -800)

        local cases = {
            { 'geo_defense_down', xi.mod.DEFP },
            { 'geo_evasion_down', xi.mod.EVA },
            { 'geo_magic_atk_down', xi.mod.MATT },
            { 'geo_magic_def_down', xi.mod.MDEF },
            { 'geo_magic_acc_down', xi.mod.MACC },
            { 'geo_magic_evasion_down', xi.mod.MEVA },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            effect = stubEffect(12)
            script.onEffectGain(nil, effect)
            assert(effect.mods[c[2]] == -12, c[1])
        end
    end)

    it('geo attack boost applies ATTP and RATTP; attack down only ATTP', function()
        local boost = require('scripts/effects/geo_attack_boost')
        local effect = stubEffect(20)
        boost.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.ATTP] == 20)
        assert(effect.mods[xi.mod.RATTP] == 20)

        local down = require('scripts/effects/geo_attack_down')
        effect = stubEffect(20)
        down.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.ATTP] == -20)
        assert(effect.mods[xi.mod.RATTP] == nil)
    end)

    it('geo accuracy boost and down apply ACC and RACC', function()
        local boost = require('scripts/effects/geo_accuracy_boost')
        local effect = stubEffect(15)
        boost.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.ACC] == 15)
        assert(effect.mods[xi.mod.RACC] == 15)

        local down = require('scripts/effects/geo_accuracy_down')
        effect = stubEffect(15)
        down.onEffectGain(nil, effect)
        assert(effect.mods[xi.mod.ACC] == -15)
        assert(effect.mods[xi.mod.RACC] == -15)
    end)

    it('colure active has empty body', function()
        local script = require('scripts/effects/colure_active')
        local effect = stubEffect(1)
        script.onEffectGain(nil, effect)
        script.onEffectLose(nil, effect)
        local count = 0
        for _ in pairs(effect.mods) do
            count = count + 1
        end
        assert(count == 0)
    end)
end)
