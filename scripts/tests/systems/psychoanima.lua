require('scripts/actions/mobskills/psychoanima')

describe('Psychoanima mob skill', function()
    it('always allows use and applies Physical Shield with USES message', function()
        local anima = require('scripts/actions/mobskills/psychoanima')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
        end
        assert(anima.onMobSkillCheck({}, mob, skill) == 0)
        assert(anima.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PHYSICAL_SHIELD)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.PHYSICAL_SHIELD and params[3] == 1 and params[4] == 0 and params[5] == 10)
        assert(message == xi.msg.basic.USES)
    end)
end)
