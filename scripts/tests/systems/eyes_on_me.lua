require('scripts/actions/mobskills/eyes_on_me')

describe('Eyes on Me mob skill', function()
    it('uses its Dark magical plan, raises fTP for NMs, and gates damage on processing', function()
        local eyesOnMe = require('scripts/actions/mobskills/eyes_on_me')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end, isNM = function() return false end }
        local nm = { getMainLvl = function() return 1 end, isNM = function() return true end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK } end
        xi.mobskills.processDamage = function() return false end
        assert(eyesOnMe.onMobSkillCheck(target, mob, {}) == 0)
        assert(eyesOnMe.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 5 and params.fTP[2] == 5 and params.fTP[3] == 5)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and params.skipDamageAdjustment and damage == nil)
        assert(eyesOnMe.onMobWeaponSkill(nm, target, {}, {}) == 123)
        assert(params.baseDamage == 3 and params.fTP[1] == 7 and params.fTP[2] == 7 and params.fTP[3] == 7)
        xi.mobskills.processDamage = function() return true end
        assert(eyesOnMe.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
