require('scripts/actions/mobskills/call_wyvern')
describe('Call Wyvern mob skill', function()
    it('defers skill check to pet host and spawns pet', function()
        local skill = require('scripts/actions/mobskills/call_wyvern')
        local petCheck, spawnPet = xi.pet.onMobSkillCheck, xi.pet.spawnPet
        local spawned = false
        xi.pet.onMobSkillCheck = function() return 1 end
        assert(skill.onMobSkillCheck({}, {}, {}) == 1)
        xi.pet.onMobSkillCheck = function() return 0 end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        xi.pet.spawnPet = function(_, job, _) spawned = (job == nil) end
        assert(skill.onMobWeaponSkill({}, {}, {}, {}) == 0)
        assert(spawned)
        xi.pet.onMobSkillCheck = petCheck
        xi.pet.spawnPet = spawnPet
    end)
end)
