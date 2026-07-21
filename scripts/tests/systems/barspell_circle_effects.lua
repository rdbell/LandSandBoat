-----------------------------------
-- Pure system tests for barspell, killer-circle, killer instinct,
-- and barrage status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
    }
end

local function stubEffect(power, subPower)
    return {
        power = power or 0,
        subPower = subPower or 0,
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

describe('Barspell and circle status-effect pure plans', function()
    it('element barspells apply power to matching elemental MEVA', function()
        local cases = {
            { 'barfire', xi.mod.FIRE_MEVA },
            { 'barblizzard', xi.mod.ICE_MEVA },
            { 'baraero', xi.mod.WIND_MEVA },
            { 'barstone', xi.mod.EARTH_MEVA },
            { 'barthunder', xi.mod.THUNDER_MEVA },
            { 'barwater', xi.mod.WATER_MEVA },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local target = stubTarget()
            local effect = stubEffect(40)
            script.onEffectGain(target, effect)
            assert(target.mods[c[2]] == 40, c[1])
            script.onEffectLose(target, effect)
            assert(target.mods[c[2]] == 0, c[1] .. ' lose')
        end
    end)

    it('status barspells apply power to matching status MEVA', function()
        local cases = {
            { 'barsleep', xi.mod.SLEEP_MEVA },
            { 'barpoison', xi.mod.POISON_MEVA },
            { 'barparalyze', xi.mod.PARALYZE_MEVA },
            { 'barblind', xi.mod.BLIND_MEVA },
            { 'barsilence', xi.mod.SILENCE_MEVA },
            { 'barpetrify', xi.mod.PETRIFY_MEVA },
            { 'barvirus', xi.mod.VIRUS_MEVA },
            { 'baramnesia', xi.mod.AMNESIA_MEVA },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local target = stubTarget()
            local effect = stubEffect(25)
            script.onEffectGain(target, effect)
            assert(target.mods[c[2]] == 25, c[1])
            script.onEffectLose(target, effect)
            assert(target.mods[c[2]] == 0, c[1] .. ' lose')
        end
    end)

    it('holy ancient arcane circles apply effect-owned killer mods', function()
        local cases = {
            { 'holy_circle', xi.mod.UNDEAD_KILLER, 15 },
            { 'ancient_circle', xi.mod.DRAGON_KILLER, 12 },
            { 'arcane_circle', xi.mod.ARCANA_KILLER, 8 },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local target = stubTarget()
            local effect = stubEffect(c[3])
            script.onEffectGain(target, effect)
            assert(effect.mods[c[2]] == c[3], c[1])
            assert(target.mods[c[2]] == nil, c[1] .. ' not target')
        end
    end)

    it('warding circle adds JP to demon killer on effect', function()
        local script = require('scripts/effects/warding_circle')
        local target = stubTarget({ jp = { [xi.jp.WARDING_CIRCLE_EFFECT] = 3 } })
        local effect = stubEffect(10)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.DEMON_KILLER] == 13)
    end)

    it('killer instinct maps ecosystem subPower to correlated killer mod', function()
        local script = require('scripts/effects/killer_instinct')
        local map = {
            { xi.ecosystem.AMORPH, xi.mod.BIRD_KILLER },
            { xi.ecosystem.AQUAN, xi.mod.AMORPH_KILLER },
            { xi.ecosystem.BEAST, xi.mod.LIZARD_KILLER },
            { xi.ecosystem.BIRD, xi.mod.AQUAN_KILLER },
            { xi.ecosystem.LIZARD, xi.mod.VERMIN_KILLER },
            { xi.ecosystem.PLANTOID, xi.mod.BEAST_KILLER },
            { xi.ecosystem.VERMIN, xi.mod.PLANTOID_KILLER },
        }
        for _, c in ipairs(map) do
            local effect = stubEffect(20, c[1])
            script.onEffectGain(stubTarget(), effect)
            assert(effect.mods[c[2]] == 20, 'eco ' .. tostring(c[1]))
        end
        -- unknown ecosystem: no mod
        local effect = stubEffect(20, 0)
        script.onEffectGain(stubTarget(), effect)
        local count = 0
        for _ in pairs(effect.mods) do
            count = count + 1
        end
        assert(count == 0)
    end)

    it('barrage applies RATT equal to JP times 3 on effect', function()
        local script = require('scripts/effects/barrage')
        local target = stubTarget({ jp = { [xi.jp.BARRAGE_EFFECT] = 4 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.RATT] == 12)
    end)
end)
