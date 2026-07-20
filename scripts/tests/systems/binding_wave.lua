require('scripts/actions/mobskills/binding_wave')
describe('Binding Wave mob skill', function()
    it('applies Bind with TP-scaled duration from mob TP', function()
        local skill = require('scripts/actions/mobskills/binding_wave')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local mob = { getTP = function() return 1000 end }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.BIND)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == xi.effect.BIND and args[2] == 1 and args[4] == 30)
        assert(message == 242)
    end)
end)
