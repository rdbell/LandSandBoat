require('scripts/actions/mobskills/di_bite_attack')

describe('Di Bite Attack mob skill', function()
    it('uses its normal-hit physical plan and applies sampled Weight after processing', function()
        local params, damage, weight = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local random = math.random
        math.random = function(minimum, maximum) assert(minimum == 3 and maximum == 15); return 9 end
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) weight = { ... } end

        local bite = require('scripts/actions/mobskills/di_bite_attack')
        assert(bite.onMobSkillCheck({}, {}, {}) == 0)
        assert(bite.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.primaryMessage == xi.msg.basic.HIT_DMG)
        assert(damage == nil and weight == nil)

        xi.mobskills.processDamage = function() return true end
        assert(bite.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        math.random = random
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(weight[3] == xi.effect.WEIGHT and weight[4] == 50 and weight[5] == 0 and weight[6] == 9)
    end)
end)
