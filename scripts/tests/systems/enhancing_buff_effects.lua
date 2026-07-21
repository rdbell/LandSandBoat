-----------------------------------
-- Pure system tests for Protect, Shell, Flurry, Reprisal, and related buffs.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        deleted = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setMod = function(self, mod, value)
            self.mods[mod] = value
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.deleted, effect)
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

describe('Enhancing buff status-effect pure plans', function()
    it('protect and shell apply DEF and DMGMAGIC', function()
        local prot = require('scripts/effects/protect')
        local target = stubTarget()
        prot.onEffectGain(target, stubEffect(40))
        assert(target.mods[xi.mod.DEF] == 40)
        prot.onEffectLose(target, stubEffect(40))
        assert(target.mods[xi.mod.DEF] == 0)

        local shell = require('scripts/effects/shell')
        target = stubTarget()
        shell.onEffectGain(target, stubEffect(24))
        assert(target.mods[xi.mod.DMGMAGIC] == -24)
        shell.onEffectLose(target, stubEffect(24))
        assert(target.mods[xi.mod.DMGMAGIC] == 0)
    end)

    it('flurry and flurry ii clear haste and apply SNAPSHOT', function()
        local flurry = require('scripts/effects/flurry')
        local target = stubTarget()
        local effect = stubEffect(15)
        flurry.onEffectGain(target, effect)
        assert(target.deleted[1] == xi.effect.HASTE)
        assert(effect.mods[xi.mod.SNAPSHOT] == 15)

        local flurry2 = require('scripts/effects/flurry_ii')
        target = stubTarget()
        effect = stubEffect(30)
        flurry2.onEffectGain(target, effect)
        assert(target.deleted[1] == xi.effect.HASTE)
        assert(effect.mods[xi.mod.SNAPSHOT] == 30)
    end)

    it('reprisal applies SPIKES 6 and zeros SPIKES_DMG', function()
        local script = require('scripts/effects/reprisal')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.SPIKES] == 6)
        assert(target.mods[xi.mod.SPIKES_DMG] == 0)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.SPIKES] == 0)
        assert(target.mods[xi.mod.SPIKES_DMG] == 0)
    end)

    it('perfect counter intension pax and crit evasion down', function()
        local pc = require('scripts/effects/perfect_counter')
        local target = stubTarget()
        pc.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.PERFECT_COUNTER_ATT] == 100)

        local int = require('scripts/effects/intension')
        target = stubTarget()
        int.onEffectGain(target, stubEffect(20))
        assert(target.mods[xi.mod.MACC] == 20)

        local pax = require('scripts/effects/pax')
        target = stubTarget()
        pax.onEffectGain(target, stubEffect(-10))
        assert(target.mods[xi.mod.ENMITY] == -10)

        local crit = require('scripts/effects/critical_hit_evasion_down')
        local effect = stubEffect(15)
        crit.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.CRITICAL_HIT_EVASION] == -15)
    end)

    it('aquaveil is empty', function()
        local script = require('scripts/effects/aquaveil')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        script.onEffectLose(target, stubEffect())
        local count = 0
        for _ in pairs(target.mods) do
            count = count + 1
        end
        assert(count == 0)
    end)
end)
