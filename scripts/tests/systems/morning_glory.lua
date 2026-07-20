require('scripts/actions/mobskills/morning_glory')
describe('Morning Glory mob skill', function()
    it('admits closed flower and uses crit physical plan', function()
        local skill = require('scripts/actions/mobskills/morning_glory')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 40, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local target = { takeDamage = function() end }
        local mob = { getAnimationSub = function() return 0 end, getWeaponDmg = function() return 20 end }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        mob.getAnimationSub = function() return 2 end
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.canCrit and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
