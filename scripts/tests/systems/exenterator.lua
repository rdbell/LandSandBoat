require('scripts/actions/mobskills/exenterator')

describe('Exenterator mob skill', function()
    it('uses its four-hit Piercing plan and applies TP-scaled Accuracy Down only after processing', function()
        local exenterator = require('scripts/actions/mobskills/exenterator')
        local physicalMove, processDamage, effectMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        assert(exenterator.onMobSkillCheck(target, mob, skill) == 0)
        assert(exenterator.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 4 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4 and damage == nil and applied == nil)
        xi.mobskills.processDamage = function() return true end
        assert(exenterator.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, effectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(applied[3] == xi.effect.ACCURACY_DOWN and applied[4] == 20 and applied[5] == 0 and applied[6] == 90)
    end)
end)
