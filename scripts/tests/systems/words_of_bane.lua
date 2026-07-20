require('scripts/actions/mobskills/words_of_bane')
describe('Words of Bane mob skill', function()
    it('applies Curse_I power 25 for 45s', function()
        local skill = require('scripts/actions/mobskills/words_of_bane')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(mob, target, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.CURSE_I)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == xi.effect.CURSE_I and args[2] == 25 and args[3] == 0 and args[4] == 45)
        assert(message == 242)
    end)
end)
