require('scripts/actions/mobskills/random_needles')
describe('Random Needles mob skill', function()
    it('uses physical WIPE_SHADOWS skipPDIF plan with random needle base', function()
        local skill = require('scripts/actions/mobskills/random_needles')
        local params = nil
        local origP, origD, origR = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, math.random
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = p.baseDamage, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        math.random = function(a, b) return 2000 end
        local mob = { getID = function() return 0 end }
        local sk = { getTotalTargets = function() return 2 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 1000)
        assert(params.baseDamage == 1000 and params.skipPDIF and params.guaranteedFirstHit)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, math.random = origP, origD, origR
    end)
end)
