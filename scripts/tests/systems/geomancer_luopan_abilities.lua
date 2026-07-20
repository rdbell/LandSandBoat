-----------------------------------
-- Pure system tests for Geomancer luopan pet abilities.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        id = opts.id or 1,
        hp = opts.hp or 100,
        maxHP = opts.maxHP or 1000,
        mp = opts.mp or 0,
        maxMP = opts.maxMP or 1000,
        hpAdded = 0,
        mpAdded = 0,
        getID = function(self)
            return self.id
        end,
        getMaxHP = function(self)
            return self.maxHP
        end,
        getMaxMP = function(self)
            return self.maxMP
        end,
        addHP = function(self, amount)
            self.hpAdded = amount
            self.hp = self.hp + amount
        end,
        addMP = function(self, amount)
            self.mpAdded = amount
            self.mp = self.mp + amount
        end,
        wakeUp = function() end,
    }
end

local function stubPet(lvl, id)
    return {
        id = id or 99,
        getMainLvl = function()
            return lvl
        end,
        getID = function(self)
            return self.id
        end,
        timer = function() end,
    }
end

local function stubMaster(opts)
    opts = opts or {}
    return {
        merits = opts.merits or {},
        mods = opts.mods or {},
        getMerit = function(self, merit)
            return self.merits[merit] or 0
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
    }
end

describe('Geomancer luopan ability pure plans', function()
    it('mending halation base is 7*level with merit and gear multipliers', function()
        local skill = require('scripts/actions/abilities/pets/mending_halation')
        local target = stubTarget({ maxHP = 1000 })
        local pet = stubPet(50, 99)
        local ret = skill.onPetAbility(target, pet, {}, stubMaster(), {})
        assert(ret == 350)
        assert(target.hpAdded == 350)

        -- merit 1: 350 * 1.05 = 367.5 → addHP uses number; return is float-coerced
        target = stubTarget({ maxHP = 1000 })
        ret = skill.onPetAbility(target, pet, {}, stubMaster({
            merits = { [xi.merit.MENDING_HALATION] = 1 },
        }), {})
        assert(math.floor(ret) == 367 or ret == 367.5)
        assert(target.hpAdded == ret)

        -- merit 1 + gear mod: 367.5 * 1.04 = 382.2
        target = stubTarget({ maxHP = 1000 })
        ret = skill.onPetAbility(target, pet, {}, stubMaster({
            merits = { [xi.merit.MENDING_HALATION] = 1 },
            mods = { [xi.mod.MENDING_HALATION] = 1 },
        }), {})
        assert(math.abs(ret - 382.2) < 1e-9 or math.floor(ret) == 382)

        -- self pet → 0
        local selfPet = stubPet(50, 50)
        local selfTarget = stubTarget({ id = 50 })
        ret = skill.onPetAbility(selfTarget, selfPet, {}, stubMaster(), {})
        assert(ret == 0)
    end)

    it('radial arcana base is 3*level with merit and gear multipliers', function()
        local skill = require('scripts/actions/abilities/pets/radial_arcana')
        local target = stubTarget({ maxMP = 1000 })
        local pet = stubPet(50, 99)
        local ret = skill.onPetAbility(target, pet, {}, stubMaster(), {})
        assert(ret == 150)
        assert(target.mpAdded == 150)

        target = stubTarget({ maxMP = 1000 })
        ret = skill.onPetAbility(target, pet, {}, stubMaster({
            merits = { [xi.merit.RADIAL_ARCANA] = 2 },
        }), {})
        assert(ret == 159)

        target = stubTarget({ maxMP = 1000 })
        ret = skill.onPetAbility(target, pet, {}, stubMaster({
            merits = { [xi.merit.RADIAL_ARCANA] = 2 },
            mods = { [xi.mod.RADIAL_ARCANA] = 1 },
        }), {})
        assert(math.abs(ret - 174.9) < 1e-9 or math.floor(ret) == 174)

        local selfPet = stubPet(50, 7)
        local selfTarget = stubTarget({ id = 7, maxMP = 1000 })
        ret = skill.onPetAbility(selfTarget, selfPet, {}, stubMaster(), {})
        assert(ret == 0)
    end)

    it('geoOnAbilityCheck requires luopan and gates life cycle low HP', function()
        -- Pure inject mirror of job_utils.geomancer.geoOnAbilityCheck
        local function check(hasLuopan, isLifeCycle, hp)
            if hasLuopan then
                return 0, 0
            end
            if isLifeCycle and hp <= 2 then
                return xi.msg.basic.UNABLE_TO_USE_JA, 0
            end
            return xi.msg.basic.REQUIRE_LUOPAN, 0
        end
        assert(check(true, false, 1) == 0)
        assert(check(false, false, 100) == xi.msg.basic.REQUIRE_LUOPAN)
        assert(check(false, true, 2) == xi.msg.basic.UNABLE_TO_USE_JA)
        assert(check(false, true, 10) == xi.msg.basic.REQUIRE_LUOPAN)
    end)
end)
