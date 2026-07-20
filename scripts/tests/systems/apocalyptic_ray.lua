require('scripts/actions/mobskills/apocalyptic_ray')
describe('Apocalyptic Ray mob skill', function()
    it('applies Doom power 10 tick 3 duration 30', function()
        local skill = require('scripts/actions/mobskills/apocalyptic_ray')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.DOOM)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == xi.effect.DOOM and args[2] == 10 and args[3] == 3 and args[4] == 30)
        assert(message == 242)
    end)
end)
