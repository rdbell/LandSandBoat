require('scripts/actions/mobskills/shell_guard')
describe('Shell Guard mob skill', function()
    it('applies Defense Boost with TP-scaled duration', function()
        local guard = require('scripts/actions/mobskills/shell_guard')
        local buff = xi.mobskills.mobBuffMove
        local calc = xi.mobskills.calculateDuration
        local params, message, tp = nil, nil, 1000
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.calculateDuration = function(a, b, c)
            assert(a == 1000 and b == 600 and c == 1080)
            return 840
        end
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(guard.onMobSkillCheck({}, {}, skill) == 0)
        assert(guard.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove, xi.mobskills.calculateDuration = buff, calc
        assert(params[2] == xi.effect.DEFENSE_BOOST and params[3] == 25 and params[5] == 840 and message == 456)
    end)
end)
