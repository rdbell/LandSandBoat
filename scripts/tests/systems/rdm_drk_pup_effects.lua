-----------------------------------
-- Pure system tests for RDM/DRK/PUP ability status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    local pet = opts.pet
    return {
        mods = {},
        mainLvl = opts.mainLvl or 1,
        jp = opts.jp or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getMainLvl = function(self)
            return self.mainLvl
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        getPet = function()
            return pet
        end,
    }
end

local function stubPet()
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
        flags = {},
        getPower = function(self)
            return self.power
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        addEffectFlag = function(self, flag)
            self.flags[flag] = true
        end,
    }
end

describe('RDM DRK PUP status-effect pure plans', function()
    it('composure applies ACC from level formula plus JP', function()
        local script = require('scripts/effects/composure')
        local target = stubTarget({ mainLvl = 75, jp = { [xi.jp.COMPOSURE_EFFECT] = 2 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        -- floor((24*75+74)/49)+2 = 38+2 = 40
        assert(effect.mods[xi.mod.ACC] == 40)
    end)

    it('spontaneity applies UFASTCAST 150 and magic begin flag', function()
        local script = require('scripts/effects/spontaneity')
        local target = stubTarget()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.UFASTCAST] == 150)
        assert(effect.flags[xi.effectFlag.MAGIC_BEGIN] == true)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.UFASTCAST] == 0)
    end)

    it('diabolic eye applies ACC power and HPP -15', function()
        local script = require('scripts/effects/diabolic_eye')
        local effect = stubEffect(25)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ACC] == 25)
        assert(effect.mods[xi.mod.HPP] == -15)
    end)

    it('element maneuvers apply pet main stats when pet present', function()
        local cases = {
            { 'fire_maneuver', xi.mod.STR },
            { 'ice_maneuver', xi.mod.INT },
            { 'wind_maneuver', xi.mod.AGI },
            { 'earth_maneuver', xi.mod.VIT },
            { 'thunder_maneuver', xi.mod.DEX },
            { 'water_maneuver', xi.mod.MND },
            { 'light_maneuver', xi.mod.CHR },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local pet = stubPet()
            local target = stubTarget({ pet = pet })
            script.onEffectGain(target, stubEffect(12))
            assert(pet.mods[c[2]] == 12, c[1])
            script.onEffectLose(target, stubEffect(12))
            assert(pet.mods[c[2]] == 0, c[1] .. ' lose')
            -- no pet: no crash, no mods
            script.onEffectGain(stubTarget(), stubEffect(12))
        end
    end)

    it('saboteur nether void scarlet delirium and dark maneuver are empty', function()
        for _, name in ipairs({ 'saboteur', 'nether_void', 'scarlet_delirium', 'dark_maneuver' }) do
            local script = require('scripts/effects/' .. name)
            local pet = stubPet()
            local target = stubTarget({ pet = pet })
            local effect = stubEffect(5)
            script.onEffectGain(target, effect)
            script.onEffectLose(target, effect)
            local count = 0
            for _ in pairs(effect.mods) do
                count = count + 1
            end
            for _ in pairs(target.mods) do
                count = count + 1
            end
            for _ in pairs(pet.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
