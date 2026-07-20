require('scripts/actions/mobskills/rotten_stench')
describe('Rotten Stench mob skill', function()
    it('applies accuracy downs with TP-scaled duration and selects return message', function()
        local stench = require('scripts/actions/mobskills/rotten_stench')
        local status = xi.mobskills.mobStatusEffectMove
        local calls, message, tp, ret = {}, nil, 1000, xi.msg.basic.SKILL_ENFEEB_IS
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        local i = 0
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls+1] = { ... }
            i = i + 1
            if i == 1 then return ret end
            return 0
        end
        assert(stench.onMobSkillCheck({}, {}, skill) == 0)
        assert(stench.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.ACCURACY_DOWN)
        assert(calls[1][3] == xi.effect.ACCURACY_DOWN and calls[1][4] == 50 and calls[1][6] == 90)
        assert(calls[2][3] == xi.effect.MAGIC_ACC_DOWN and message == xi.msg.basic.SKILL_ENFEEB)
        i, ret, calls = 0, 0, {}
        assert(stench.onMobWeaponSkill({}, {}, skill, {}) == 0)
        xi.mobskills.mobStatusEffectMove = status
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
