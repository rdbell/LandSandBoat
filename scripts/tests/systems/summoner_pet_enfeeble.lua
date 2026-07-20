-----------------------------------
-- Pure system tests for Summoner pet heal/enfeeble blood pacts.
-----------------------------------

local function stubAction(primaryID)
    return {
        getPrimaryTargetID = function()
            return primaryID or 1
        end,
    }
end

local function stubPetSkill()
    local msg = nil
    return {
        setMsg = function(_, m)
            msg = m
        end,
        getMsg = function()
            return msg
        end,
    }
end

local function stubTarget(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    return {
        id = opts.id or 1,
        hp = opts.hp or 0,
        maxHP = opts.maxHP or 1000,
        mainLvl = opts.mainLvl or 50,
        isPCFlag = opts.isPC ~= false,
        dead = opts.dead == true,
        effects = effects,
        added = {},
        dels = {},
        enmity = nil,
        raiseLevel = nil,
        getID = function(self)
            return self.id
        end,
        getHP = function(self)
            return self.hp
        end,
        getMaxHP = function(self)
            return self.maxHP
        end,
        getMainLvl = function(self)
            return self.mainLvl
        end,
        isPC = function(self)
            return self.isPCFlag
        end,
        isDead = function(self)
            return self.dead
        end,
        isAlive = function(self)
            return not self.dead
        end,
        addHP = function(self, amount)
            self.hp = self.hp + amount
        end,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] ~= nil
        end,
        getStatusEffect = function(self, effect)
            if self.effects[effect] then
                return {
                    addEffectFlag = function() end,
                }
            end
            return nil
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        addStatusEffect = function(self, effect, params)
            if opts.failAdd then
                return false
            end
            table.insert(self.added, { effect = effect, params = params })
            self.effects[effect] = true
            return true
        end,
        addEnmity = function(self, pet, ce, ve)
            self.enmity = { pet = pet, ce = ce, ve = ve }
        end,
        updateEnmity = function() end,
        sendRaise = function(self, level)
            self.raiseLevel = level
        end,
        setTP = function(self, tp)
            self.tp = tp
        end,
        getStat = function()
            return opts.chr or 50
        end,
    }
end

local function stubPet(opts)
    opts = opts or {}
    return {
        mainLvl = opts.mainLvl or 40,
        tp = opts.tp or 1200,
        getMainLvl = function(self)
            return self.mainLvl
        end,
        getTP = function(self)
            return self.tp
        end,
        getStat = function()
            return opts.chr or 60
        end,
    }
end

local function stubSummoner(opts)
    opts = opts or {}
    return {
        mp = opts.mp or 500,
        mods = opts.mods or {},
        isPCFlag = opts.isPC ~= false,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        setMP = function(self, v)
            self.mp = v
        end,
        isPC = function(self)
            return self.isPCFlag
        end,
    }
end

local origCan, origUse, origImmune, origResistant, origNullified, origSkillOverCap, origResist
local function stubHosts()
    origCan = xi.job_utils.summoner.canUseBloodPact
    origUse = xi.job_utils.summoner.onUseBloodPact
    origImmune = xi.data.statusEffect.isTargetImmune
    origResistant = xi.data.statusEffect.isTargetResistant
    origNullified = xi.data.statusEffect.isEffectNullified
    origSkillOverCap = xi.summon.getSummoningSkillOverCap
    origResist = xi.combat.magicHitRate.calculateResistRate
    xi.job_utils.summoner.canUseBloodPact = function()
        return 0, 0
    end
    xi.job_utils.summoner.onUseBloodPact = function() end
    xi.data.statusEffect.isTargetImmune = function()
        return false
    end
    xi.data.statusEffect.isTargetResistant = function()
        return false
    end
    xi.data.statusEffect.isEffectNullified = function()
        return false
    end
    xi.summon.getSummoningSkillOverCap = function()
        return 0
    end
    xi.combat.magicHitRate.calculateResistRate = function()
        return 1.0
    end
end

local function restoreHosts()
    xi.job_utils.summoner.canUseBloodPact = origCan
    xi.job_utils.summoner.onUseBloodPact = origUse
    xi.data.statusEffect.isTargetImmune = origImmune
    xi.data.statusEffect.isTargetResistant = origResistant
    xi.data.statusEffect.isEffectNullified = origNullified
    xi.summon.getSummoningSkillOverCap = origSkillOverCap
    xi.combat.magicHitRate.calculateResistRate = origResist
end

describe('Summoner pet heal/enfeeble pure plans', function()
    before_each(function()
        stubHosts()
    end)
    after_each(function()
        restoreHosts()
    end)

    it('healing ruby uses low and high pet-level formulas with missing-HP clamp', function()
        local skill = require('scripts/actions/abilities/pets/healing_ruby')
        -- pet 30: 14 + 50 + 1200/12 = 164
        local target = stubTarget({ mainLvl = 50, hp = 0, maxHP = 1000 })
        local ret = skill.onPetAbility(target, stubPet({ mainLvl = 30, tp = 1200 }), stubPetSkill(), stubSummoner(), stubAction())
        assert(ret == 164)
        -- clamp
        target = stubTarget({ mainLvl = 50, hp = 990, maxHP = 1000 })
        ret = skill.onPetAbility(target, stubPet({ mainLvl = 30, tp = 1200 }), stubPetSkill(), stubSummoner(), stubAction())
        assert(ret == 10)
        -- pet 40 high formula: 44+30 + 100*(3-1) = 274
        target = stubTarget({ mainLvl = 50, hp = 0, maxHP = 1000 })
        ret = skill.onPetAbility(target, stubPet({ mainLvl = 40, tp = 1200 }), stubPetSkill(), stubSummoner(), stubAction())
        assert(ret == 274)
    end)

    it('healing ruby ii is 28 + petLvl*4', function()
        local skill = require('scripts/actions/abilities/pets/healing_ruby_ii')
        local target = stubTarget({ hp = 0, maxHP = 1000 })
        local ret = skill.onPetAbility(target, stubPet({ mainLvl = 40 }), stubPetSkill(), stubSummoner(), stubAction())
        assert(ret == 188)
    end)

    it('lunar cry applies complementary ACC/EVA down from moon cycle', function()
        local skill = require('scripts/actions/abilities/pets/lunar_cry')
        local target = stubTarget()
        skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner(), stubAction())
        local acc, eva
        for _, a in ipairs(target.added) do
            if a.effect == xi.effect.ACCURACY_DOWN then
                acc = a.params.power
            end
            if a.effect == xi.effect.EVASION_DOWN then
                eva = a.params.power
            end
        end
        assert(acc ~= nil and eva ~= nil)
        assert(acc + eva == 32)
        assert(target.added[1].params.duration == 180)
    end)

    it('tidal roar applies Attack Down 25/60 or no-effect when present', function()
        local skill = require('scripts/actions/abilities/pets/tidal_roar')
        local target = stubTarget()
        assert(skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner(), stubAction()) == xi.effect.ATTACK_DOWN)
        assert(target.added[1].params.power == 25 and target.added[1].params.duration == 60)

        local blocked = stubTarget({ effects = { [xi.effect.ATTACK_DOWN] = true } })
        local petskill = stubPetSkill()
        skill.onPetAbility(blocked, stubPet(), petskill, stubSummoner(), stubAction())
        assert(petskill.getMsg() == xi.msg.basic.JA_NO_EFFECT_2)
        assert(#blocked.added == 0)
    end)

    it('slowga duration is min(180+SUMMONING, 350) with power 3000 tier 3', function()
        local skill = require('scripts/actions/abilities/pets/slowga')
        local target = stubTarget()
        skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner({ mods = { [xi.mod.SUMMONING] = 50 } }), stubAction())
        assert(target.added[1].effect == xi.effect.SLOW)
        assert(target.added[1].params.power == 3000)
        assert(target.added[1].params.tier == 3)
        assert(target.added[1].params.duration == 230)

        target = stubTarget()
        skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner({ mods = { [xi.mod.SUMMONING] = 200 } }), stubAction())
        assert(target.added[1].params.duration == 350)
    end)

    it('sleepga applies floor(90*resist) duration and nullifies when already asleep', function()
        local skill = require('scripts/actions/abilities/pets/sleepga')
        local target = stubTarget()
        assert(skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner(), stubAction()) == xi.effect.SLEEP_I)
        assert(target.added[1].params.power == 1 and target.added[1].params.duration == 90)

        local asleep = stubTarget({ effects = { [xi.effect.SLEEP_I] = true } })
        local petskill = stubPetSkill()
        skill.onPetAbility(asleep, stubPet(), petskill, stubSummoner(), stubAction(1))
        assert(petskill.getMsg() == xi.msg.basic.JA_NO_EFFECT_2)
        assert(#asleep.added == 0)
    end)

    it('mewing lullaby resets TP then applies sleep', function()
        local skill = require('scripts/actions/abilities/pets/mewing_lullaby')
        local target = stubTarget()
        target.tp = 3000
        skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner(), stubAction())
        assert(target.tp == 0)
        assert(target.added[1].effect == xi.effect.SLEEP_I)
        assert(target.added[1].params.duration == 90)
    end)

    it('nightmare applies sleep tier 4 and bio on success', function()
        local skill = require('scripts/actions/abilities/pets/nightmare')
        local target = stubTarget()
        skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner(), stubAction())
        local sleep, bio
        for _, a in ipairs(target.added) do
            if a.effect == xi.effect.SLEEP_I then
                sleep = a
            end
            if a.effect == xi.effect.BIO then
                bio = a
            end
        end
        assert(sleep and sleep.params.subPower == 2 and sleep.params.tier == 4)
        assert(bio and bio.params.power == 2 and bio.params.tick == 3 and bio.params.subPower == 10)
    end)

    it('altana favor raises dead PC or applies reraise 3', function()
        local skill = require('scripts/actions/abilities/pets/altana_s_favor')
        local dead = stubTarget({ isPC = true, dead = true })
        local summoner = stubSummoner({ mp = 500 })
        skill.onPetAbility(dead, stubPet(), stubPetSkill(), summoner, stubAction())
        assert(dead.raiseLevel == 4)
        assert(summoner.mp == 0)

        local living = stubTarget({ isPC = true, dead = false })
        summoner = stubSummoner({ mp = 100 })
        local ret = skill.onPetAbility(living, stubPet(), stubPetSkill(), summoner, stubAction())
        assert(ret == xi.effect.RERAISE)
        assert(living.added[1].params.power == 3)
        assert(summoner.mp == 0)
    end)

    it('soothing current applies Curing Conduit 15/180', function()
        local skill = require('scripts/actions/abilities/pets/soothing_current')
        local target = stubTarget()
        assert(skill.onPetAbility(target, stubPet(), stubPetSkill(), stubSummoner(), stubAction()) == xi.effect.CURING_CONDUIT)
        assert(target.added[1].params.power == 15 and target.added[1].params.duration == 180)
    end)

    it('eerie eye applies silence and optionally amnesia from dual resists', function()
        local skill = require('scripts/actions/abilities/pets/eerie_eye')
        local target = stubTarget({ chr = 40 })
        local ret = skill.onPetAbility(target, stubPet({ chr = 60 }), stubPetSkill(), stubSummoner(), stubAction())
        -- resist 1.0 → silence 30, amnesia 10; return AMNESIA when both land
        assert(ret == xi.effect.AMNESIA or ret == xi.effect.SILENCE)
        assert(#target.added >= 1)
        assert(target.added[1].effect == xi.effect.SILENCE)
        assert(target.added[1].params.duration == 30)
    end)
end)
