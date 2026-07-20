require('scripts/actions/mobskills/electrocharge')

describe('Electrocharge mob skill', function()
    it('uses its non-elemental magical plan and damages only after processing succeeds', function()
        local electrocharge = require('scripts/actions/mobskills/electrocharge')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ELEMENTAL }
        end
        xi.mobskills.processDamage = function() return false end

        assert(electrocharge.onMobSkillCheck({}, {}, {}) == 0)
        assert(electrocharge.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 0.5 and params.fTP[2] == 0.5 and params.fTP[3] == 0.5)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ELEMENTAL)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(electrocharge.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ELEMENTAL)
    end)
end)
