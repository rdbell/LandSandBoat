require('scripts/actions/mobskills/reactive_armor')
describe('Reactive Armor mob skill', function()
    it('always allows use and applies Shock Spikes with sampled power', function()
        local armor = require('scripts/actions/mobskills/reactive_armor')
        local buff, random = xi.mobskills.mobBuffMove, math.random
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        math.random = function(low, high) assert(low==20 and high==30); return 25 end
        assert(armor.onMobSkillCheck({}, mob, skill) == 0)
        assert(armor.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.SHOCK_SPIKES)
        xi.mobskills.mobBuffMove, math.random = buff, random
        assert(params[2] == xi.effect.SHOCK_SPIKES and params[3] == 25 and params[5] == 180 and message == 456)
    end)
end)
