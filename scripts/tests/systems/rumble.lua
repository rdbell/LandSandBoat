require('scripts/actions/mobskills/rumble')
describe('Rumble mob skill', function()
    it('applies TP-scaled Evasion Down', function()
        local rumble = require('scripts/actions/mobskills/rumble')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message, tp = nil, nil, 1000
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(rumble.onMobSkillCheck({}, {}, skill) == 0)
        assert(rumble.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.EVASION_DOWN)
        assert(params[3] == xi.effect.EVASION_DOWN and params[4] == 10 and params[6] == 45)
        tp = 2000
        rumble.onMobWeaponSkill({}, {}, skill, {})
        xi.mobskills.mobStatusEffectMove = status
        assert(params[6] == 210 and message == 456)
    end)
end)
