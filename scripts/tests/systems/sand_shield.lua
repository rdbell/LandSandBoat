require('scripts/actions/mobskills/sand_shield')
describe('Sand Shield mob skill', function()
    it('applies Defense Boost buff', function()
        local shield = require('scripts/actions/mobskills/sand_shield')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(shield.onMobSkillCheck({}, {}, skill) == 0)
        assert(shield.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.DEFENSE_BOOST and params[3] == 50 and params[5] == 90 and message == 456)
    end)
end)
