require('scripts/actions/mobskills/sonic_boom')
describe('Sonic Boom mob skill', function()
    it('skips when Attack Down present else applies TP-scaled Attack Down', function()
        local boom = require('scripts/actions/mobskills/sonic_boom')
        local status = xi.mobskills.mobStatusEffectMove
        local calc = xi.mobskills.calculateDuration
        local hasAD, params, message = true, nil, nil
        local target = { hasStatusEffect = function(_, e) return e == xi.effect.ATTACK_DOWN and hasAD end }
        local skill = {
            getTP = function() return 1000 end,
            setMsg = function(_, value) message = value end,
        }
        assert(boom.onMobSkillCheck(target, {}, skill) == 0)
        assert(boom.onMobWeaponSkill({}, target, skill, {}) == nil)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        hasAD = false
        xi.mobskills.calculateDuration = function(a, b, c)
            assert(a == 1000 and b == 180 and c == 360)
            return 270
        end
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(boom.onMobWeaponSkill({}, target, skill, {}) == xi.effect.ATTACK_DOWN)
        xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration = status, calc
        assert(params[3] == xi.effect.ATTACK_DOWN and params[4] == 25 and params[6] == 270 and message == 456)
    end)
end)
