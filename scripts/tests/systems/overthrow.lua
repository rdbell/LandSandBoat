require('scripts/actions/mobskills/overthrow')

describe('Overthrow mob skill', function()
    it('requires non-zero animation sub and uses H2H ignore-shadows plan', function()
        local overthrow = require('scripts/actions/mobskills/overthrow')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local animSub, params, damage = 0, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        animSub = 0
        assert(overthrow.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 1
        assert(overthrow.onMobSkillCheck(target, mob, {}) == 0)

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(overthrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        overthrow.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND)
    end)
end)
