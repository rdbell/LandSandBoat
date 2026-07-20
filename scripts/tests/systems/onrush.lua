require('scripts/actions/mobskills/onrush')

describe('Onrush mob skill', function()
    it('uses its H2H physical plan with crit and damages only after processing', function()
        local onrush = require('scripts/actions/mobskills/onrush')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(onrush.onMobSkillCheck(target, mob, {}) == 0 and onrush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.canCrit)
        assert(params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        onrush.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND)
    end)
end)
