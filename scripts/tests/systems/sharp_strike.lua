require('scripts/actions/mobskills/sharp_strike')
describe('Sharp Strike mob skill', function()
    it('applies Attack Boost buff', function()
        local strike = require('scripts/actions/mobskills/sharp_strike')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(strike.onMobSkillCheck({}, {}, skill) == 0)
        assert(strike.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.ATTACK_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.ATTACK_BOOST and params[3] == 50 and params[5] == 180 and message == 456)
    end)
end)
