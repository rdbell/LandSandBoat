require('scripts/actions/mobskills/reaving_wind')
describe('Reaving Wind mob skill', function()
    it('always allows use and sets target TP to zero', function()
        local wind = require('scripts/actions/mobskills/reaving_wind')
        local tp, message = nil, nil
        local target = { setTP = function(_, value) tp = value end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(wind.onMobSkillCheck(target, {}, skill) == 0)
        assert(wind.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(tp == 0 and message == xi.msg.basic.TP_REDUCED)
    end)
end)
