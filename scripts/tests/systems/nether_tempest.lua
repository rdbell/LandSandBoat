require('scripts/actions/mobskills/nether_tempest')

describe('Nether Tempest mob skill', function()
    it('uses its Dark magical plan and damages only after processing', function()
        local tempest = require('scripts/actions/mobskills/nether_tempest')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end

        assert(tempest.onMobSkillCheck(target, mob, skill) == 0 and tempest.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 75 and params.additiveDamage[1] == 10 and params.additiveDamage[2] == 10 and params.additiveDamage[3] == 10)
        assert(params.fTP[1] == 5 and params.fTP[2] == 5 and params.fTP[3] == 5)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        tempest.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.DARK)
    end)
end)
