require('scripts/actions/mobskills/damnation_dive_nm')

describe('Damnation Dive NM mob skill', function()
    it('uses a critical physical plan and applies Stun only after processing succeeds', function()
        local move, process, effect = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getWeaponDmg = function() return 42 end }
        local target = { takeDamage = function(_, value) damage = value end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        local skill = require('scripts/actions/mobskills/damnation_dive_nm')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 42 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(params.canCrit and params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(damage == nil and applied == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage == 123 and applied[3] == xi.effect.STUN and applied[4] == 1 and applied[5] == 0 and applied[6] == 15)
    end)
end)
