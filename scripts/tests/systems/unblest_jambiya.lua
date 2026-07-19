require('scripts/actions/mobskills/unblest_jambiya')

describe('Unblest Jambiya mob skill', function()
    it('requires an unbroken sword and drains HP after processed physical damage', function()
        local unblestJambiya = require('scripts/actions/mobskills/unblest_jambiya')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local originalDrainMove = xi.mobskills.mobDrainMove
        local params, drain, message = nil, nil, nil
        local mob = {
            getAnimationSub = function() return 0 end,
            getWeaponDmg = function() return 77 end,
        }
        local brokenMob = { getAnimationSub = function() return 1 end }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, got)
            params = got
            return { damage = 123 }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobDrainMove = function(_, _, drainType, damage)
            drain = { drainType, damage }
            return 456
        end

        assert(unblestJambiya.onMobSkillCheck(nil, mob, skill) == 0)
        assert(unblestJambiya.onMobSkillCheck(nil, brokenMob, skill) == 1)
        assert(unblestJambiya.onMobWeaponSkill(mob, {}, skill, {}) == 123)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage
        xi.mobskills.mobDrainMove = originalDrainMove

        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2.0 and params.fTP[2] == 2.0 and params.fTP[3] == 2.0)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(drain[1] == xi.mobskills.drainType.HP and drain[2] == 123)
        assert(message == 456)
    end)

    it('does not message a drain when damage processing fails', function()
        local unblestJambiya = require('scripts/actions/mobskills/unblest_jambiya')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local originalDrainMove = xi.mobskills.mobDrainMove
        local drained, message = false, nil
        local mob = { getWeaponDmg = function() return 1 end }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobPhysicalMove = function() return { damage = 99 } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function()
            drained = true
            return 1
        end

        assert(unblestJambiya.onMobWeaponSkill(mob, {}, skill, {}) == 99)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage
        xi.mobskills.mobDrainMove = originalDrainMove

        assert(not drained and message == nil)
    end)
end)
