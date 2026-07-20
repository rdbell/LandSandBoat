require('scripts/actions/mobskills/eclosion')

describe('Eclosion mob skill', function()
    it('schedules source disappearance and reports no message', function()
        local eclosion = require('scripts/actions/mobskills/eclosion')
        local timer, message = nil, nil
        local source = {}
        source.timer = function(_, delay, callback) timer = delay; callback(source) end
        source.setStatus = function(_, value) source.status = value end
        source.getID = function() return 0 end
        local skill = { setMsg = function(_, value) message = value end }

        assert(eclosion.onMobSkillCheck({}, source, skill) == 0)
        assert(eclosion.onMobWeaponSkill(source, {}, skill, {}) == 0)

        assert(timer == 4000 and source.status == xi.status.INVISIBLE)
        assert(message == xi.msg.basic.NONE)
    end)
end)
