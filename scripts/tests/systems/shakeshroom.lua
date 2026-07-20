require('scripts/actions/mobskills/shakeshroom')
describe('Shakeshroom mob skill', function()
    it('requires anim sub 2, applies Disease, sets final anim sub 3', function()
        local shroom = require('scripts/actions/mobskills/shakeshroom')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, anim, finalAnim = nil, nil, nil, 0, nil
        local mob = {
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 22 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { setFinalAnimationSub = function(_, v) finalAnim = v end }
        assert(shroom.onMobSkillCheck(target, mob, skill) == 1)
        anim = 2; assert(shroom.onMobSkillCheck(target, mob, skill) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 33, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(shroom.onMobWeaponSkill(mob, target, skill, {}) == 33)
        assert(params.fTP[1] == 1.5 and params.canCrit and damage == nil and statusParams == nil and finalAnim == 3)
        xi.mobskills.processDamage = function() return true end
        shroom.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 33 and statusParams[3] == xi.effect.DISEASE and statusParams[6] == 720)
    end)
end)
