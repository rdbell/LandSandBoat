require('scripts/actions/mobskills/peacebreaker')

describe('Peacebreaker mob skill', function()
    it('uses its blunt physical plan and applies Magic Defense Down after processing', function()
        local peacebreaker = require('scripts/actions/mobskills/peacebreaker')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(peacebreaker.onMobSkillCheck(target, mob, {}) == 0 and peacebreaker.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        peacebreaker.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(statusParams[3] == xi.effect.MAGIC_DEF_DOWN and statusParams[4] == 50 and statusParams[5] == 0 and statusParams[6] == 60)
    end)
end)
