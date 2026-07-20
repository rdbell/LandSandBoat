require('scripts/actions/mobskills/reactive_shield')
describe('Reactive Shield mob skill', function()
    it('always allows use and applies Shock Spikes with sampled power', function()
        local shield = require('scripts/actions/mobskills/reactive_shield')
        local buff, random = xi.mobskills.mobBuffMove, math.random
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        math.random = function(low, high) assert(low==20 and high==30); return 22 end
        assert(shield.onMobSkillCheck({}, mob, skill) == 0)
        assert(shield.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.SHOCK_SPIKES)
        xi.mobskills.mobBuffMove, math.random = buff, random
        assert(params[3] == 22 and params[5] == 180 and message == 456)
    end)
end)
