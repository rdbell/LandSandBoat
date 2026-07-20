require('scripts/actions/mobskills/entropy')

describe('Entropy mob skill', function()
    it('uses its four-hit plan and grants floored MP only after processing', function()
        local entropy = require('scripts/actions/mobskills/entropy')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, mp = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end, addMP = function(_, value) mp = value end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end

        assert(entropy.onMobSkillCheck({}, {}, {}) == 0)
        assert(entropy.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 4 and params.fTP[1] == 0.75 and params.fTP[2] == 1.25 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4 and damage == nil and mp == nil)

        xi.mobskills.processDamage = function() return true end
        assert(entropy.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING and mp == 24)
    end)
end)
