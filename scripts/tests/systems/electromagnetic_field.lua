require('scripts/actions/mobskills/electromagnetic_field')

describe('Electromagnetic Field mob skill', function()
    it('uses its Thunder magical plan and damages only after processing succeeds', function()
        local field = require('scripts/actions/mobskills/electromagnetic_field')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER } end
        xi.mobskills.processDamage = function() return false end

        assert(field.onMobSkillCheck({}, {}, {}) == 0)
        assert(field.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(field.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
    end)
end)
