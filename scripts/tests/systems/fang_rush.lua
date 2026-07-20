require('scripts/actions/mobskills/fang_rush')

describe('Fang Rush mob skill', function()
    it('uses its three-hit Slashing critical plan and damages only after processing', function()
        local fangRush = require('scripts/actions/mobskills/fang_rush')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        assert(fangRush.onMobSkillCheck(target, mob, {}) == 0)
        assert(fangRush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(params.canCrit and params.criticalChance[1] == 0.1 and params.criticalChance[2] == 0.2 and params.criticalChance[3] == 0.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(fangRush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
