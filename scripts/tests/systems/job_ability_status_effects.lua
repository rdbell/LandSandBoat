-----------------------------------
-- Pure system tests for MNK/THF/DRG job-ability status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        isMobFlag = opts.isMob or false,
        species = opts.species or 0,
        weaponDmg = opts.weaponDmg or 0,
        jobLevels = opts.jobLevels or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        isMob = function(self)
            return self.isMobFlag
        end,
        getSpecies = function(self)
            return self.species
        end,
        getWeaponDmg = function(self)
            return self.weaponDmg
        end,
        getJobLevel = function(self, job)
            return self.jobLevels[job] or 0
        end,
        updateHealth = function() end,
        setTP = function() end,
        uncharm = function() end,
    }
end

-- utils.getActiveJobLevel is used by focus/dodge; provide minimal stub via package.
-- The real function uses target:getMainJob/getSubJob; we inject via jobLevels + mainJob.
-- Instead require scripts that call utils.getActiveJobLevel — stub global utils if needed.

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
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Job ability status-effect pure plans', function()
    it('boost applies ATTP or Fantod MAIN_DMG_RATING for mobs', function()
        local script = require('scripts/effects/boost')
        local effect = stubEffect(25, 0)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ATTP] == 25)

        effect = stubEffect(400, 1)
        local mob = stubTarget({ isMob = true, weaponDmg = 100 })
        script.onEffectGain(mob, effect)
        assert(effect.mods[xi.mod.MAIN_DMG_RATING] == 300)
    end)

    it('counterstance applies COUNTER and optional bugbear ATTP', function()
        local script = require('scripts/effects/counterstance')
        local effect = stubEffect(50)
        script.onEffectGain(stubTarget({ isMob = false }), effect)
        assert(effect.mods[xi.mod.COUNTER] == 50)
        assert(effect.mods[xi.mod.ATTP] == nil)

        effect = stubEffect(50)
        script.onEffectGain(stubTarget({ isMob = true, species = xi.mobSpecies.BUGBEAR }), effect)
        assert(effect.mods[xi.mod.ATTP] == 15)
        assert(effect.mods[xi.mod.COUNTER] == 50)
    end)

    it('focus and dodge scale with active MNK level plus power', function()
        -- Patch utils.getActiveJobLevel for this test
        local orig = utils.getActiveJobLevel
        utils.getActiveJobLevel = function(target, job)
            return 75
        end
        local focus = require('scripts/effects/focus')
        local effect = stubEffect(10)
        focus.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ACC] == 86) -- 76+10
        assert(effect.mods[xi.mod.CRITHITRATE] == 15)

        local dodge = require('scripts/effects/dodge')
        effect = stubEffect(10)
        dodge.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.EVA] == 86)
        assert(effect.mods[xi.mod.ADDITIVE_GUARD] == 15)
        utils.getActiveJobLevel = orig
    end)

    it('footwork sneak attack trick attack and spirit surge apply expected mods', function()
        local footwork = require('scripts/effects/footwork')
        local target = stubTarget({ jp = { [xi.jp.FOOTWORK_EFFECT] = 5 } })
        local effect = stubEffect(20)
        footwork.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.KICK_ATTACK_RATE] == 20)
        assert(effect.mods[xi.mod.KICK_DMG] == 25)

        local sa = require('scripts/effects/sneak_attack')
        target = stubTarget({ jp = { [xi.jp.SNEAK_ATTACK_EFFECT] = 7 } })
        effect = stubEffect()
        sa.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.SNEAK_ATK_DEX] == 7)

        local ta = require('scripts/effects/trick_attack')
        target = stubTarget({ jp = { [xi.jp.TRICK_ATTACK_EFFECT] = 4 } })
        effect = stubEffect()
        ta.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.TRICK_ATK_AGI] == 4)

        local ss = require('scripts/effects/spirit_surge')
        target = stubTarget({ jp = { [xi.jp.SPIRIT_SURGE_EFFECT] = 3 } })
        effect = stubEffect(500, 40)
        ss.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.HP] == 500)
        assert(effect.mods[xi.mod.STR] == 40)
        assert(effect.mods[xi.mod.ACC] == 50)
        assert(effect.mods[xi.mod.ATTP] == 25)
        assert(effect.mods[xi.mod.DEFP] == 25)
        assert(effect.mods[xi.mod.HASTE_ABILITY] == 2500)
        assert(effect.mods[xi.mod.MAIN_DMG_RATING] == 3)
    end)

    it('spur and charm apply storetp/att and optional regen down', function()
        local spur = require('scripts/effects/spur')
        local effect = stubEffect(10, 20)
        spur.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.STORETP] == 10)
        assert(effect.mods[xi.mod.ATT] == 20)

        local charm = require('scripts/effects/charm')
        effect = stubEffect(0, 5)
        charm.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.REGEN_DOWN] == 5)
    end)

    it('formless strikes feint and hide are empty', function()
        for _, name in ipairs({ 'formless_strikes', 'feint', 'hide' }) do
            local script = require('scripts/effects/' .. name)
            local effect = stubEffect(1)
            script.onEffectGain(stubTarget(), effect)
            local count = 0
            for _ in pairs(effect.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
