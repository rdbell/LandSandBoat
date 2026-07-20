require('scripts/actions/mobskills/choke_chain')
describe('Choke Chain mob skill', function()
    it('admits animationSub 0 and applies Bind Silence Amnesia', function()
        local skill = require('scripts/actions/mobskills/choke_chain')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local statuses, msg = {}, nil
        local mob = { getAnimationSub = function() return 1 end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            statuses[#statuses+1] = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.AMNESIA)
        assert(#statuses == 3)
        assert(statuses[1][1] == xi.effect.BIND and statuses[2][1] == xi.effect.SILENCE and statuses[3][1] == xi.effect.AMNESIA)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
