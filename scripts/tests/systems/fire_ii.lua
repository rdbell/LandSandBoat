describe('Fire II mob skill', function()
    it('uses its magical Fire plan and damages only after processing', function()
        local fireII = require('scripts/actions/mobskills/fire_ii')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(fireII.onMobSkillCheck(target, mob, {}) == 0)
        assert(fireII.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(fireII.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
