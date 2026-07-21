-----------------------------------
-- Pure system tests for Rune Fencer status-effect scripts / job_utils handlers.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        runes = opts.runes or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        getAllRuneEffects = function(self)
            return self.runes
        end,
        getHighestRuneEffect = function(self)
            return self.runes[1] or 0
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
        setPower = function(self, v)
            self.power = v
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Rune Fencer status-effect pure plans', function()
    it('swordplay gain uses EVASION skill mod; tick and lose use EVA', function()
        local util = xi.job_utils.rune_fencer
        local target = stubTarget()
        local effect = stubEffect(12, 10)
        util.onSwordplayEffectGain(target, effect)
        assert(target.mods[xi.mod.ACC] == 12)
        assert(target.mods[xi.mod.EVASION] == 12)
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 10)
        -- tick with JP 0, cap 60
        util.onSwordplayEffectTick(target, effect)
        assert(effect.power == 15)
        assert(target.mods[xi.mod.ACC] == 15)
        assert(target.mods[xi.mod.EVA] == 3)
        util.onSwordplayEffectLose(target, effect)
        -- lose del ACC/EVA by current power 15 and subtle 10
        assert(target.mods[xi.mod.ACC] == 0)
        assert(target.mods[xi.mod.EVA] == -12)
        assert(target.mods[xi.mod.SUBTLE_BLOW] == 0)
    end)

    it('battuta applies inquartata and parry spikes from subPower', function()
        local util = xi.job_utils.rune_fencer
        local target = stubTarget({ runes = { xi.effect.IGNIS } })
        local effect = stubEffect(56, 26)
        util.onBattutaEffectGain(target, effect)
        assert(effect.mods[xi.mod.INQUARTATA] == 56)
        assert(effect.mods[xi.mod.PARRY_SPIKES_DMG] == 26)
        assert(effect.mods[xi.mod.PARRY_SPIKES] ~= nil)
    end)

    it('foil decays SPECIAL_ATTACK_EVASION by 3 per tick', function()
        local script = require('scripts/effects/foil')
        local target = stubTarget()
        local effect = stubEffect(30)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.SPECIAL_ATTACK_EVASION] == 30)
        script.onEffectTick(target, effect)
        assert(effect.power == 27)
        assert(target.mods[xi.mod.SPECIAL_ATTACK_EVASION] == 27)
        script.onEffectLose(target, effect)
        assert(target.mods[xi.mod.SPECIAL_ATTACK_EVASION] == 0)
    end)

    it('elemental sforzo applies UDMGMAGIC -10000', function()
        local script = require('scripts/effects/elemental_sforzo')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.UDMGMAGIC] == -10000)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.UDMGMAGIC] == 0)
    end)

    it('pflug applies resist mods for each active rune at power+subPower', function()
        local util = xi.job_utils.rune_fencer
        local target = stubTarget({ runes = { xi.effect.IGNIS, xi.effect.LUX } })
        local effect = stubEffect(15, 4)
        util.onPflugEffectGain(target, effect)
        assert(effect.mods[xi.mod.PARALYZERES] == 19)
        assert(effect.mods[xi.mod.BINDRES] == 19)
        assert(effect.mods[xi.mod.SLEEPRES] == 19)
        assert(effect.mods[xi.mod.BLINDRES] == 19)
        assert(effect.mods[xi.mod.CURSERES] == 19)
    end)

    it('vallation valiance liement embolden one_for_all are empty', function()
        for _, name in ipairs({ 'vallation', 'valiance', 'liement', 'embolden', 'one_for_all' }) do
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
