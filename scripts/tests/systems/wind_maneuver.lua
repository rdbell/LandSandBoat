require('scripts/actions/mobskills/wind_maneuver')
describe('Wind Maneuver mob skill', function()
    it('sets USES message', function()
        local skill = require('scripts/actions/mobskills/wind_maneuver')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        skill.onMobWeaponSkill({}, {}, sk, {})
        assert(message == xi.msg.basic.USES)
    end)
end)
