require('scripts/actions/mobskills/one_inch_punch')

describe('One Inch Punch mob skill', function()
    it('uses its twofold H2H plan with ignore-defense and damages only after processing', function()
        local punch = require('scripts/actions/mobskills/one_inch_punch')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(punch.onMobSkillCheck(target, mob, {}) == 0 and punch.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.ignoreDefense[1] == 0 and params.ignoreDefense[2] == 0.3 and params.ignoreDefense[3] == 0.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        punch.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND)
    end)
end)
