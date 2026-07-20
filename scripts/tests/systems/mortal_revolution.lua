require('scripts/actions/mobskills/mortal_revolution')

describe('Mortal Revolution mob skill', function()
    it('uses its Blunt physical plan and damages only after processing', function()
        local rev = require('scripts/actions/mobskills/mortal_revolution')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(rev.onMobSkillCheck(target, mob, skill) == 0 and rev.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        rev.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
