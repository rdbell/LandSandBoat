-----------------------------------
-- Pure system tests for pet-mirrored status-effect scripts:
-- Allied Tags, Confrontation, Pennant, Reive Mark, Battlefield.
-----------------------------------

local function stubPet(opts)
    opts = opts or {}
    return {
        copied = false,
        deleted = {},
        leftBF = nil,
        copyStatusEffect = function(self, effect)
            self.copied = true
            self.copiedEffect = effect
        end,
        delStatusEffect = function(self, effectID)
            table.insert(self.deleted, effectID)
        end,
        leaveBattlefield = function(self, arg)
            self.leftBF = arg
        end,
    }
end

local function stubTarget(opts)
    opts = opts or {}
    local pet = opts.pet
    return {
        pet = pet,
        objType = opts.objType or xi.objType.PC,
        localVars = {},
        trustsCleared = false,
        getPet = function(self)
            return self.pet
        end,
        getObjType = function(self)
            return self.objType
        end,
        clearTrusts = function(self)
            self.trustsCleared = true
        end,
        setLocalVar = function(self, key, val)
            self.localVars[key] = val
        end,
    }
end

local function stubEffect()
    return { power = 1 }
end

describe('Pet status-effect mirror pure plans', function()
    it('simple mirrors copy to pet on gain and del on lose', function()
        local cases = {
            { 'scripts/effects/allied_tags', xi.effect.ALLIED_TAGS },
            { 'scripts/effects/confrontation', xi.effect.CONFRONTATION },
            { 'scripts/effects/pennant', xi.effect.PENNANT },
            { 'scripts/effects/reive_mark', xi.effect.REIVE_MARK },
        }
        for _, c in ipairs(cases) do
            local script = require(c[1])
            local pet = stubPet()
            local target = stubTarget({ pet = pet })
            script.onEffectGain(target, stubEffect())
            assert(pet.copied, c[1] .. ' should copy')
            script.onEffectLose(target, stubEffect())
            assert(pet.deleted[1] == c[2], c[1] .. ' should del pet effect')
        end
    end)

    it('simple mirrors are no-ops without a pet', function()
        local script = require('scripts/effects/pennant')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        script.onEffectLose(target, stubEffect())
        -- no error is success
    end)

    it('battlefield gain copies pet and clears trusts on PC', function()
        local script = require('scripts/effects/battlefield')
        local pet = stubPet()
        local target = stubTarget({ pet = pet, objType = xi.objType.PC })
        script.onEffectGain(target, stubEffect())
        assert(pet.copied)
        assert(target.trustsCleared)
    end)

    it('battlefield gain skips clearTrusts for non-PC', function()
        local script = require('scripts/effects/battlefield')
        local pet = stubPet()
        local target = stubTarget({ pet = pet, objType = xi.objType.MOB })
        script.onEffectGain(target, stubEffect())
        assert(pet.copied)
        assert(not target.trustsCleared)
    end)

    it('battlefield lose dels pet effect, leaveBattlefield(1), clears area var', function()
        local script = require('scripts/effects/battlefield')
        local pet = stubPet()
        local target = stubTarget({ pet = pet })
        script.onEffectLose(target, stubEffect())
        assert(pet.deleted[1] == xi.effect.BATTLEFIELD)
        assert(pet.leftBF == 1)
        assert(target.localVars['[battlefield]area'] == 0)
    end)

    it('battlefield lose without pet still clears area localVar', function()
        local script = require('scripts/effects/battlefield')
        local target = stubTarget()
        script.onEffectLose(target, stubEffect())
        assert(target.localVars['[battlefield]area'] == 0)
    end)

    it('effect IDs match enums', function()
        assert(xi.effect.BATTLEFIELD == 254)
        assert(xi.effect.ALLIED_TAGS == 267)
        assert(xi.effect.CONFRONTATION == 276)
        assert(xi.effect.PENNANT == 292)
        assert(xi.effect.REIVE_MARK == 511)
    end)
end)
