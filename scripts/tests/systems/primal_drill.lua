require('scripts/actions/mobskills/primal_drill')

describe('Primal Drill mob skill', function()
    it('uses its piercing physical plan and applies Bind after processing', function()
        local drill = require('scripts/actions/mobskills/primal_drill')
        local move, process, status, random = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        math.random = function(low, high)
            assert(low == 45 and high == 90)
            return 60
        end
        xi.mobskills.processDamage = function() return false end
        assert(drill.onMobSkillCheck(target, mob, {}) == 0 and drill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        drill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, status, random
        assert(damage[1] == 123)
        assert(statusParams[3] == xi.effect.BIND and statusParams[4] == 1 and statusParams[6] == 60)
    end)
end)
