-----------------------------------
-- Pure system tests for Summoner/Paladin ability status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        petMods = {},
        jp = opts.jp or {},
        isPCFlag = opts.isPC ~= false,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        addPetMod = function(self, mod, delta)
            self.petMods[mod] = (self.petMods[mod] or 0) + delta
        end,
        delPetMod = function(self, mod, delta)
            self.petMods[mod] = (self.petMods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        isPC = function(self)
            return self.isPCFlag
        end,
        recalculateAbilitiesTable = function() end,
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

describe('SMN and PLD ability status-effect pure plans', function()
    it('astral flow applies pet main-stat JP bonus for PCs', function()
        local script = require('scripts/effects/astral_flow')
        local target = stubTarget({ jp = { [xi.jp.ASTRAL_FLOW_EFFECT] = 3 } })
        script.onEffectGain(target, stubEffect())
        assert(target.petMods[xi.mod.STR] == 15)
        assert(target.petMods[xi.mod.CHR] == 15)
        script.onEffectLose(target, stubEffect())
        assert(target.petMods[xi.mod.STR] == 0)

        -- Non-PC: no pet mods
        target = stubTarget({ isPC = false, jp = { [xi.jp.ASTRAL_FLOW_EFFECT] = 5 } })
        script.onEffectGain(target, stubEffect())
        assert(target.petMods[xi.mod.STR] == nil)
    end)

    it('astral conduit applies BP_DELAY 99 and MPP 100 on effect', function()
        local script = require('scripts/effects/astral_conduit')
        local effect = stubEffect()
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.BP_DELAY] == 99)
        assert(effect.mods[xi.mod.MPP] == 100)
    end)

    it('palisade applies PALISADE_BLOCK_BONUS power', function()
        local script = require('scripts/effects/palisade')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect(30))
        assert(target.mods[xi.mod.PALISADE_BLOCK_BONUS] == 30)
        script.onEffectLose(target, stubEffect(30))
        assert(target.mods[xi.mod.PALISADE_BLOCK_BONUS] == 0)
    end)

    it('majesty applies cure potency II and white magic recast reduction', function()
        local script = require('scripts/effects/majesty')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect(20))
        assert(target.mods[xi.mod.CURE_POTENCY_II] == 20)
        assert(target.mods[xi.mod.WHITE_MAGIC_RECAST] == -20)
        script.onEffectLose(target, stubEffect(20))
        assert(target.mods[xi.mod.CURE_POTENCY_II] == 0)
        assert(target.mods[xi.mod.WHITE_MAGIC_RECAST] == 0)
    end)

    it('fealty and mana cede are empty', function()
        for _, name in ipairs({ 'fealty', 'mana_cede' }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            script.onEffectGain(target, stubEffect())
            script.onEffectLose(target, stubEffect())
            local count = 0
            for _ in pairs(target.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
