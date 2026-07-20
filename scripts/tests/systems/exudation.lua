require('scripts/actions/mobskills/exudation')

describe('Exudation mob skill', function()
    it('uses its one-hit Blunt plan and damages only after processing', function()
        local exudation = require('scripts/actions/mobskills/exudation')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        assert(exudation.onMobSkillCheck(target, mob, {}) == 0)
        assert(exudation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.8 and params.fTP[2] == 2.8 and params.fTP[3] == 2.8)
        assert(params.attackMultiplier[1] == 1.5 and params.attackMultiplier[2] == 3.625 and params.attackMultiplier[3] == 4.75)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(exudation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
