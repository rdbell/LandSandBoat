require('scripts/actions/mobskills/slipstream')
describe('Slipstream mob skill', function()
    it('applies TP-scaled status effect', function()
        local skillmod = require('scripts/actions/mobskills/slipstream')
        local status = xi.mobskills.mobStatusEffectMove
        local calc = xi.mobskills.calculateDuration
        local params, message = nil, nil
        local skill = {
            getTP = function() return 1000 end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.calculateDuration = function(a, b, c)
            assert(a == 1000 and b == 120 and c == 180)
            return 150
        end
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skillmod.onMobSkillCheck({}, {}, skill) == 0)
        assert(skillmod.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.ACCURACY_DOWN)
        xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration = status, calc
        assert(params[3] == xi.effect.ACCURACY_DOWN and params[4] == 25 and params[6] == 150 and message == 456)
    end)
end)
