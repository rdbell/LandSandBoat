-----------------------------------
-- Pure system tests for Dancer status-effect scripts
-- (fan_dance, saber_dance, ternary_flourish, haste_samba_haste, empties).
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        merits = opts.merits or {},
        traits = opts.traits or {},
        deletedEffects = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getMerit = function(self, merit)
            return self.merits[merit] or 0
        end,
        hasTrait = function(self, trait)
            return self.traits[trait] == true
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
    }
end

local function stubEffect(power)
    return {
        power = power or 0,
        getPower = function(self)
            return self.power
        end,
        setPower = function(self, v)
            self.power = v
        end,
    }
end

describe('Dancer status-effect pure plans', function()
    it('fan dance clears sambas/saber and adds enmity 15', function()
        local script = require('scripts/effects/fan_dance')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.ENMITY] == 15)
        assert(#target.deletedEffects == 4)
        assert(target.deletedEffects[1] == xi.effect.HASTE_SAMBA)
        assert(target.deletedEffects[4] == xi.effect.SABER_DANCE)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.ENMITY] == 0)
    end)

    it('saber dance applies DA power, suppresses trait DA, decays to 20%', function()
        local script = require('scripts/effects/saber_dance')
        local target = stubTarget({
            merits = { [xi.merit.SABER_DANCE] = 8 },
            traits = { [xi.trait.DOUBLE_ATTACK] = true },
        })
        local effect = stubEffect(50)
        script.onEffectGain(target, effect)
        -- merits 8 → samba pdur 3; trait -10; power +50 → net DA 40
        assert(target.mods[xi.mod.SAMBA_PDURATION] == 3)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 40)
        assert(target.deletedEffects[1] == xi.effect.FAN_DANCE)
        script.onEffectTick(target, effect)
        assert(effect.power == 47)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 37)
        -- decay until 20
        while effect.power > 20 do
            script.onEffectTick(target, effect)
        end
        assert(effect.power == 20)
        local daAtFloor = target.mods[xi.mod.DOUBLE_ATTACK]
        script.onEffectTick(target, effect)
        assert(effect.power == 20)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == daAtFloor)
        script.onEffectLose(target, effect)
        -- restore trait +10, remove remaining power 20 and samba pdur 3
        assert(target.mods[xi.mod.SAMBA_PDURATION] == 0)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == daAtFloor + 10 - 20)
    end)

    it('saber dance without high merits skips samba duration mod', function()
        local script = require('scripts/effects/saber_dance')
        local target = stubTarget({ merits = { [xi.merit.SABER_DANCE] = 5 } })
        script.onEffectGain(target, stubEffect(50))
        assert(target.mods[xi.mod.SAMBA_PDURATION] == nil)
        assert(target.mods[xi.mod.DOUBLE_ATTACK] == 50)
    end)

    it('ternary flourish grants 100 triple attack', function()
        local script = require('scripts/effects/ternary_flourish')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.TRIPLE_ATTACK] == 100)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.TRIPLE_ATTACK] == 0)
    end)

    it('haste samba haste applies HASTE_ABILITY power', function()
        local script = require('scripts/effects/haste_samba_haste')
        local target = stubTarget()
        local effect = stubEffect(512)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.HASTE_ABILITY] == 512)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.HASTE_ABILITY] == 0)
    end)

    it('samba trance grand pas and empty flourishes have empty bodies', function()
        local empties = {
            'haste_samba', 'drain_samba', 'drain_samba_ii', 'drain_samba_iii',
            'aspir_samba', 'aspir_samba_ii',
            'trance', 'grand_pas', 'contradance',
            'building_flourish', 'climactic_flourish', 'striking_flourish',
        }
        for _, name in ipairs(empties) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            local effect = stubEffect(1)
            script.onEffectGain(target, effect)
            script.onEffectTick(target, effect)
            script.onEffectLose(target, effect)
            local count = 0
            for _ in pairs(target.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
