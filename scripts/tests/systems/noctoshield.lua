require('scripts/actions/mobskills/noctoshield')

describe('Noctoshield mob skill', function()
    it('always allows use and applies its fixed Phalanx self-buff', function()
        local shield = require('scripts/actions/mobskills/noctoshield')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end

        assert(shield.onMobSkillCheck({}, mob, skill) == 0)
        assert(shield.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PHALANX)
        xi.mobskills.mobBuffMove = buff

        assert(params[1] == mob and params[2] == xi.effect.PHALANX and params[3] == 13 and params[4] == 0 and params[5] == 120)
        assert(message == 456)
    end)
end)
