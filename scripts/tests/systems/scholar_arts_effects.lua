-----------------------------------
-- Pure system tests for Scholar arts status-effect scripts
-- (light_arts, dark_arts, tabula_rasa).
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    return {
        mods = {},
        effects = effects,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] == true
        end,
        recalculateAbilitiesTable = function() end,
        recalculateSkillsTable = function() end,
        getObjType = function()
            return opts.objType or xi.objType.PC
        end,
    }
end

local function stubEffect(power, subPower)
    return {
        power = power,
        subPower = subPower or 0,
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
    }
end

describe('Scholar arts status-effect pure plans', function()
    it('light arts applies white bonus and trade-off when no tabula', function()
        local script = require('scripts/effects/light_arts')
        local target = stubTarget()
        local effect = stubEffect(7, 24)
        script.onEffectGain(target, effect)
        -- power bonus + favored -10 = -17 white cost
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == -17)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == 20)
        assert(target.mods[xi.mod.LIGHT_ARTS_REGEN] == 24)
        assert(target.mods[xi.mod.REGEN_DURATION] == 48)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == 0)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == 0)
    end)

    it('light arts under tabula only applies power bonus', function()
        local script = require('scripts/effects/light_arts')
        local target = stubTarget({ effects = { [xi.effect.TABULA_RASA] = true } })
        local effect = stubEffect(7, 24)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == -7)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == nil)
        assert(target.mods[xi.mod.LIGHT_ARTS_REGEN] == nil)
    end)

    it('dark arts applies black bonus and helix when no tabula', function()
        local script = require('scripts/effects/dark_arts')
        local target = stubTarget()
        local effect = stubEffect(1, 24)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == -11) -- -1 + -10
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == 20)
        assert(target.mods[xi.mod.HELIX_EFFECT] == 24)
        assert(target.mods[xi.mod.HELIX_DURATION] == 72)
    end)

    it('tabula rasa with light arts boosts black and scales regen', function()
        local script = require('scripts/effects/tabula_rasa')
        local target = stubTarget({ effects = { [xi.effect.LIGHT_ARTS] = true } })
        local effect = stubEffect(36, 24) -- power=helix, sub=regen
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == -30)
        assert(target.mods[xi.mod.LIGHT_ARTS_REGEN] == 16) -- ceil(24/1.5)
        assert(target.mods[xi.mod.REGEN_DURATION] == 32) -- ceil(48/1.5)
        assert(target.mods[xi.mod.HELIX_EFFECT] == 36)
        assert(target.mods[xi.mod.HELIX_DURATION] == 108)
    end)

    it('tabula rasa with dark arts boosts white and scales helix', function()
        local script = require('scripts/effects/tabula_rasa')
        local target = stubTarget({ effects = { [xi.effect.DARK_ARTS] = true } })
        local effect = stubEffect(36, 24)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == -30)
        assert(target.mods[xi.mod.LIGHT_ARTS_REGEN] == 24)
        assert(target.mods[xi.mod.REGEN_DURATION] == 48)
        assert(target.mods[xi.mod.HELIX_EFFECT] == 24) -- ceil(36/1.5)
        assert(target.mods[xi.mod.HELIX_DURATION] == 36)
    end)

    it('tabula rasa alone boosts both schools moderately', function()
        local script = require('scripts/effects/tabula_rasa')
        local target = stubTarget()
        local effect = stubEffect(36, 24)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == -10)
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == -10)
        assert(target.mods[xi.mod.HELIX_DURATION] == 108)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == 0)
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == 0)
    end)

    it('stratagem flag effects have empty bodies', function()
        for _, name in ipairs({
            'accession', 'manifestation', 'penury', 'parsimony',
            'celerity', 'alacrity', 'rapture', 'ebullience',
        }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            local effect = stubEffect(1, 0)
            script.onEffectGain(target, effect)
            script.onEffectLose(target, effect)
            assert(next(target.mods) == nil)
        end
    end)
end)
