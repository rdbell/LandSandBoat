require('scripts/actions/mobskills/activate')
describe('Activate mob skill', function()
    it('delegates pet skill check and spawns pet', function()
        local skill = require('scripts/actions/mobskills/activate')
        local petCheck = xi.pet.onMobSkillCheck
        local spawnPet = xi.pet.spawnPet
        local spawned = false
        xi.pet.onMobSkillCheck = function() return 0 end
        xi.pet.spawnPet = function(mob, pet, sk)
            spawned = true
            assert(pet == nil)
        end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, {}, {}, {}) == 0)
        assert(spawned)
        xi.pet.onMobSkillCheck = petCheck
        xi.pet.spawnPet = spawnPet
    end)
end)
