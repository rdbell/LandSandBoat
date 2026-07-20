require('scripts/actions/mobskills/stave_toss')
describe('Stave Toss mob skill', function()
    it('admits staff mage with weapon and sets final anim sub 2', function()
        local skill = require('scripts/actions/mobskills/stave_toss')
        local params, anim = nil, nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 50, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = {
            getAnimationSub = function() return 0 end,
            getMainJob = function() return xi.job.BLM end,
            getWeaponDmg = function() return 40 end,
        }
        local sk = { setFinalAnimationSub = function(_, a) anim = a end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 50)
        assert(params.fTP[1] == 1.0 and anim == 2)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
