require('scripts/actions/mobskills/bane')
describe('Bane mob skill', function()
    it('applies Bane power 65 for 600s', function()
        local skill = require('scripts/actions/mobskills/bane')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BANE)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == xi.effect.BANE and args[2] == 65 and args[4] == 600)
        assert(message == 242)
    end)
end)
