require('scripts/actions/mobskills/warp_out_adjido')
describe('Warp Out Adjido mob skill', function()
    it('skill check 0 and sets NONE', function()
        local skill = require('scripts/actions/mobskills/warp_out_adjido')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
