require('scripts/actions/mobskills/vile_belch')
describe('Vile Belch mob skill', function()
    it('applies TP-scaled plague then silence and returns plague', function()
        local skill = require('scripts/actions/mobskills/vile_belch')
        local status = xi.mobskills.mobStatusEffectMove
        local message, calls = nil, {}
        local sk = { setMsg = function(_, v) message = v end, getTP = function() return 1000 end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
            return 100 + #calls
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PLAGUE and message == 102)
        xi.mobskills.mobStatusEffectMove = status
        assert(calls[1][3] == xi.effect.PLAGUE and calls[1][4] == 10 and calls[1][5] == 3 and calls[1][6] == 15)
        assert(calls[2][3] == xi.effect.SILENCE and calls[2][4] == 1 and calls[2][6] == 30)
    end)
end)
