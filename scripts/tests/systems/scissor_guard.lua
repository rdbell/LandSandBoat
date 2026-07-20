require('scripts/actions/mobskills/scissor_guard')
describe('Scissor Guard mob skill', function()
    it('applies Defense Boost buff on target', function()
        local guard = require('scripts/actions/mobskills/scissor_guard')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(guard.onMobSkillCheck({}, {}, skill) == 0)
        assert(guard.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.DEFENSE_BOOST and params[3] == 100 and params[5] == 60 and message == 456)
    end)
end)
