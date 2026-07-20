require('scripts/actions/mobskills/transmogrification')
describe('Transmogrification mob skill', function()
    it('requires animationSub 0 and buffs PHYSICAL_SHIELD power 2', function()
        local skill = require('scripts/actions/mobskills/transmogrification')
        local buff = xi.mobskills.mobBuffMove
        local message, buffParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getAnimationSub = function() return 1 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            buffParams = { effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0 and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(buffParams[1] == xi.effect.PHYSICAL_SHIELD and buffParams[2] == 2 and buffParams[4] == 30)
    end)
end)
