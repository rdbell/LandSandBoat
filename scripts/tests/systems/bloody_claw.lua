require('scripts/actions/mobskills/bloody_claw')
describe('Bloody Claw mob skill', function()
    it('admits animationSub 4 and uses physical drain with random attr down', function()
        local skill = require('scripts/actions/mobskills/bloody_claw')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local drainMove = xi.mobskills.mobDrainMove
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, drain, status, message, randomCalls = nil, nil, nil, nil, 0
        local origRandom = math.random
        math.random = function(a, b)
            if a == 0 and b == 6 then
                randomCalls = randomCalls + 1
                return 3
            end
            return origRandom(a, b)
        end
        local mob = { getAnimationSub = function() return 0 end, getWeaponDmg = function() return 50 end }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getAnimationSub = function() return 4 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobDrainMove = function(_, _, dtype, amount)
            drain = { dtype, amount }
            return 123
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        assert(params.numHits == 3 and params.fTP[1] == 0.9)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.damageType == xi.damageType.BLUNT)
        assert(drain == nil and status == nil and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        assert(drain[1] == xi.mobskills.drainType.HP and drain[2] == 90)
        assert(status[1] == xi.effect.STR_DOWN + 3 and status[2] == 20 and status[3] == 9 and status[4] == 120)
        assert(message == 123)
        assert(randomCalls == 1)
        math.random = origRandom
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobDrainMove = drainMove
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
