require('scripts/actions/mobskills/onslaught')

describe('Onslaught mob skill', function()
    it('uses its Slashing physical plan and applies Accuracy Down only after processing', function()
        local skillObj = require('scripts/actions/mobskills/onslaught')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, acc = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) acc = { ... } end
        assert(skillObj.onMobSkillCheck(target, mob, {}) == 0 and skillObj.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.75 and params.fTP[2] == 2.75 and params.fTP[3] == 2.75)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and acc == nil)
        xi.mobskills.processDamage = function() return true end
        skillObj.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and acc[3] == xi.effect.ACCURACY_DOWN and acc[4] == 30 and acc[5] == 0 and acc[6] == 60)
    end)
end)
