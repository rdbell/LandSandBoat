-----------------------------------
-- Pure system tests for Summoner pet support blood pacts.
-----------------------------------

local function stubAction(primaryID)
    return {
        getPrimaryTargetID = function()
            return primaryID
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
    return {
        id = opts.id or 1,
        hp = opts.hp or 100,
        maxHP = opts.maxHP or 1000,
        isPCFlag = opts.isPC ~= false,
        alive = opts.alive ~= false,
        added = {},
        dels = {},
        getID = function(self)
            return self.id
        end,
        isPC = function(self)
            return self.isPCFlag
        end,
        isAlive = function(self)
            return self.alive
        end,
        getHP = function(self)
            return self.hp
        end,
        getMaxHP = function(self)
            return self.maxHP
        end,
        getMainLvl = function(self)
            return opts.mainLvl or 75
        end,
        addHP = function(self, amount)
            self.hp = self.hp + amount
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.dels, effect)
        end,
        addStatusEffect = function(self, effect, params)
            table.insert(self.added, { effect = effect, params = params })
            return true
        end,
        sendRaise = function(self, level)
            self.raiseLevel = level
        end,
        wakeUp = function() end,
    }
end

local function stubSummoner(skill)
    return {
        getSkillLevel = function()
            return skill
        end,
    }
end

local function stubPet(lvl, tp)
    return {
        getMainLvl = function()
            return lvl
        end,
        getTP = function()
            return tp or 1000
        end,
    }
end

local origCan, origUse, origBuff
local function stubHosts()
    origCan = xi.job_utils.summoner.canUseBloodPact
    origUse = xi.job_utils.summoner.onUseBloodPact
    origBuff = xi.mobskills.mobBuffMove
    xi.job_utils.summoner.canUseBloodPact = function()
        return 0, 0
    end
    xi.job_utils.summoner.onUseBloodPact = function() end
    xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
        target.buff = { effect = effect, power = power, tick = tick, duration = duration }
        return 0
    end
end

local function restoreHosts()
    xi.job_utils.summoner.canUseBloodPact = origCan
    xi.job_utils.summoner.onUseBloodPact = origUse
    xi.mobskills.mobBuffMove = origBuff
end

describe('Summoner pet support pure plans', function()
    before_each(function()
        stubHosts()
    end)
    after_each(function()
        restoreHosts()
    end)

    it('noctoshield applies Phalanx 13 with 180+bonus duration', function()
        local skill = require('scripts/actions/abilities/pets/noctoshield')
        local target = stubTarget()
        skill.onPetAbility(target, {}, stubPetSkill(), stubSummoner(400), stubAction(1))
        assert(target.added[1].effect == xi.effect.PHALANX)
        assert(target.added[1].params.power == 13)
        assert(target.added[1].params.duration == 280)
    end)

    it('shining ruby applies power 1 duration 180 at skill 300', function()
        local skill = require('scripts/actions/abilities/pets/shining_ruby')
        local target = stubTarget()
        assert(skill.onPetAbility(target, {}, stubPetSkill(), stubSummoner(300), stubAction(1)) == xi.effect.SHINING_RUBY)
        assert(target.added[1].params.power == 1 and target.added[1].params.duration == 180)
    end)

    it('glittering ruby power is 3 + floor(petLvl/5)', function()
        local skill = require('scripts/actions/abilities/pets/glittering_ruby')
        local target = stubTarget()
        skill.onPetAbility(target, stubPet(50), stubPetSkill(), stubSummoner(0), stubAction(1))
        assert(target.added[1].params.power == 13)
        assert(target.added[1].params.duration == 90)
    end)

    it('whispering wind heals 16 + petLvl*2.5 clamped to missing HP', function()
        local skill = require('scripts/actions/abilities/pets/whispering_wind')
        local target = stubTarget({ hp = 0, maxHP = 1000 })
        local ret = skill.onPetAbility(target, stubPet(40), stubPetSkill(), stubSummoner(0), stubAction(1))
        assert(ret == 116)
        local low = stubTarget({ hp = 995, maxHP = 1000 })
        ret = skill.onPetAbility(low, stubPet(40), stubPetSkill(), stubSummoner(0), stubAction(1))
        assert(ret == 5)
    end)

    it('spring water scales with TP and clears status effects', function()
        local skill = require('scripts/actions/abilities/pets/spring_water')
        local target = stubTarget({ hp = 0, maxHP = 2000 })
        local ret = skill.onPetAbility(target, stubPet(40, 2000), stubPetSkill(), stubSummoner(0), stubAction(1))
        -- (47+120)=167 * 2 = 334
        assert(ret == 334)
        assert(#target.dels >= 6)
    end)

    it('raise ii rejects living or non-PC targets', function()
        local skill = require('scripts/actions/abilities/pets/raise_ii')
        local alive = stubTarget({ isPC = true, alive = true })
        local petskill = stubPetSkill()
        assert(skill.onPetAbility(alive, {}, petskill, stubSummoner(0), stubAction(1)) == 0)
        assert(petskill.getMsg() == xi.msg.basic.NO_EFFECT)
        local dead = stubTarget({ isPC = true, alive = false })
        petskill = stubPetSkill()
        assert(skill.onPetAbility(dead, {}, petskill, stubSummoner(0), stubAction(1)) == 0)
        assert(dead.raiseLevel == 2)
        assert(petskill.getMsg() == xi.msg.basic.NONE)
    end)

    it('reraise ii applies power 2 duration 3600 on PC', function()
        local skill = require('scripts/actions/abilities/pets/reraise_ii')
        local target = stubTarget({ isPC = true })
        assert(skill.onPetAbility(target, {}, stubPetSkill(), stubSummoner(0), stubAction(1)) == xi.effect.RERAISE)
        assert(target.added[1].params.power == 2 and target.added[1].params.duration == 3600)
    end)

    it('rolling thunder potency tables and duration', function()
        -- pure potency formulas (skill cap inject)
        local low = 3 + 6 * 100 / 100
        local high = 5 + 5 * 250 / 100
        assert(low == 9)
        assert(high == 17.5)
        local skill = require('scripts/actions/abilities/pets/rolling_thunder')
        local target = stubTarget()
        -- Stub skill cap to 100
        local orig = xi.data.skillLevel.getSkillCap
        xi.data.skillLevel.getSkillCap = function()
            return 100
        end
        skill.onPetAbility(target, {}, stubPetSkill(), stubSummoner(400), stubAction(1))
        assert(target.buff.effect == xi.effect.ENTHUNDER)
        assert(target.buff.power == 9)
        assert(target.buff.duration == 220)
        xi.data.skillLevel.getSkillCap = orig
    end)
end)
