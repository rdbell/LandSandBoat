require('scripts/actions/mobskills/trance')
describe('Trance mob skill', function()
    it('sets USES and returns TRANCE', function()
        local skill = require('scripts/actions/mobskills/trance')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.TRANCE and message == xi.msg.basic.USES)
    end)
end)
