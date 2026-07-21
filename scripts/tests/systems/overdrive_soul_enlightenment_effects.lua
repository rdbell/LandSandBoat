-----------------------------------
-- Pure system tests for Overdrive, Soul Enslavement, and Enlightenment
-- status-effect scripts.
-----------------------------------

local function stubEffect()
    return {
        mods = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

local function stubPC()
    return {
        updated = false,
        getObjType = function()
            return xi.objType.PC
        end,
        updateAttachments = function(self)
            self.updated = true
        end,
    }
end

local function stubPet(opts)
    opts = opts or {}
    local master = opts.master
    local frame = opts.frame or xi.automaton.frame.HARLEQUIN
    return {
        getObjType = function()
            return xi.objType.PET
        end,
        getMaster = function()
            return master
        end,
        getAutomatonFrame = function()
            return frame
        end,
    }
end

local function stubMaster(jpLevel)
    return {
        getJobPointLevel = function(_, jp)
            if jp == xi.jp.OVERDRIVE_EFFECT then
                return jpLevel or 0
            end
            return 0
        end,
    }
end

describe('Overdrive Soul Enslavement Enlightenment status-effect pure plans', function()
    it('overdrive PC applies OVERLOAD_THRESH 5000 and updateAttachments', function()
        local script = require('scripts/effects/overdrive')
        local target = stubPC()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.OVERLOAD_THRESH] == 5000)
        assert(target.updated == true)
        -- Lose re-updates attachments
        target.updated = false
        script.onEffectLose(target, effect)
        assert(target.updated == true)
    end)

    it('overdrive pet applies combat mods, frame DEFP, res ranks, and JP stats', function()
        local script = require('scripts/effects/overdrive')
        local master = stubMaster(3)
        local target = stubPet({ master = master, frame = xi.automaton.frame.HARLEQUIN })
        local effect = stubEffect()
        script.onEffectGain(target, effect)

        assert(effect.mods[xi.mod.HASTE_MAGIC] == 2500)
        assert(effect.mods[xi.mod.ATTP] == 25)
        assert(effect.mods[xi.mod.RATTP] == 25)
        assert(effect.mods[xi.mod.ACC] == 25)
        assert(effect.mods[xi.mod.RACC] == 25)
        assert(effect.mods[xi.mod.EVA] == 25)
        -- floor(400/20) = 20 for Harlequin
        assert(effect.mods[xi.mod.DEFP] == 20)
        assert(effect.mods[xi.mod.PARALYZE_RES_RANK] == 11)
        assert(effect.mods[xi.mod.BLIND_RES_RANK] == 11)
        -- JP 3 * 5 = 15 all main stats
        assert(effect.mods[xi.mod.STR] == 15)
        assert(effect.mods[xi.mod.CHR] == 15)
    end)

    it('overdrive pet frame DEFP divisors and early return without master', function()
        local script = require('scripts/effects/overdrive')

        local cases = {
            { xi.automaton.frame.VALOREDGE, 16 },  -- floor(400/24)
            { xi.automaton.frame.SHARPSHOT, 22 },  -- floor(400/18)
            { xi.automaton.frame.STORMWAKER, 25 }, -- floor(400/16)
        }
        for _, c in ipairs(cases) do
            local effect = stubEffect()
            local target = stubPet({ master = stubMaster(0), frame = c[1] })
            script.onEffectGain(target, effect)
            assert(effect.mods[xi.mod.DEFP] == c[2], 'frame ' .. tostring(c[1]))
            -- JP 0: no main-stat mods
            assert(effect.mods[xi.mod.STR] == nil)
        end

        -- No master: combat mods only, no DEFP / res / JP
        local effect = stubEffect()
        local target = stubPet({ master = nil, frame = xi.automaton.frame.HARLEQUIN })
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.HASTE_MAGIC] == 2500)
        assert(effect.mods[xi.mod.DEFP] == nil)
        assert(effect.mods[xi.mod.PARALYZE_RES_RANK] == nil)
    end)

    it('soul enslavement clears auspice/enspells and sets ENSPELL 22', function()
        local script = require('scripts/effects/soul_enslavement')
        local deleted = {}
        local target = {
            delStatusEffectSilent = function(_, id)
                table.insert(deleted, id)
            end,
        }
        local effect = stubEffect()
        script.onEffectGain(target, effect)

        assert(deleted[1] == xi.effect.AUSPICE)
        assert(deleted[2] == xi.effect.ENSTONE)
        assert(deleted[3] == xi.effect.ENSTONE_II)
        assert(deleted[#deleted] == xi.effect.ENDARK)
        assert(#deleted == 15)
        assert(effect.mods[xi.mod.ENSPELL] == 22)
        assert(effect.mods[xi.mod.ENSPELL_DMG] == 0)
    end)

    it('enlightenment applies INT and MND equal to power on target', function()
        local script = require('scripts/effects/enlightenment')
        local target = {
            mods = {},
            addMod = function(self, mod, delta)
                self.mods[mod] = (self.mods[mod] or 0) + delta
            end,
            delMod = function(self, mod, delta)
                self.mods[mod] = (self.mods[mod] or 0) - delta
            end,
        }
        local effect = {
            power = 10,
            getPower = function(self)
                return self.power
            end,
        }
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.INT] == 10)
        assert(target.mods[xi.mod.MND] == 10)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.INT] == 0)
        assert(target.mods[xi.mod.MND] == 0)
    end)
end)
