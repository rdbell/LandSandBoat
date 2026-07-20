require('scripts/actions/mobskills/earth_maneuver')

describe('Earth Maneuver mob skill', function()
    it('allows use and sets the fixed uses message', function()
        local maneuver = require('scripts/actions/mobskills/earth_maneuver')
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }

        assert(maneuver.onMobSkillCheck({}, {}, skill) == 0)
        maneuver.onMobWeaponSkill({}, {}, skill, {})
        assert(message == xi.msg.basic.USES and message == 101)
    end)
end)
