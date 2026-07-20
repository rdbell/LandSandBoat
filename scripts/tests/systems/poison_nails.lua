require('scripts/actions/mobskills/poison_nails')

describe('Poison Nails mob skill', function()
    it('uses its piercing physical plan and applies Poison after processing', function()
        local nails = require('scripts/actions/mobskills/poison_nails')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(nails.onMobSkillCheck(target, mob, {}) == 0 and nails.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2)
        assert(params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        nails.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(statusParams[3] == xi.effect.POISON and statusParams[4] == 1 and statusParams[5] == 3 and statusParams[6] == 60)
    end)
end)
