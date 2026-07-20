require('scripts/actions/mobskills/doctors_orders')

describe("Doctor's Orders mob skill", function()
    it('uses its main-level magical plan and applies damage only after processing', function()
        local orders = require('scripts/actions/mobskills/doctors_orders')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.NONE, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(orders.onMobSkillCheck({}, {}, {}) == 0)
        assert(orders.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2.8 and params.fTP[2] == 2.8 and params.fTP[3] == 2.8)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.NONE and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(orders.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.NONE and damage[4] == xi.damageType.NONE)
    end)
end)
