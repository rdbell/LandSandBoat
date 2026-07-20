require('scripts/actions/mobskills/queasyshroom')
describe('Queasyshroom mob skill', function()
    it('requires animation sub 0, applies Poison after processing, and sets final animation sub', function()
        local shroom = require('scripts/actions/mobskills/queasyshroom')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local animSub, params, damage, statusParams, finalSub = 0, nil, nil, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getWeaponDmg = function() return 77 end,
            getMainLvl = function() return 50 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { setFinalAnimationSub = function(_, value) finalSub = value end }
        animSub = 1; assert(shroom.onMobSkillCheck(target, mob, skill) == 1)
        animSub = 0; assert(shroom.onMobSkillCheck(target, mob, skill) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(shroom.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.canCrit and params.fTP[1] == 1.5 and statusParams == nil and finalSub == 1)
        xi.mobskills.processDamage = function() return true end
        shroom.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.POISON and statusParams[4] == 6 and statusParams[5] == 3)
    end)
end)
