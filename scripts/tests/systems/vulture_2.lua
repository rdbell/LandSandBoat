require('scripts/actions/mobskills/vulture_2')
describe('Vulture 2 mob skill', function()
    it('sets NONE and returns 0', function()
        local skill = require('scripts/actions/mobskills/vulture_2')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0 and message == xi.msg.basic.NONE)
    end)
end)
