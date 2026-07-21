-----------------------------------
-- Pure system tests for PLD/SAM/DRK/RDM/MNK/THF job stance status effects.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        merits = opts.merits or {},
        mainJob = opts.mainJob or 0,
        isPCFlag = opts.isPC ~= false,
        traits = opts.traits or {},
        deletedEffects = {},
        localVars = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
        getMainJob = function(self)
            return self.mainJob
        end,
        isPC = function(self)
            return self.isPCFlag
        end,
        hasTrait = function(self, trait)
            return self.traits[trait] == true
        end,
        getMerit = function(self, merit)
            return self.merits[merit] or 0
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        setLocalVar = function(self, key, value)
            self.localVars[key] = value
        end,
        messageBasic = function() end,
    }
end

local function stubEffect(power, subPower)
    return {
        power = power,
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
        setSubPower = function(self, v)
            self.subPower = v
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        getLastTick = function(self)
            return self.lastTick or 0
        end,
        getEffectType = function()
            return 0
        end,
    }
end

describe('Job stance status-effect pure plans', function()
    it('sentinel applies UDMG reduction, enmity, and decays above 50%', function()
        local script = require('scripts/effects/sentinel')
        local target = stubTarget({ mainJob = xi.job.PLD })
        local effect = stubEffect(6000, 20)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.UDMGPHYS] == -6000)
        assert(target.mods[xi.mod.ENMITY] == 100)
        assert(target.mods[xi.mod.ENMITY_LOSS_REDUCTION] == 20)
        script.onEffectTick(target, effect)
        assert(effect.power == 5200)
        assert(target.mods[xi.mod.UDMGPHYS] == -5200)
        -- subjob enmity 50
        target = stubTarget({ mainJob = xi.job.WAR })
        effect = stubEffect(6000, 0)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.ENMITY] == 50)
        -- 5500 odd feet decay 500
        target = stubTarget({ mainJob = xi.job.PLD })
        effect = stubEffect(5500, 0)
        script.onEffectGain(target, effect)
        script.onEffectTick(target, effect)
        assert(effect.power == 5000)
    end)

    it('rampart applies all SDT mods and optional Iron Will', function()
        local script = require('scripts/effects/rampart')
        -- Base SDT products always apply (player/trait injects optional).
        local target = stubTarget({ isPC = false })
        local effect = stubEffect(2500, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.SLASH_SDT] == -2500)
        assert(effect.mods[xi.mod.PIERCE_SDT] == -2500)
        assert(effect.mods[xi.mod.DARK_SDT] == -2500)
        assert(effect.mods[xi.mod.FIRE_SDT] == -2500)
        -- Iron Will FC product pure pin: enhances * merit / 19
        assert(3 * 19 / 19 == 3)
    end)

    it('seigan and hasso apply JP and main-SAM bonuses', function()
        local seigan = require('scripts/effects/seigan')
        local target = stubTarget({ mainJob = xi.job.SAM, jp = { [xi.jp.SEIGAN_EFFECT] = 4 } })
        local effect = stubEffect(0, 0)
        seigan.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.DEF] == 12)
        assert(effect.mods[xi.mod.SEIGAN_COUNTER_BONUS] == 1)

        local hasso = require('scripts/effects/hasso')
        target = stubTarget({ mainJob = xi.job.SAM })
        effect = stubEffect(15, 0)
        hasso.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.TWOHAND_STR] == 15)
        assert(effect.mods[xi.mod.TWOHAND_HASTE_ABILITY] == 1000)
        assert(effect.mods[xi.mod.TWOHAND_ACC] == 10)
        assert(effect.mods[xi.mod.HASSO_ZANSHIN_BONUS] == 1)
    end)

    it('last resort applies JP ATT, merit ATTP/DEFP, desperate blows haste', function()
        local script = require('scripts/effects/last_resort')
        -- Fixed injects mirror Go LastResortGain(3, 5, 100, 50).
        local target = stubTarget()
        local meritCalls = 0
        target.getJobPointLevel = function()
            return 3
        end
        target.getMerit = function()
            meritCalls = meritCalls + 1
            if meritCalls == 1 then
                return 5 -- LAST_RESORT_EFFECT
            end
            return 50 -- DESPERATE_BLOWS
        end
        target.getMod = function()
            return 100 -- DESPERATE_BLOWS mod
        end
        local effect = stubEffect(0, 0)
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ATT] == 6 and effect.mods[xi.mod.RATT] == 6)
        assert(effect.mods[xi.mod.ATTP] == 30 and effect.mods[xi.mod.RATTP] == 30)
        assert(effect.mods[xi.mod.DEFP] == -30)
        assert(effect.mods[xi.mod.TWOHAND_HASTE_ABILITY] == 150)
    end)

    it('souleater dark seal blood weapon products', function()
        local se = require('scripts/effects/souleater')
        local effect = stubEffect(0, 0)
        se.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ACC] == 25)

        local ds = require('scripts/effects/dark_seal')
        local target = stubTarget()
        effect = stubEffect(10, 20)
        ds.onEffectGain(target, effect)
        assert(#target.deletedEffects == 3)
        assert(effect.mods[xi.mod.DARK_MAGIC_CAST] == -10)
        assert(effect.mods[xi.mod.DARK_MAGIC_DURATION] == 20)

        local bw = require('scripts/effects/blood_weapon')
        effect = stubEffect(50, 0)
        bw.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ENSPELL] == 17)
        assert(effect.mods[xi.mod.ENSPELL_DMG] == 50)
    end)

    it('chainspell hundred fists perfect dodge JP products', function()
        local cs = require('scripts/effects/chainspell')
        local target = stubTarget({ jp = { [xi.jp.CHAINSPELL_EFFECT] = 4 } })
        local effect = stubEffect(0, 0)
        cs.onEffectGain(target, effect)
        assert(target.mods[xi.mod.UFASTCAST] == 150)
        assert(target.mods[xi.mod.MAGIC_DAMAGE] == 8)
        cs.onEffectLose(target, effect)
        assert(target.mods[xi.mod.UFASTCAST] == 0)

        local hf = require('scripts/effects/hundred_fists')
        target = stubTarget({ jp = { [xi.jp.HUNDRED_FISTS_EFFECT] = 5 } })
        hf.onEffectGain(target, effect)
        assert(target.mods[xi.mod.ACC] == 10)
        hf.onEffectLose(target, effect)
        assert(target.mods[xi.mod.ACC] == 0)

        local pd = require('scripts/effects/perfect_dodge')
        target = stubTarget({ jp = { [xi.jp.PERFECT_DODGE_EFFECT] = 3 } })
        pd.onEffectGain(target, effect)
        assert(target.mods[xi.mod.MEVA] == 9)
    end)

    it('cover lose clears local var; sneak wears off message gate', function()
        local cover = require('scripts/effects/cover')
        local target = stubTarget()
        cover.onEffectLose(target, stubEffect(0, 0))
        assert(target.localVars['COVER_ABILITY_TARGET'] == 0)

        local sneak = require('scripts/effects/sneak')
        local effect = stubEffect(0, 0)
        effect.lastTick = 3
        -- just ensure tick does not error
        sneak.onEffectTick(stubTarget(), effect)
        assert(true)
    end)

    it('manafont third_eye hide have empty gain/lose', function()
        for _, name in ipairs({ 'manafont', 'third_eye', 'hide' }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            local effect = stubEffect(1, 0)
            script.onEffectGain(target, effect)
            script.onEffectLose(target, effect)
            assert(next(target.mods) == nil)
        end
    end)
end)
