describe('Ice Maneuver mob skill', function()
    it('allows use and sets the USES message', function()
        local maneuver = require('scripts/actions/mobskills/ice_maneuver')
        local message
        local skill = { setMsg = function(_, value) message = value end }

        assert(maneuver.onMobSkillCheck({}, {}, skill) == 0)
        assert(maneuver.onMobWeaponSkill({}, {}, skill, {}) == nil)
        assert(message == xi.msg.basic.USES)
    end)
end)
