require('scripts/actions/mobskills/call_of_the_wyrmking')
describe('Call of the Wyrmking mob skill', function()
    it('sets NONE message and returns zero', function()
        local skill = require('scripts/actions/mobskills/call_of_the_wyrmking')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
