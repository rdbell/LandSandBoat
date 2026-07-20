require('scripts/actions/mobskills/empty_salvation')

describe('Empty Salvation mob skill', function()
    it('uses its Dark magical plan and dispels up to three effects independently of damage processing', function()
        local salvation = require('scripts/actions/mobskills/empty_salvation')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, dispels = nil, nil, 0
        local mob = { getMainLvl = function() return 75 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            dispelStatusEffect = function(_, flag) dispels = dispels + 1; assert(flag == xi.effectFlag.DISPELABLE); return dispels < 3 end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK } end
        xi.mobskills.processDamage = function() return false end

        assert(salvation.onMobSkillCheck({}, {}, {}) == 0)
        assert(salvation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and dispels == 3)

        dispels = 0
        target.dispelStatusEffect = function(_, _) dispels = dispels + 1; return false end
        xi.mobskills.processDamage = function() return true end
        assert(salvation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK and dispels == 1)
    end)
end)
