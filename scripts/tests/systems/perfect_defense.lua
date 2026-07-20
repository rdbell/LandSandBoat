require('scripts/actions/mobskills/perfect_defense')

describe('Perfect Defense mob skill', function()
    it('always allows use and applies fixed Perfect Defense with USES message', function()
        local defense = require('scripts/actions/mobskills/perfect_defense')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
        end
        assert(defense.onMobSkillCheck({}, mob, skill) == 0)
        assert(defense.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PERFECT_DEFENSE)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.PERFECT_DEFENSE and params[3] == 10000 and params[4] == 3 and params[5] == 10)
        assert(message == xi.msg.basic.USES)
    end)
end)
