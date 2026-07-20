require('scripts/actions/mobskills/erosion_dust')

describe('Erosion Dust mob skill', function()
    it('uses its Fire magical plan and applies Dia only after processed damage', function()
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        local skill = require('scripts/actions/mobskills/erosion_dust')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3.3 and params.fTP[2] == 3.3 and params.fTP[3] == 3.3)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and applied == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
        assert(applied[3] == xi.effect.DIA and applied[4] == 11 and applied[5] == 3 and applied[6] == 30 and applied[8] == 25)
    end)
end)
