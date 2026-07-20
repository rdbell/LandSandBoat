require('scripts/actions/mobskills/rock_smash')
describe('Rock Smash mob skill', function()
    it('requires non-zero animation sub and applies Petrification after processing', function()
        local smash = require('scripts/actions/mobskills/rock_smash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local animSub, params, damage, statusParams = 0, nil, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 0; assert(smash.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 1; assert(smash.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(smash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        smash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.PETRIFICATION and statusParams[6] == 45)
    end)
end)
