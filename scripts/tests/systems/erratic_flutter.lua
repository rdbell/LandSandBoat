require('scripts/actions/mobskills/erratic_flutter')

describe('Erratic Flutter mob skill', function()
    it('uses its Fire magical plan, gates damage, and always applies self Haste', function()
        local move, process, buff = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobBuffMove
        local params, damage, applied = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobBuffMove = function(...) applied = { ... } end
        local skill = require('scripts/actions/mobskills/erratic_flutter')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2.75 and params.fTP[2] == 2.75 and params.fTP[3] == 2.75)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and applied[2] == xi.effect.HASTE and applied[3] == 4500 and applied[4] == 0 and applied[5] == 180)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobBuffMove = move, process, buff
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
