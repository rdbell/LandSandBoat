require('scripts/globals/job_utils/geomancer')

describe('Geomancer Luopan spawn host', function()
    it('creates the resolved aura, stores state, and applies Bolster and Blaze of Glory adjustments', function()
        local geoPotency, aura, modelID, hp, mods = nil, nil, nil, nil, {}
        local mpCost, auraSize, blazeRemoved, spawned
        local luopan = {
            setLocalVar = function(_, _, value) geoPotency = value end,
            addStatusEffect = function(_, _, options) aura = options end,
            setModelId = function(_, value) modelID = value end,
            getMaxHP = function() return 1000 end,
            setHP = function(_, value) hp = value end,
            getMainLvl = function() return 99 end,
            addMod = function(_, mod, value) mods[mod] = value end,
        }
        local player = {
            getPet = function() return luopan end,
            getSkillLevel = function() return 0 end,
            getMod = function() return 0 end,
            getObjType = function() return xi.objType.PC end,
            getMaxGearMod = function() return 0 end,
            getEquipID = function() return 0 end,
            getWeaponSkillType = function() return 0 end,
            hasStatusEffect = function(_, effect)
                return effect == xi.effect.BOLSTER or effect == xi.effect.BLAZE_OF_GLORY or effect == xi.effect.WIDENED_COMPASS
            end,
            setMod = function(_, _, value) auraSize = value end,
            setLocalVar = function(_, _, value) mpCost = value end,
            getJobPointLevel = function() return 3 end,
            delStatusEffect = function(_, effect) blazeRemoved = effect end,
        }
        local spell = {
            getID = function() return xi.magic.spell.GEO_REGEN end,
            getMPCost = function() return 12 end,
        }
        local spawnPet = xi.pet.spawnPet
        xi.pet.spawnPet = function() spawned = true end

        xi.job_utils.geomancer.spawnLuopan(player, {}, spell)
        xi.pet.spawnPet = spawnPet

        assert(spawned and geoPotency == 1 and aura.subPower == 2 and aura.subType == xi.effect.GEO_REGEN)
        assert(mpCost == 12 and modelID == 2856 and auraSize == 625)
        assert(blazeRemoved == xi.effect.BLAZE_OF_GLORY and hp == 530)
        assert(mods[xi.mod.REGEN_DOWN] == 21 and mods[xi.mod.DMG] == -5000)
    end)
end)
