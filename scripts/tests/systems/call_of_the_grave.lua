require('scripts/actions/mobskills/call_of_the_grave')
describe('Call of the Grave mob skill', function()
    it('applies INT_DOWN', function()
        local skill = require('scripts/actions/mobskills/call_of_the_grave')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.INT_DOWN)
        assert(status[1] == xi.effect.INT_DOWN and status[2] == 10 and status[3] == 5 and status[4] == 120)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
