require('scripts/actions/mobskills/unblest_jambiya')
describe('Unblest Jambiya mob skill', function()
    it('requires animationSub 0 and drains HP after processing', function()
        local skill = require('scripts/actions/mobskills/unblest_jambiya')
        local move, process, drain = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getAnimationSub = function() return 1 end, getWeaponDmg = function() return 80 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobDrainMove = function() return 123 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(params.fTP[1] == 2.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and message == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123)
    end)
end)
