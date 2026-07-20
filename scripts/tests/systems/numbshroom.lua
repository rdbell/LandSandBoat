require('scripts/actions/mobskills/numbshroom')

describe('Numbshroom mob skill', function()
    it('requires animation sub 1 and paralyzes only after processing with final anim-sub 2', function()
        local shroom = require('scripts/actions/mobskills/numbshroom')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, paralysis, animSub, finalAnim = nil, nil, nil, 0, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { setFinalAnimationSub = function(_, value) finalAnim = value end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) paralysis = { ... } end
        assert(shroom.onMobSkillCheck(target, mob, skill) == 1)
        animSub = 1
        assert(shroom.onMobSkillCheck(target, mob, skill) == 0)
        assert(shroom.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1.5 and params.canCrit)
        assert(params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and paralysis == nil)
        assert(finalAnim == 2)
        xi.mobskills.processDamage = function() return true end
        finalAnim = nil
        shroom.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(paralysis[3] == xi.effect.PARALYSIS and paralysis[4] == 25 and paralysis[5] == 0 and paralysis[6] == 180)
        assert(finalAnim == 2)
    end)
end)
