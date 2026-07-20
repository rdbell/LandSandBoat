require('scripts/actions/mobskills/plasma_charge')

describe('Plasma Charge mob skill', function()
    it('always allows use and applies Shock Spikes with sampled power', function()
        local charge = require('scripts/actions/mobskills/plasma_charge')
        local buff, random = xi.mobskills.mobBuffMove, math.random
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        math.random = function(low, high)
            assert(low == 15 and high == 30)
            return 22
        end
        assert(charge.onMobSkillCheck({}, mob, skill) == 0)
        assert(charge.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.SHOCK_SPIKES)
        xi.mobskills.mobBuffMove, math.random = buff, random
        assert(params[1] == mob and params[2] == xi.effect.SHOCK_SPIKES and params[3] == 22 and params[4] == 0 and params[5] == 60)
        assert(message == 456)
    end)
end)
