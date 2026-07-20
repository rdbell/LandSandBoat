require('scripts/actions/mobskills/refueling')
describe('Refueling mob skill', function()
    it('always allows use and applies fixed Haste self-buff', function()
        local refuel = require('scripts/actions/mobskills/refueling')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(refuel.onMobSkillCheck({}, mob, skill) == 0)
        assert(refuel.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.HASTE)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.HASTE and params[3] == 2000 and params[4] == 0 and params[5] == 180 and message == 456)
    end)
end)
