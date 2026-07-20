require('scripts/actions/mobskills/axe_throw')
describe('Axe Throw mob skill', function()
    it('admits armed BST and sets final animationSub 2', function()
        local skill = require('scripts/actions/mobskills/axe_throw')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, finalSub = nil, nil, nil
        local mob = {
            getAnimationSub = function() return 1 end,
            getMainJob = function() return xi.job.BST end,
            getWeaponDmg = function() return 50 end,
        }
        local sk = { setFinalAnimationSub = function(_, v) finalSub = v end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.RANGED, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 60)
        assert(params.attackType == xi.attackType.RANGED and params.skipParry and params.fTP[1] == 1.0)
        assert(finalSub == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 60)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 60)
    end)
end)
