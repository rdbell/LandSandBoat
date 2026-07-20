require('scripts/actions/mobskills/perfect_dodge')

describe('Perfect Dodge mob skill', function()
    it('always allows use and applies fixed Perfect Dodge with USES message', function()
        local dodge = require('scripts/actions/mobskills/perfect_dodge')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
        end
        assert(dodge.onMobSkillCheck({}, mob, skill) == 0)
        assert(dodge.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PERFECT_DODGE)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.PERFECT_DODGE and params[3] == 1 and params[4] == 0 and params[5] == 30)
        assert(message == xi.msg.basic.USES)
    end)
end)
