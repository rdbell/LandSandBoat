-----------------------------------
-- Pure system tests for regen/refresh/poison/bio/dia/stoneskin/blink/
-- copy_image/spikes status-effect scripts.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        deletedEffects = {},
        setMods = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setMod = function(self, mod, value)
            self.setMods[mod] = value
            self.mods[mod] = value
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
    }
end

local function stubEffect(power, subPower, tier)
    return {
        power = power,
        subPower = subPower or 0,
        tier = tier or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        getTier = function(self)
            return self.tier
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Recovery, DoT, and spikes status-effect pure plans', function()
    it('regen adds REGEN power via effect:addMod', function()
        local script = require('scripts/effects/regen')
        local effect = stubEffect(12, 0, 0)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.REGEN] == 12)
    end)

    it('refresh tier 3 clears sublimation and adds REFRESH', function()
        local script = require('scripts/effects/refresh')
        local target = stubTarget()
        local effect = stubEffect(5, 0, 3)
        script.onEffectGain(target, effect)
        assert(target.deletedEffects[1] == xi.effect.SUBLIMATION_ACTIVATED)
        assert(target.deletedEffects[2] == xi.effect.SUBLIMATION_COMPLETE)
        assert(effect.mods[xi.mod.REFRESH] == 5)
        -- tier 1 does not clear
        target = stubTarget()
        effect = stubEffect(5, 0, 1)
        script.onEffectGain(target, effect)
        assert(#target.deletedEffects == 0)
    end)

    it('poison applies REGEN_DOWN and resets immunobreak', function()
        local script = require('scripts/effects/poison')
        local target = stubTarget()
        local effect = stubEffect(7, 0, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.REGEN_DOWN] == 7)
        assert(target.mods[xi.mod.POISON_IMMUNOBREAK] == 0)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.REGEN_DOWN] == 0)
    end)

    it('bio regular applies REGEN_DOWN and ATTP-sub; nightmare tier skips REGEN_DOWN', function()
        local script = require('scripts/effects/bio')
        local effect = stubEffect(3, 10, 1)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.REGEN_DOWN] == 3)
        assert(effect.mods[xi.mod.ATTP] == -10)
        effect = stubEffect(2, 10, 11)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.REGEN_DOWN] == nil)
        assert(effect.mods[xi.mod.ATTP] == -10)
    end)

    it('dia applies REGEN_DOWN and DEFP-sub', function()
        local script = require('scripts/effects/dia')
        local effect = stubEffect(2, 53, 0)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.REGEN_DOWN] == 2)
        assert(effect.mods[xi.mod.DEFP] == -53)
    end)

    it('stoneskin adds STONESKIN power', function()
        local script = require('scripts/effects/stoneskin')
        local effect = stubEffect(350, 0, 0)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.STONESKIN] == 350)
    end)

    it('blink and copy_image set absolute mod values', function()
        local blink = require('scripts/effects/blink')
        local target = stubTarget()
        local effect = stubEffect(3, 0, 0)
        blink.onEffectGain(target, effect)
        assert(target.setMods[xi.mod.BLINK] == 3)
        blink.onEffectLose(target, effect)
        assert(target.setMods[xi.mod.BLINK] == 0)

        local copy = require('scripts/effects/copy_image')
        target = stubTarget()
        effect = stubEffect(0, 4, 0)
        copy.onEffectGain(target, effect)
        assert(target.setMods[xi.mod.UTSUSEMI] == 4)
        copy.onEffectLose(target, effect)
        assert(target.setMods[xi.mod.UTSUSEMI] == 0)
    end)

    it('blaze/ice/shock spikes set SPIKES type and SPIKES_DMG', function()
        local cases = {
            { 'blaze_spikes', 1 },
            { 'ice_spikes', 2 },
            { 'shock_spikes', 5 },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local target = stubTarget()
            local effect = stubEffect(15, 0, 0)
            script.onEffectGain(target, effect)
            assert(target.mods[xi.mod.SPIKES] == c[2])
            assert(target.mods[xi.mod.SPIKES_DMG] == 15)
            script.onEffectLose(target, effect)
            assert(target.mods[xi.mod.SPIKES] == 0)
            assert(target.mods[xi.mod.SPIKES_DMG] == 0)
        end
    end)
end)
