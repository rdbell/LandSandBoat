require('scripts/actions/mobskills/overdrive')
describe('Overdrive mob skill', function()
    it('sets USES and returns OVERDRIVE', function()
        local skill = require('scripts/actions/mobskills/overdrive')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.OVERDRIVE)
        assert(msg == xi.msg.basic.USES)
    end)
end)
