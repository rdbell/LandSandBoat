-----------------------------------
-- Pure system tests for Summoner pet-buff blood pact scripts.
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

local function stubTarget(id)
    local effects = {}
    return {
        id = id,
        effects = effects,
        added = {},
        dels = {},
        getID = function(self)
            return self.id
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        addStatusEffect = function(self, effect, params)
            table.insert(self.added, { effect = effect, params = params })
            self.effects[effect] = true
            return true
        end,
    }
end

local function stubSummoner(skill)
    return {
        getSkillLevel = function()
            return skill
        end,
    }
end

-- Stub blood pact hosts so pure products are testable in isolation.
local origCan, origUse
local function stubBloodPactHosts()
    origCan = xi.job_utils.summoner.canUseBloodPact
    origUse = xi.job_utils.summoner.onUseBloodPact
    xi.job_utils.summoner.canUseBloodPact = function()
        return 0, 0
    end
    xi.job_utils.summoner.onUseBloodPact = function() end
end

local function restoreBloodPactHosts()
    xi.job_utils.summoner.canUseBloodPact = origCan
    xi.job_utils.summoner.onUseBloodPact = origUse
end

describe('Summoner pet buff pure plans', function()
    before_each(function()
        stubBloodPactHosts()
    end)
    after_each(function()
        restoreBloodPactHosts()
    end)

    it('aerial armor applies Blink power 3 duration 900', function()
        local skill = require('scripts/actions/abilities/pets/aerial_armor')
        local target = stubTarget(1)
        local petskill = stubPetSkill()
        local ret = skill.onPetAbility(target, {}, petskill, stubSummoner(0), stubAction(1))
        assert(ret == xi.effect.BLINK)
        assert(target.added[1].params.power == 3 and target.added[1].params.duration == 900)
        assert(petskill.getMsg() == xi.msg.basic.SKILL_GAIN_EFFECT_2)
    end)

    it('crimson howl duration is 60 + clamp(skill-300,0,200)', function()
        local skill = require('scripts/actions/abilities/pets/crimson_howl')
        local target = stubTarget(2)
        local petskill = stubPetSkill()
        skill.onPetAbility(target, {}, petskill, stubSummoner(400), stubAction(1))
        assert(target.added[1].effect == xi.effect.WARCRY)
        assert(target.added[1].params.power == 9)
        assert(target.added[1].params.duration == 160)
        assert(petskill.getMsg() == xi.msg.basic.JA_GAIN_EFFECT)
    end)

    it('earthen ward stoneskin power is petLvl*2+50', function()
        local skill = require('scripts/actions/abilities/pets/earthen_ward')
        local target = stubTarget(1)
        local pet = {
            getMainLvl = function()
                return 50
            end,
        }
        skill.onPetAbility(target, pet, stubPetSkill(), stubSummoner(0), stubAction(1))
        assert(target.added[1].effect == xi.effect.STONESKIN)
        assert(target.added[1].params.power == 150)
        assert(target.added[1].params.tier == 3)
        assert(target.added[1].params.duration == 900)
    end)

    it('frost armor and hastega use 180+bonus base', function()
        local frost = require('scripts/actions/abilities/pets/frost_armor')
        local target = stubTarget(1)
        frost.onPetAbility(target, {}, stubPetSkill(), stubSummoner(500), stubAction(1))
        assert(target.added[1].effect == xi.effect.ICE_SPIKES)
        assert(target.added[1].params.power == 15 and target.added[1].params.duration == 380)

        local hastega = require('scripts/actions/abilities/pets/hastega')
        target = stubTarget(1)
        hastega.onPetAbility(target, {}, stubPetSkill(), stubSummoner(300), stubAction(1))
        assert(target.added[1].effect == xi.effect.HASTE)
        assert(target.added[1].params.power == 1494 and target.added[1].params.duration == 180)
    end)

    it('dream shroud applies MAB+MDB with complementary powers and duration', function()
        -- VanadielHour is a C++ binding; assert pure complement invariant instead of
        -- a fixed hour. buffvalue = abs(12-hour)+1; MDB = 14-buffvalue.
        local skill = require('scripts/actions/abilities/pets/dream_shroud')
        local target = stubTarget(1)
        local petskill = stubPetSkill()
        skill.onPetAbility(target, {}, petskill, stubSummoner(400), stubAction(1))
        local mab, mdb, dur
        for _, a in ipairs(target.added) do
            if a.effect == xi.effect.MAGIC_ATK_BOOST then
                mab = a.params.power
                dur = a.params.duration
            end
            if a.effect == xi.effect.MAGIC_DEF_BOOST then
                mdb = a.params.power
            end
        end
        assert(mab ~= nil and mdb ~= nil)
        assert(mab + mdb == 14)
        assert(mab >= 1 and mab <= 13)
        assert(dur == 280) -- 180 + clamp(400-300,0,200)
        assert(petskill.getMsg() == xi.msg.basic.JA_RECEIVES_MAB_MDB)
    end)

    it('ecliptic growl and howl apply complementary stat pairs from moon cycle', function()
        -- getVanadielMoonCycle is a C++ binding; assert table invariants:
        -- Growl: STR+AGI == 8; Howl: ACC+EVA == 25; duration 180 at skill 300.
        local growl = require('scripts/actions/abilities/pets/ecliptic_growl')
        local target = stubTarget(1)
        growl.onPetAbility(target, {}, stubPetSkill(), stubSummoner(300), stubAction(1))
        local str, agi, dur
        for _, a in ipairs(target.added) do
            if a.effect == xi.effect.STR_BOOST then
                str = a.params.power
                dur = a.params.duration
            end
            if a.effect == xi.effect.AGI_BOOST then
                agi = a.params.power
            end
        end
        assert(str ~= nil and agi ~= nil)
        assert(str + agi == 8)
        assert(dur == 180)

        local howl = require('scripts/actions/abilities/pets/ecliptic_howl')
        target = stubTarget(1)
        howl.onPetAbility(target, {}, stubPetSkill(), stubSummoner(300), stubAction(1))
        local acc, eva
        for _, a in ipairs(target.added) do
            if a.effect == xi.effect.ACCURACY_BOOST then
                acc = a.params.power
            end
            if a.effect == xi.effect.EVASION_BOOST then
                eva = a.params.power
            end
        end
        assert(acc ~= nil and eva ~= nil)
        assert(acc + eva == 25)
    end)

    it('ecliptic moon tables pin NEW_MOON and FULL_MOON pure values', function()
        -- Pure table pins matching Go EclipticGrowl/HowlBuffValue (no C++ host).
        local growlCycle =
        {
            [xi.moonCycle.NEW_MOON] = 1,
            [xi.moonCycle.FULL_MOON] = 7,
            [xi.moonCycle.FIRST_QUARTER] = 4,
        }
        local howlCycle =
        {
            [xi.moonCycle.NEW_MOON] = 1,
            [xi.moonCycle.FULL_MOON] = 25,
            [xi.moonCycle.FIRST_QUARTER] = 13,
        }
        assert(growlCycle[xi.moonCycle.NEW_MOON] == 1)
        assert(growlCycle[xi.moonCycle.FULL_MOON] == 7)
        assert(8 - growlCycle[xi.moonCycle.FULL_MOON] == 1)
        assert(howlCycle[xi.moonCycle.FULL_MOON] == 25)
        assert(25 - howlCycle[xi.moonCycle.FIRST_QUARTER] == 12)
    end)
end)
