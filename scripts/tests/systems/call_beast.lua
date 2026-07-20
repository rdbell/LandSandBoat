require('scripts/actions/mobskills/call_beast')
describe('Call Beast mob skill', function()
    it('defers skill check to pet host and spawns pet', function()
        local skill = require('scripts/actions/mobskills/call_beast')
        local petCheck = xi.pet.onMobSkillCheck
        local spawnPet = xi.pet.spawnPet
        local spawned = false
        xi.pet.onMobSkillCheck = function() return 1 end
        assert(skill.onMobSkillCheck({}, {}, {}) == 1)
        xi.pet.onMobSkillCheck = function() return 0 end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        xi.pet.spawnPet = function(mob, job, sk)
            spawned = (job == nil)
        end
        assert(skill.onMobWeaponSkill({}, {}, {}, {}) == 0)
        assert(spawned)
        xi.pet.onMobSkillCheck = petCheck
        xi.pet.spawnPet = spawnPet
    end)
end)
