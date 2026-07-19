require('scripts/actions/mobskills/damnation_dive')

describe('Damnation Dive mob skill', function()
    it('only permits Ghrah in bird form', function()
        local skill = require('scripts/actions/mobskills/damnation_dive')
        assert(skill.onMobSkillCheck({}, { getFamily = function() return xi.mobFamily.GHRAH end, getAnimationSub = function() return 2 end }, {}) == 1)
        assert(skill.onMobSkillCheck({}, { getFamily = function() return xi.mobFamily.GHRAH end, getAnimationSub = function() return 3 end }, {}) == 0)
        assert(skill.onMobSkillCheck({}, { getFamily = function() return 0 end, getAnimationSub = function() return 0 end }, {}) == 0)
    end)

    it('uses physical parameters and applies Stun only after processing succeeds', function()
        local move, process, effect = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getWeaponDmg = function() return 42 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        local skill = require('scripts/actions/mobskills/damnation_dive')
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 42 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and applied == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and applied[3] == xi.effect.STUN and applied[4] == 1 and applied[5] == 0 and applied[6] == 15)
    end)
end)
