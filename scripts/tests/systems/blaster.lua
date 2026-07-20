require('scripts/actions/mobskills/blaster')
describe('Blaster mob skill', function()
    it('applies Paralysis power 70 for 60s', function()
        local skill = require('scripts/actions/mobskills/blaster')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == xi.effect.PARALYSIS and args[2] == 70 and args[4] == 60)
        assert(message == 242)
    end)
end)
