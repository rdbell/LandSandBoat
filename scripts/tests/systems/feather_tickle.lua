describe('Feather Tickle mob skill', function()
    it('allows use, sets target TP to zero, reports the reduction, and returns zero', function()
        local featherTickle = require('scripts/actions/mobskills/feather_tickle')
        local tp, message = nil, nil
        local target = { setTP = function(_, value) tp = value end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(featherTickle.onMobSkillCheck(target, {}, skill) == 0)
        assert(featherTickle.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(tp == 0 and message == xi.msg.basic.TP_REDUCED)
    end)
end)
