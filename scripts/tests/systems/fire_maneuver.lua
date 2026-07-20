describe('Fire Maneuver mob skill', function()
    it('allows use and sets the USES message', function()
        local fireManeuver = require('scripts/actions/mobskills/fire_maneuver')
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        assert(fireManeuver.onMobSkillCheck({}, {}, skill) == 0)
        fireManeuver.onMobWeaponSkill({}, {}, skill, {})
        assert(message == xi.msg.basic.USES)
    end)
end)
