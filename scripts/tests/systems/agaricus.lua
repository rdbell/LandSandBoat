require('scripts/actions/mobskills/agaricus')

describe('Agaricus mob skill', function()
    it('uses physical parameters and damages and applies Plague then Slow only after processing succeeds', function()
        local move, process, effect = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) effects[#effects + 1] = { ... } end
        local skill = require('scripts/actions/mobskills/agaricus')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(#effects == 2)
        assert(effects[1][3] == xi.effect.PLAGUE and effects[1][4] == 5 and effects[1][5] == 0 and effects[1][6] == 60)
        assert(effects[2][3] == xi.effect.SLOW and effects[2][4] == 2500 and effects[2][5] == 0 and effects[2][6] == 120)
    end)
end)
