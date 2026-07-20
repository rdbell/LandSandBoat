require('scripts/actions/mobskills/envoutement')

describe('Envoutement mob skill', function()
    it('uses its physical plan and applies Curse only after processing', function()
        local envoutement = require('scripts/actions/mobskills/envoutement')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, curse = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) curse = { effect, power, tick, duration } end

        assert(envoutement.onMobSkillCheck({}, {}, {}) == 0)
        assert(envoutement.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3.2 and params.fTP[2] == 3.2 and params.fTP[3] == 3.2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and curse == nil)

        xi.mobskills.processDamage = function() return true end
        assert(envoutement.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING and curse[1] == xi.effect.CURSE_I and curse[2] == 25 and curse[3] == 0 and curse[4] == 180)
    end)
end)
