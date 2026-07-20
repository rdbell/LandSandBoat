require('scripts/actions/mobskills/petal_pirouette')

describe('Petal Pirouette mob skill', function()
    it('always allows use and sets target TP to zero', function()
        local pirouette = require('scripts/actions/mobskills/petal_pirouette')
        local tp, message = nil, nil
        local target = { setTP = function(_, value) tp = value end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(pirouette.onMobSkillCheck(target, {}, skill) == 0)
        assert(pirouette.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(tp == 0 and message == xi.msg.basic.TP_REDUCED)
    end)
end)
