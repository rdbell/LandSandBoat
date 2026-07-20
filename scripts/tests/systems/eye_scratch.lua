require('scripts/actions/mobskills/eye_scratch')

describe('Eye Scratch mob skill', function()
    it('uses its one-hit Slashing plan and applies Blindness only after processing', function()
        local eyeScratch = require('scripts/actions/mobskills/eye_scratch')
        local physicalMove, processDamage, effectMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        assert(eyeScratch.onMobSkillCheck(target, mob, {}) == 0)
        assert(eyeScratch.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and applied == nil)
        xi.mobskills.processDamage = function() return true end
        assert(eyeScratch.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, effectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(applied[3] == xi.effect.BLINDNESS and applied[4] == 30 and applied[5] == 0 and applied[6] == 60)
    end)
end)
