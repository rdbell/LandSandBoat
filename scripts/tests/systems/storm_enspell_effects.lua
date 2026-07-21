-----------------------------------
-- Pure system tests for storm, Embrava, enmity, enspell, Klimaform effects.
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

describe('Storm and enspell status-effect pure plans', function()
    it('single-stat storms apply power to matching main stats', function()
        local cases = {
            { 'firestorm', xi.mod.STR },
            { 'hailstorm', xi.mod.INT },
            { 'windstorm', xi.mod.AGI },
            { 'sandstorm', xi.mod.VIT },
            { 'thunderstorm', xi.mod.DEX },
            { 'rainstorm', xi.mod.MND },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local target = stubTarget()
            local effect = stubEffect(15)
            script.onEffectGain(target, effect)
            assert(target.mods[c[2]] == 15, c[1])
            script.onEffectLose(target, effect)
            assert(target.mods[c[2]] == 0, c[1] .. ' lose')
        end
    end)

    it('aurorastorm applies effect-owned CHR', function()
        local script = require('scripts/effects/aurorastorm')
        local target = stubTarget()
        local effect = stubEffect(12)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.CHR] == 12)
        assert(target.mods[xi.mod.CHR] == nil)
    end)

    it('voidstorm applies floor power/2 to all seven stats', function()
        local script = require('scripts/effects/voidstorm')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect(11))
        for _, mod in ipairs({
            xi.mod.STR, xi.mod.DEX, xi.mod.VIT, xi.mod.AGI,
            xi.mod.INT, xi.mod.MND, xi.mod.CHR,
        }) do
            assert(target.mods[mod] == 5, tostring(mod))
        end
    end)

    it('embrava derives regen refresh haste from skill power', function()
        local script = require('scripts/effects/embrava')
        local target = stubTarget()
        local effect = stubEffect(350)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.REGEN] == 51)
        assert(target.mods[xi.mod.REFRESH] == 4)
        assert(target.mods[xi.mod.HASTE_MAGIC] == 1800)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.REGEN] == 0)
    end)

    it('enmity boost and down apply signed ENMITY power', function()
        local boost = require('scripts/effects/enmity_boost')
        local target = stubTarget()
        boost.onEffectGain(target, stubEffect(40))
        assert(target.mods[xi.mod.ENMITY] == 40)
        local down = require('scripts/effects/enmity_down')
        target = stubTarget()
        down.onEffectGain(target, stubEffect(25))
        assert(target.mods[xi.mod.ENMITY] == -25)
        down.onEffectLose(target, stubEffect(25))
        assert(target.mods[xi.mod.ENMITY] == 0)
    end)

    it('element enspells set ENSPELL element and damage on effect', function()
        local script = require('scripts/effects/enfire')
        local effect = stubEffect(20)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ENSPELL] == xi.element.FIRE)
        assert(effect.mods[xi.mod.ENSPELL_DMG] == 20)
    end)

    it('enlight and endark include JP bonuses', function()
        local enlight = require('scripts/effects/enlight')
        local target = stubTarget({ jp = { [xi.jp.ENLIGHT_EFFECT] = 5 } })
        local effect = stubEffect(30)
        enlight.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ENSPELL] == xi.element.LIGHT)
        assert(effect.mods[xi.mod.ENSPELL_DMG] == 35)
        assert(effect.mods[xi.mod.ACC] == 5)

        local endark = require('scripts/effects/endark')
        target = stubTarget({ jp = { [xi.jp.ENDARK_EFFECT] = 4 } })
        effect = stubEffect(30)
        endark.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ENSPELL] == xi.element.DARK)
        assert(effect.mods[xi.mod.ENSPELL_DMG] == 34)
        assert(effect.mods[xi.mod.ATT] == 4)
        assert(effect.mods[xi.mod.ACC] == 4)
    end)

    it('klimaform has empty body', function()
        local script = require('scripts/effects/klimaform')
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
