require('scripts/actions/mobskills/cacodemonia')
describe('Cacodemonia mob skill', function()
    it('applies CURSE_I', function()
        local skill = require('scripts/actions/mobskills/cacodemonia')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.CURSE_I)
        assert(status[1] == xi.effect.CURSE_I and status[2] == 35 and status[4] == 60)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
