require('scripts/actions/mobskills/thunderous_yowl')
describe('Thunderous Yowl mob skill', function()
    it('applies plague then curse and returns curse message', function()
        local skill = require('scripts/actions/mobskills/thunderous_yowl')
        local status = xi.mobskills.mobStatusEffectMove
        local message, calls = nil, {}
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
            return 456
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.CURSE_I and message == 456)
        xi.mobskills.mobStatusEffectMove = status
        assert(calls[1][3] == xi.effect.PLAGUE and calls[1][4] == 5 and calls[1][5] == 3 and calls[1][6] == 60)
        assert(calls[2][3] == xi.effect.CURSE_I and calls[2][4] == 25 and calls[2][6] == 60)
    end)
end)
